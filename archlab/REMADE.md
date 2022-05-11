## lab4-archlab ##
### 环境安装 ###
```sh
#下载archlab-handout，并在当前目录解压，然后进入目录解压sim
cd sim
sudo apt install flex
sudo apt install bison
sudo apt install tcl tcl-dev
sudo apt install tk tk-dev
cd sim
make clean; make
#至此环境就配好了
```

### part-A ###
这一部分进入misc目录进行工作，使用y86-64汇编编写sum.ys/rsum.ys/copy.ys
```c
/* linked list element */
typedef struct ELE {

	long val;
	struct ELE *next;

} *list_ptr;
/* sum_list - Sum the elements of a linked list */
	long sum_list(list_ptr ls)
	{
		long val = 0;
		while (ls) {
		val += ls->val;
		ls = ls->next;
		}
		return val;
	}

	/* rsum_list - Recursive version of sum_list */
	long rsum_list(list_ptr ls)
	{
		if (!ls)
		return 0;
		else {
			long val = ls->val;
			long rest = rsum_list(ls->next);
			return val + rest;
		}
	}

	/* copy_block - Copy src to dest and return xor checksum of src */
	long copy_block(long *src, long *dest, long len)
	{
		long result = 0;
		while (len > 0) {
			long val = *src++;
		*dest++ = val;
		result ˆ= val;
		len--;
	}
	return result;
}

```
sum.ys如下(注意在loop结尾加上判断就可以了)
```sh
#long suml_list(list_ptr ls)
#{
#	long val = 0;
#	while (ls) {
#		val += ls->val;
#		ls = ls->next;
#	}
#	return val;

	.pos 0
	irmovq stack,%rsp
	call main
	halt

# Sample linked list
	.align 8
ele1:
	.quad 0x0a
	.quad ele2
ele2:
	.quad 0xb0
	.quad ele3
ele3:
	.quad 0xc00
	.quad 0

main:
	irmovq ele1,%rdi
	call sum
	ret
sum:
	irmovq $8,%r8
	xorq %rax,%rax
	andq %rdi,%rdi
	jmp test
loop:
	mrmovq (%rdi),%r9
	addq %r9,%rax
	addq %r8,%rdi
	mrmovq (%rdi),%rdi
	andq %rdi,%rdi
test:
	jne loop
	ret

	.pos 0x200
stack:
```
rsum.ys如下(需要注意的地方是需要使用栈来保存参数)
```sh
#long rsum_list(list_ptr ls)
#{
#	if (!ls)
#		return 0;
#	else {
#		long val = ls->val;
#		long rest = rsum_lsit(ls->next);
#		return val + rest;
#	}
#}
# ls - %rdi

	.pos 0
	irmovq stack,%rsp
	call main
	halt
# Sample linked list
	.align 8
ele1:
	.quad 0x00a
	.quad ele2
ele2:
	.quad 0x0b0
	.quad ele3
ele3:
	.quad 0xc00
	.quad 0

main:
	irmovq ele1,%rdi
	call rsum_list
	ret
rsum_list:
	andq %rdi,%rdi
	je exit
	mrmovq (%rdi),%r10
	pushq %r10
	mrmovq 8(%rdi),%rdi
	call rsum_list
	popq %r10
	addq %r10,%rax	
exit:
	ret
	.pos 0x200
stack:
```
copy.ys如下
```sh
#long copy_block(long *src, long *dest, long len)
#{
#	long result = 0;
#	while (len > 0) {
#		long val = *src++;
#		*dest++ = val;
#		result ^= val;
#		len--;
#	}
#	return result;
#}
    .pos 0
    irmovq stack, %rsp
    call main        
    halt

    .align 8
src:
	.quad 0x2222333
	.quad 0x4455666
	.quad 0x7788999
	.quad 0xaabbccc
	.quad 0xddeefff
dest:
	.quad 0xaabbccc
	.quad 0xddeefff
	.quad 0x9988977
	.quad 0x1122333
	.quad 0x4455666
main:	
	irmovq src, %rdi
    	irmovq dest, %rsi
    	irmovq $5, %rdx
	call copy
	ret
copy:
	irmovq $8,%r8
	irmovq $1,%r9
	xorq %rax,%rax
	addq %rdx,%rdx
	jmp test
loop:
	mrmovq (%rdi),%r10
	addq %r8,%rdi
	rmmovq %r10,(%rsi)
	addq %r8,%rsi
	xorq %r10,%rax
	subq %r8,%rdx
test:
	jne loop
	ret

	.pos 0x200
stack:
	
```

### part-B ###
任务要求是扩展SEQ处理器支持iaddq，需要修改seq/seq-full.hcl文件
iaddq类似于mrmovq，原型可以看出iaddq D,rB如图
处理如下:
```sh
#取值：
	icode:ifun <-- M1[PC]
	rA:rB <- M1[PC + 1]
	valC <- M8[PC + 2]
	valP <- PC + 10
#译码:
	valB <- R[rB]
#执行:
	valE <- valB + valC
#写回:
	R[rB] <- valE
#更新PC:
	PC <- valP
```
```sh
bool instr_valid = icode in #判断是否有效，需要加上IIADDQ
	{ INOP, IHALT, IRRMOVQ, IIRMOVQ, IRMMOVQ, IMRMOVQ,
	       IOPQ, IJXX, ICALL, IRET, IPUSHQ, IPOPQ, IIADDQ }

bool need_regids = #确定icode的值是否为带有寄存器指示值字节的指令，所以这里要加上IIADDQ
		icode in { 	IRMOVQ, IOPQ, IPUSHQ,IPOPQ,
					IIRMOVQ, IRMOVQ, IMRMOVQ, IIADDQ };

#使用rB
word srcB = [
	icode in { IOPQ, IRMMOVQ, IMRMOVQ, IIADDQ  } : rB;
	icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RRSP;
	1 : RNONE;  # Don't need register
];

#dstE表示写端口E的目的寄存器
word dstE = [
	icode in { IRRMOVQ } && Cnd : rB;
	icode in { IIRMOVQ, IOPQ, IIADDQ} : rB;
	icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RRSP;
	1 : RNONE;  # Don't write any register
];

#后面两步是根据执行操作 valB + valC
## Select input A to ALU
word aluA = [
	icode in { IRRMOVQ, IOPQ } : valA;
	icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ, IIADDQ } : valC;
	icode in { ICALL, IPUSHQ } : -8;
	icode in { IRET, IPOPQ } : 8;
	# Other instructions don't need ALU
];

## Select input B to ALU
word aluB = [
	icode in { IRMMOVQ, IMRMOVQ, IOPQ, ICALL, 
		      IPUSHQ, IRET, IPOPQ, IIADDQ } : valB;
	icode in { IRRMOVQ, IIRMOVQ } : 0;
	
];

#类似于IOPQ
bool set_cc = icode in { IOPQ, IIADDQ };
```

### part-C ###
优化步骤:
	1.尽量使用iaddq
	2.循环展开
c代码如下:
```c
#include <stdio.h>

typedef word_t word_t;

word_t src[8], dst[8];

word_t ncopy(word_t *src, word_t *dst, word_t len)
{
    word_t count = 0;
    word_t val;


    while (len > 0) {

    	if(len == 1) {
			val = *src++;
			*dst++ = val;
			if (val > 0)
			    count++;
			len--;
		}
		val = *src++;
		*dst++ = val;
		if (val > 0)
		    count++;
		val = *src++;
		*dst++ = val;
		if (val > 0)
		    count++;
		len -= 2;
		
    }
    return count;
}
```
如下
```sh
ncopy:
	iaddq $0,%rdx
	jle Done
Loop:
	mrmovq (%rdi), %r10
	rmmovq %r10, (%rsi)
	andq %r10, %r10 # val <= 0?
	jle Npos # if so, goto Npos:
	iaddq $1, %rax
Npos:
	iaddq $8, %rdi
	iaddq $8, %rsi
	iaddq $-1, %rdx
	jg Loop
Done:
	ret
```
三次展开(jg> , jge >= , jle <=,  je <):
```sh
ncopy:
	andq %rdx, %rdx
	jle Done # len <= 0 -> Done rdx = 1 || rdx = 2 || rdx = 3
	iaddq $-3, %rdx # rdx -= 3
	jge L3
L0: # rdx == -1 || rdx == -2
	iaddq $2, %rdx
	jg L1 #
	mrmovq (%rdi), %r10
	rmmovq %r10, (%rsi)
	andq %r10, %r10 # val <= 0?
	jle Done
	iaddq $1, %rax
	ret
L1:
	mrmovq (%rdi), %r10
	rmmovq %r10, (%rsi)
	mrmovq $8(%rdi), %r11
	rmmovq %r11, $8(%rsi)
	andq %r10, %r10
	jle L2
	iaddq $1, %rax
L2:
	andq %r11, %r11
	jle Done
	iaddq $1, %rax
	ret

L3:
	mrmovq (%rdi), %r10
	rmmovq %r10, (%rsi)
	mrmovq $8(%rdi), %r11
	rmmovq %r11, $8(%rsi)
	mrmovq $16(%rdi), %r12
	rmmovq %r12, $16(%rsi)
	andq %r10, %r10
	jle L4
	iaddq $1, %rax
L4:
	andq %r11, %r11
	jle L5
	iaddq $1, %rax
L5:
	andq %r12, %r12
	jle L6
	iaddq $1, %rax
L6:	
	iaddq $24, %rdi
	iaddq $24, %rsi
	iaddq $0, %rdx
	jg ncopy
Done:
	ret
```
四次展开:
```sh
ncopy:
	andq %rdx, %rdx
	jle Done # len <= 0 -> Done rdx = 1 || rdx = 2 || rdx = 3
	iaddq $-4, %rdx # rdx -= 3
	jge L4
L0: # rdx == -1 || rdx == -2 || rdx == -3
	iaddq $2, %rdx
	jg L1
	iaddq $1, %rdx
	jg L22
	mrmovq (%rdi), %r10
	rmmovq %r10, (%rsi)	
	andq %r10, %r10 # val <= 0?
	jle Done
	iaddq $1, %rax
	ret
L22: #2
	mrmovq (%rdi), %r10
	rmmovq %r10, (%rsi)
	mrmovq $8(%rdi), %r11
	rmmovq %r11, $8(%rsi)
	andq %r10, %r10
	jle L23
	iaddq $1, %rax
L23:
	andq %r11, %r11
	jle Done
	iaddq $1, %rax
	ret
L1: # 3
	mrmovq (%rdi), %r10
	rmmovq %r10, (%rsi)
	mrmovq $8(%rdi), %r11
	rmmovq %r11, $8(%rsi)
	mrmovq $16(%rdi), %r12
	rmmovq %r12, $16(%rsi)
	andq %r10, %r10
	jle L2
	iaddq $1, %rax
L2:
	andq %r11, %r11
	jle L3
	iaddq $1, %rax
L3:
	andq %r12, %r12
	jle Done
	iaddq $1, %rax
	ret

L4:
	mrmovq (%rdi), %r10
	rmmovq %r10, (%rsi)
	mrmovq $8(%rdi), %r11
	rmmovq %r11, $8(%rsi)
	mrmovq $16(%rdi), %r12
	rmmovq %r12, $16(%rsi)
	mrmovq $24(%rdi), %r13
	rmmovq %r13, $24(%rsi)
	andq %r10, %r10
	jle L5
	iaddq $1, %rax
L5:
	andq %r11, %r11
	jle L6
	iaddq $1, %rax
L6:
	andq %r12, %r12
	jle L7
	iaddq $1, %rax
L7:
	andq %r13, %r13
	jle L8
	iaddq $1, %rax
L8:	
	iaddq $32, %rdi
	iaddq $32, %rsi
	iaddq $0, %rdx
	jg ncopy
Done:
	ret
```
