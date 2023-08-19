
# 文件IO

## 系统调用

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
```

flag 的参数必须要包含 `O_RDONLY` `O_WRONLY` `O_RDWR` 三者中的一个,且只有一个(read-only, write-only, or read/write)

除此之外还有一些 flag 参数, 比如 `O_TRUNC`(截断) `O_CREAT`(创建) `O_NONBLOCK` 非阻塞等等, 如果文件需要被创建, 则还需要传入 mode 参数

```c
#include <unistd.h>

int close(int fd);
```

参数fd是要关闭的文件描述符.当一个进程终止时,内核对该进程所有尚未关闭的文件描述符调用close关闭,所以即使用户程序不调用close,在终止时内核也会自动关闭它打开的所有文件, 但是程序中应该手动关闭. 由open返回的文件描述符一定是该进程尚未使用的**最小**描述符.

```c
#include <sys/types.h>
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);
```

偏移 fd, whence 三个值  `SEEK_SET` `SEEK_CUR` `SEEK_END`, offset 正负代表后移和前移, 返回到文件头的偏移量


```c
#include <sys/mman.h>
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);
```

addr 传 NULL, length 为映射大小, fd 文件描述符, offset 偏移量传0

prot 不能与 fd 冲突

- `PROT_EXEC`  Pages may be executed.
- `PROT_READ`  Pages may be read.
- `PROT_WRITE` Pages may be written.
- `PROT_NONE`  Pages may not be accessed.

flag:

- MAP_SHARED: 多个进程对相同文件映射共享
- MAP_PRIVATE: 多个进程对相同文件映射不共享
- MAP_SHARED_VALIDATE

> MAP_SHARED_VALIDATE的行为与MAP_SHARED提供的相同,但MAP_SHARED映射会忽略标志中的未知标志.相比之下,使用MAP_SHARED_VALIDATE创建映射时,内核会验证所有传递的标志是否已知,并对未知标志使用错误EOPNOTSUPP失败映射


## C 标准库

```c
#include <stdio.h>

FILE *fopen(const char *pathname, const char *mode);
int fgetc(FILE *stream);
int fputc(int c, FILE *stream);
int fputs(const char *s, FILE *stream);
int fclose(FILE *stream);

int fseek(FILE *stream, long offset, int whence); // 类似 lseek
long ftell(FILE *stream); // 返回指针位置
void rewind(FILE *stream); // 回到开头
```

fopen(3) 会调用 oepn(2) 打开指定文件(获取文件描述符), fopen会分配一个 FILE 结构体, 其中包含该文件的描述符, IO缓冲区和当前读写位置等信息, 并返回FILE结构体的地址

fgetc(3) 通过传入的 FILE* 找到该文件描述符, IO缓冲区和当前读写位置, 判断能否从IO缓冲区读到下一个字符, 如果能读到则直接返回, 否则调用 read(2), 让内核读取该文件的数据到IO缓冲区, 然后返回下一个字符

fputs(3) 判断IO缓冲区是否有空间再存放一个字符, 如果有空间则直接保存在IO缓冲区并返回, 如果IO缓冲区已满则调用write(2)让内核把IO缓冲区的内容写回文件

fclose(3) 如果IO缓冲区还有数据没写回文件,则调用 write(2)写回文件,然后调用close(2)关闭文件,释放 FILE 结构体和 IO缓冲区

三种缓冲模式

- 全缓冲: 缓冲区满才将调用write写入buffer, 普通文件
- 行缓冲: 缓冲区满/输入了换行符,调用write写入buffer,stdout
- 无缓冲: 无缓冲区,stderr

下面的代码不会有输出,因为stdout使用行缓冲, 没有检测到换行符不会输出到终端

```c
#include <stdio.h>

int main() {
    
    fputc('A',stdout); // 无输出
    // fputc('\n',stdout);
    while(1) {
        
    }
    return 0;
}
```

但是如果换成 stderr 会有输出, 因为无缓冲

```c
#include <stdio.h>

int main() {
    
    fputc('A',stderr);
    while(1) {
        
    }
    return 0;
}
```

缓冲区长度为 `1024*sizeof(int)`, 也就是说如下的代码刚好不会写入. 再多写一个就会溢出.

```c
#include <stdio.h>

int main() {
    
    int max_buffer_length = 1024;
    for (int i=0;i<max_buffer_length;i++) {
        fputc('A', stdout);
    }
    // fputc('!', stdout);
    while(1) {
        
    }
    return 0;
}
```




## 其他



### fputc putc

fputc 和 putc 基本相同, 都是将一个字符输出到 stdout, 区别在于 putc 在 glibc 中是使用宏来实现的

> https://github.com/bminor/glibc/blob/master/stdio-common/printf_size.c#L28

```c
// stdio-common/printf_size.c
#define putc(c, f) (wide \
		    ? (int)_IO_putwc_unlocked (c, f) : _IO_putc_unlocked (c, f))

// libio/libio.h
#define _IO_putc_unlocked(_ch, _fp) __putc_unlocked_body (_ch, _fp)

# define _IO_getwc_unlocked(_fp)					\
  (__glibc_unlikely ((_fp)->_wide_data == NULL				\
		     || ((_fp)->_wide_data->_IO_read_ptr		\
			 >= (_fp)->_wide_data->_IO_read_end))		\
   ? __wuflow (_fp) : (wint_t) *(_fp)->_wide_data->_IO_read_ptr++)
# define _IO_putwc_unlocked(_wch, _fp)					\
  (__glibc_unlikely ((_fp)->_wide_data == NULL				\
		     || ((_fp)->_wide_data->_IO_write_ptr		\
			 >= (_fp)->_wide_data->_IO_write_end))		\
   ? __woverflow (_fp, _wch)						\
   : (wint_t) (*(_fp)->_wide_data->_IO_write_ptr++ = (_wch)))
```

> https://github.com/bminor/glibc/blob/master/libio/fputc.c#L31

```c
int
fputc (int c, FILE *fp)
{
  int result;
  CHECK_FILE (fp, EOF);
  if (!_IO_need_lock (fp))
    return _IO_putc_unlocked (c, fp);
  _IO_acquire_lock (fp);
  result = _IO_putc_unlocked (c, fp);
  _IO_release_lock (fp);
  return result;
}
```