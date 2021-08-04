(bomblab真有意思
### 1.phase1 ###
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

### 2.phase2 ###
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

### 3.phase3 ###
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

### 4.phase4 ###
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

### 5.phase5 ###
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

### 6.phase6 ###
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

### 7.secret phase ###
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
