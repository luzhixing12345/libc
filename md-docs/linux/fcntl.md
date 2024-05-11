
# fcntl

### 非阻塞读

如果在open一个设备时指定了O_NONBLOCK标志,read/write就不会阻塞.以read为例,如果设备暂时没有数据可读就返回-1,同时置errno为`EWOULDBLOCK`(或者`EAGAIN`,这两个宏定义的值相同),表示本来应该阻塞在这里(would block,虚拟语),事实上并没有阻塞而是直接返回错误,调
用者应该试着再读一次(again).这种行为方式称为轮询(Poll),调用者只是查询一下,而不是阻塞在这里死等,这样可以同时监视多个设备:

正常来说 stdin 是阻塞的读状态

```c
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    char buf[20];
    ssize_t n;

    n = read(STDIN_FILENO, buf, 10);
    printf("read %ld bytes\n", n);

    return 0;
}
```

```bash
(base) kamilu@LZX:~/xbox/notes$ ./test
hello world
read 10 bytes
(base) kamilu@LZX:~/xbox/notes$ d
d: command not found
```

运行发现只会读取 10 个字节, 多余的字符输出到 shell 中, 且 read 需要等待用户输入

一个非阻塞读案例的如下

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

int main() {

    int fd = open("/dev/tty", O_RDONLY|O_NONBLOCK);
    if (fd < 0) {
        perror("Open fail");
    }
    char buf[20];
    ssize_t n;

    while (1) {
        n = read(fd, buf, 10);
        if (~n) {
            printf("read %ld bytes\n",n);
            break;
        }
        if (errno != EAGAIN) {
            perror("READ /dev/tty");
            exit(1);
        }
        write(STDOUT_FILENO,"try again\n",10);
        sleep(2);
    }
    close(fd);
    
    return 0;
}
```

当然也可以使用 fcntl 来修改 stdin 的 flags 位

```c
#include <unistd.h>
#include <fcntl.h>

int fcntl(int fd, int cmd, ... /* arg */ );
```

这个 fcntl 比较复杂, 功能取决于 cmd 的值, 例如 `F_GETFD` `F_SETFD`

```c
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {

    int origin_flags;
    int flags;
    if ((flags = fcntl(STDIN_FILENO, F_GETFL))< 0) {
        perror("fcntl get flags");
        exit(1);
    }
    origin_flags = flags;
    flags |= O_NONBLOCK;
    if ((flags = fcntl(STDIN_FILENO,F_SETFL, flags))<0) {
        perror("fcntl set flags");
        exit(1);
    }
 
    char buf[20];
    ssize_t n;

    while (1) {
        n = read(STDIN_FILENO, buf, 10);
        if (n >=0) {
            break;
        }
        if (errno != EAGAIN) {
            perror("read");
            exit(1);
        }
        printf("try again\n");
        sleep(2);
    }
    printf("read %ld bytes\n", n);
    fcntl(STDIN_FILENO,F_SETFL, origin_flags);
    return 0;
}
```

```c
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("usage: cmd fd\n");
        exit(1);
    }

    int flags;
    if ((flags = fcntl(atoi(argv[1]), F_GETFL)) < 0) {
        perror("fcntl get flags");
        exit(1);
    }

    switch (flags & O_ACCMODE) {
        case O_RDONLY:
            printf("read only");
            break;
        case O_WRONLY:
            printf("write only");
            break;
        case O_RDWR:
            printf("read write");
            break;
        default:
            printf("invalid access mode\n");
    }

    if (flags & O_APPEND) {
        printf(", append");
    }
    if (flags & O_NONBLOCK) {
        printf(", nonblock");
    }

    printf("\n");
    return 0;
}
```

open twice vs open then dup/fork + write append + kernel debug

18min https://www.bilibili.com/video/BV1j24y1x7UH
