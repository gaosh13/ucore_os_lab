##练习题

---
###问题1

####操作系统镜像文件ucore.img是如何一步一步生成的？

使用make clean与make 'V='命令观察结果：

1. 依次会生成init.o（系统初始化），stdio.o（标准输入输入），readline.o（输入行），panic.o（一些调试宏），kdebug.o（源码与二进制查询、显示调用栈），clock.o（时钟控制器），console.o（串口和键盘），picirq.o（中断控制器），intr.o（屏蔽和使能中断），trap.o，vectors.o，trapentry.o（终端部分处理，中断例程入口以及两步处理），pmm.o（全局变量如gdt等），string.o（字符串处理），printfmt.o（输出），最后链接所有.o文件为bin/kernel

1. 生成bootasm.o（bootloader入口），bootmain.o（bootloader重要组成部分），sign.o（生成合法主引导扇区），链接boot生成bin/bootblock

1. 生成ucore.img

####一个被系统认为是符合规范的硬盘主引导扇区的特征是什么

查看源代码tools/sign.c可以得知合法的主引导扇区满足：

文件大小为512字节，且最后两个字节值分别为0x55和0xAA

---
###问题2

####使用qemu和gdb进行调试

使用make debug或make lab1-mon命令调试：

make lab1-mon断点为0x7c00，而make debug的断点设置在bootmain入口处，地址为0x7d0f

使用si或n进行单步调试，使用x /10i $pc输出最近10行汇编码，如0x7c02为xor %ax,%ax（即%ax清0），0x7c0c为test $0x2,%al

可以设置断点b *0x7d0f，在0x7c4a处调用bootmain函数，跳转至0x7d0f，此后运行至0x7d27调用位于0x7c72的readseg函数（加载到内存）

---
###问题3

####分析bootloader进入保护模式的过程

首先bootloader屏蔽中断

1. 开启A20

>A20地址线禁止时程序就像在8086中运行，1MB以上地址不可访问，进入保护模式必须要打开A20地址线

>等待并打开8042，设置P2端口可写，等待并设置P2端口的A20位为1，以打开A20地址线

1. 初始化GDT表

>使用lgdt指令加载GDT的基地址

1. 进入保护模式

>将寄存器cr0的使能打开，即CR0_PE_ON

>ljmp指令，执行下一条，并采取32位指令，由此进入了保护模式，此后还需要初始化段寄存器并进入bootmain

---
###问题4

####bootloader如何读取硬盘扇区

实现读取硬盘扇区在bootmain.c的readsect函数中，由readseg调用（从硬盘中分批次读取elf文件）

1. 首先等待硬盘准备完毕

1. 其次设置好参数（1个扇区，LBA参数表示sector number，读取命令），其中0x1F6的7至4位为1110，表示主盘LBA模式

1. 等待，同1

1. 调用insl，把磁盘扇区读到内存

####bootloader是如何加载ELF格式的OS

实现加载ELF格式的OS在bootmain函数

1. 首先从硬盘中读取ELF header，并通过e_magic判断是否为ELF header

1. 从header中获取program header的首地址header+e_phoff和末地址header+e_phoff+e_phnum，然后从硬盘中读取相应的数据到内存

1. 从程序的入口e_entry开始执行，接下来控制权交给ucore

---
###问题5

实现细节区别：

逐一对arg赋值（代码相较麻烦但易于理解），循环变量采取了uint8_t而不是int类型
<br>

运行结果略有不同
<blockquote>
<p>
ebp:0x00007b38 eip:0x00100a28 args:0x00010094 0x00010094 0x00007b68 0x0010007f<br>
    kern/debug/kdebug.c:295: print_stackframe+22<br>
ebp:0x00007b48 eip:0x00100d15 args:0x00000000 0x00000000 0x00000000 0x00007bb8<br>
    kern/debug/kmonitor.c:125: mon_backtrace+10<br>
ebp:0x00007b68 eip:0x0010007f args:0x00000000 0x00007b90 0xffff0000 0x00007b94<br>
    kern/init/init.c:48: grade_backtrace2+19<br>
ebp:0x00007b88 eip:0x001000a1 args:0x00000000 0xffff0000 0x00007bb4 0x00000029<br>
    kern/init/init.c:53: grade_backtrace1+27<br>
ebp:0x00007ba8 eip:0x001000be args:0x00000000 0x00100000 0xffff0000 0x00100043<br>
    kern/init/init.c:58: grade_backtrace0+19<br>
ebp:0x00007bc8 eip:0x001000df args:0x00000000 0x00000000 0x00000000 0x00103420<br>
    kern/init/init.c:63: grade_backtrace+26<br>
ebp:0x00007be8 eip:0x00100050 args:0x00000000 0x00000000 0x00000000 0x00007c4f<br>
    kern/init/init.c:28: kern_init+79<br>
ebp:0x00007bf8 eip:0x00007d6e args:0xc031fcfa 0xc08ed88e 0x64e4d08e 0xfa7502a8<br>
    `<unknow>`: -- 0x00007d6d --
</p>
</blockquote>

最后一行表示的是bootmain的栈信息，ebp为其栈帧，eip为其返回地址（在bootloader中，对应bootasm.S文件的spin死循环），因为调用bootmain时没有参数，所以参数部分对应的是bootloader的代码

---
###问题6

###中断描述符表中一个表项占多少字节？其中哪几位代表中断处理代码的入口？

根据gitbook上的Interrupt-gate descriptor可知：

一个表项占8字节，其中第33-48个字节为选择子，得到相应的段的基址后加上由1-16（高16位）、49-64（低16位）拼接的偏移得到中断代码入口

###完善idt_init

实现细节区别：

用256代替了`sizeof()/sizeof()`（对编译器来说等价）
<br>

思考：

需要将除了T_SWITCH_TOK（而不是T_SYSCALL）以外的例程的DPL设置为内核态。首先T_SWITCH_TOK是转到内核态，所以只有用户态调用才有意义，DPL为用户态

又T_SYSCALL即int 0x80调用时隐含了T_SWITCH_TOK，所以此时已经进入了内核态，所以没必要也不应将T_SYSCALL的DPL设为用户态
<br>

另外lidt类似于lgdt，用于初始化特殊的基址寄存器

###完善处理函数trap

此处的程序太过简单，与answer比实现几乎没有区别

---
###Challenge

参考answer
<br>

思考：

在进行状态转换时，顺序为状态判断、中断帧处理（包括ds，esp及eflags等等）、压栈、改变栈顶