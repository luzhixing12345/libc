
# pipe

管道是一种进程间通信(IPC, InterProcess Communication)机制,**它只能在具有亲缘关系的进程之间使用**.亲缘关系是指一个进程通过fork()系统调用创建另一个进程,并且这两个进程共享相同的进程空间.在这种情况下,子进程可以使用管道来与父进程通信,因为管道是由父进程创建并在fork()之后被子进程继承.

但是,**如果你想要在两个独立的进程之间通信,这些进程没有亲缘关系,那么你不能使用管道**.这是因为在这种情况下,没有共享文件描述符表或内存空间的机制来共享管道.

**为了在两个独立的进程之间通信,你可以使用其他IPC机制,例如消息队列、共享内存、信号量或套接字**.这些机制不仅可以在亲缘关系进程之间使
用,还可以在任意两个独立进程之间使用,因为它们使用内核提供的IPC机制来实现进程之间的通信.

使用管道需要注意以下4种特殊情况(假设都是阻塞I/0操作,没有设置O_NONBLOCK标志):

1. 如果所有指向管道写端的文件描述符都关闭了,而仍然有进程从管道的读端读数据,那么管道中剩余的数据都被读取后,**再次read会返回0**,就像读到文件末尾一样.
2. 如果有指向管道写端的文件描述符没关闭,而持有管道写端的进程也没有向管道中写数据,这时有进程从管道读端读数据,那么管道中剩余的数据都被读取后,**再次read会阻塞**,直到管道中有数据可读了才读取数据并返回.
3. 如果所有指向管道读端的文件描述符都关闭了,这时有进程向管道的写端write,**那么该进程会收到信号SIGPIPE**,通常会导致进程异常终止
4. 如果有指向管道读端的文件描述符没关闭,而持有管道读端的进程也没有读数据,这时有进程向管道写端写数据,**那么在管道被写满时再次write会阻塞**,直到管道中有空位置了才写入数据并返回.

运行如下的程序, 可以发现父进程因为 wait 等待子进程结束而阻塞, 子进程因为第二次 read 无法读取到数据而阻塞(如情况2)

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int n;
    int fd[2];
    char buf[30];
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0) {
        close(fd[1]);
        n = read(fd[0], buf, 20);
        printf("n = %d\n",n);
        write(1, buf, n);
        n = read(fd[0], buf, 20);
        printf("n = %d\n",n);
        write(1, buf, n);
    } else {
        close(fd[0]);
        write(fd[1], "hello pipe\n", 12);
        wait(NULL);
    }

    return 0;
}
```

此时如果使用 ctrl + c 终止会杀死所有前台进程(父子进程都是前台进程), 可以使用 ps -a 查看父子进程的pid,然后使用 `kill -s SIGINT <pid>` 杀死父进程可以看到输出 `n=0`

下面的程序对应情况4, 此时父进程持续的向管道内写数据, 一共可以写 64kb 的数据后管道满,阻塞. 子进程sleep(10) 后退出,关闭所有打开的文件描述符, 此时进入情况3, 父进程的管道中发现读端被关闭了,所以异常退出

> 使用 echo $? 查看返回值为 141, 对应SIGPIPE信号

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int n;
    int fd[2];
    char buf[1024];
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0) {
        close(fd[1]);
        sleep(10);
    } else {
        close(fd[0]);
        for (int i=0;i<100;i++) {
            write(fd[1], buf, 1024);
            printf("i = %d\n",i);
        }
        wait(NULL);
    }

    return 0;
}
```