
# man

```bash
1   可执行程序或shell命令
2   系统调用（由内核提供的函数）
3   库调用（程序库中的函数）
4   特殊文件（通常在/dev中找到）
5   文件格式和约定，例如/etc/passwd
6   游戏
7   各种各样的主题（包括宏包和约定），例如man(7)，groff(7)，man-pages(7)
8   系统管理命令（通常只针对root）
9   内核例程 [非标准]
```

![20230420194604](https://raw.githubusercontent.com/learner-lu/picbed/master/20230420194604.png)

## [glibc](https://www.gnu.org/software/libc/)

```bash
git clone https://sourceware.org/git/glibc.git
cd glibc
git checkout master
```

```bash
git clone https://sourceware.org/git/glibc.git
cd glibc
git checkout release/2.37/master
```

## 通用概念

### perror exit

错误信息处理通常使用 perror

```c
#include <stdio.h>
#include <errno.h>

void perror(const char *s);
const char * const sys_errlist[];
int sys_nerr;
int errno;
```

通常不直接使用 errno, 而是使用 perror 输出一些提示信息, 让内核根据 errno 补全错误信息, 并退出.

exit 的返回值是 0-255 无符号数, 可以通过下面的命令查看程序运行的返回值

```bash
echo $?
```

也可以使用 atexit 绑定一个函数指针在退出时执行

### umask

对应linux中的用户,组,其他用户的概念, 文件的权限包括对于 用户|组|其他用户的读(R)写(W)和执行(X), 所以 `rwxrwxrwx`

默认umask是0022(八进制), 代表你可以设置的文件范围是 `rwxr-xr-x`, 新创建的文件将会默认具有所有者的读、写权限，但是不具有组和其他用户的写权限。这意味着，只有文件所有者才能修改文件内容，而其他用户只能读取文件内容，无法进行修改。这样可以保护文件的安全性，避免其他用户意外地修改或删除文件。

### 轮询(poll) 和 select