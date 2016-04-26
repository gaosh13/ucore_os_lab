# 同步互斥(lec 17) spoc 思考题

## 小组思考题

1. （spoc）阅读[简化x86计算机模拟器的使用说明](https://github.com/chyyuu/ucore_lab/blob/master/related_info/lab7/lab7-spoc-exercise.md)，理解基于简化x86计算机的汇编代码。

2. （spoc)了解race condition. 进入[race-condition代码目录](https://github.com/chyyuu/ucore_lab/tree/master/related_info/lab7/race-condition)。

测试环境：linux ubuntu 15.10（随机种子和系统有关）

 - 执行 `./x86.py -p loop.s -t 1 -i 100 -R dx`， 请问`dx`的值是什么？
 - dx=-1
 - 执行 `./x86.py -p loop.s -t 2 -i 100 -a dx=3,dx=3 -R dx` ， 请问`dx`的值是什么？
 - dx=-1
 - 执行 `./x86.py -p loop.s -t 2 -i 3 -r -a dx=3,dx=3 -R dx`， 请问`dx`的值是什么？
 - dx=-1
 - 变量x的内存地址为2000, `./x86.py -p looping-race-nolock.s -t 1 -M 2000`, 请问变量x的值是什么？
 - 位于地址2000的x=1
 - 变量x的内存地址为2000, `./x86.py -p looping-race-nolock.s -t 2 -a bx=3 -M 2000`, 请问变量x的值是什么？为何每个线程要循环3次？
 - 最终x=6，因为参数设置寄存器bx=3，各线程初始化均设置bx=3
 - 变量x的内存地址为2000, `./x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 0`， 请问变量x的值是什么？
 - 数据间存在互斥，最终x=1
 - 变量x的内存地址为2000, `./x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 1`， 请问变量x的值是什么？
 - 最终x=1
 - 变量x的内存地址为2000, `./x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 2`， 请问变量x的值是什么？ 
 - 最终x=2
 - 变量x的内存地址为2000, `./x86.py -p looping-race-nolock.s -a bx=1 -t 2 -M 2000 -i 1`， 请问变量x的值是什么？ 
 - 最终x=1

3. （spoc） 了解software-based lock, hardware-based lock, [software-hardware-lock代码目录](https://github.com/chyyuu/ucore_lab/tree/master/related_info/lab7/software-hardware-locks)

  - 理解flag.s,peterson.s,test-and-set.s,ticket.s,test-and-test-and-set.s 请通过x86.py分析这些代码是否实现了锁机制？请给出你的实验过程和结论说明。能否设计新的硬件原子操作指令Compare-And-Swap,Fetch-And-Add？
```
Compare-And-Swap

int CompareAndSwap(int *ptr, int expected, int new) {
  int actual = *ptr;
  if (actual == expected)
    *ptr = new;
  return actual;
}
```

```
Fetch-And-Add

int FetchAndAdd(int *ptr) {
  int old = *ptr;
  *ptr = old + 1;
  return old;
}
```

测试结果：

 - flag.s没有锁好，采用./x86.py -p flag.s -a bx=2 -t 2 -i 4 -r -R ax,bx -M count -c可以详细观察运行情况最终count=3，比预期值小，因为两个进程同时进入临界区，程序在flag判断结束后没有及时为flag置1
 - peterson.s锁好了，./x86.py -p peterson.s -t 2 -i 4 -r -a bx=0,bx=1 -R ax,bx -M count -c，运行多次，结果均为2，预期值为2
 - test-and-set.s锁好了，利用了swap原子操作
 - ticket.s锁好了，执行./x86.py -p ticket.s -t 2 -a bx=5 -i 4 -r -M count -c多次，结果均为10
 - test-and-test-and-set.s锁好了

利用CompareAndSwap实现互斥锁：

思路：
```
int a = 1;
while (CompareAndSwap(mutex, 0, 1) == 1);
// critical section
while (CompareAndSwap(mutex, 1, 0) == 0);
// quit
```
利用FetchAndAdd实现互斥锁：

思路：全局变量
```
int ticket = 0，turn = 0;
```
主体部分：
```
do{
    int a = FetchAndAdd(ticket);
    int c = turn;
}while(a!=c);
//critical section
FetchAndAdd(turn);
//quit
```