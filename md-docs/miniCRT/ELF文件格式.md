
# ELF文件格式

> 代码见 note/SimpleSection.c

```c
int printf(const char *format, ...);

int global_init_var = 84;
int global_uninit_var;

void func1(int i) {
    printf("%d\n",i);
}

int main(void) {
    static int static_var = 85;
    static int static_var2;
    int a = 1;
    int b;
    func1(static_var + static_var2 + a + b);
}
```

使用 gcc 编译器得到 .o 文件

```bash
gcc -c SimpleSection.c
```

## ELF中各个段概览


```bash
$ objdump -h SimpleSection.o

SimpleSection.o:     file format elf64-x86-64

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .text         00000064  0000000000000000  0000000000000000  00000040  2**0
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  1 .data         00000008  0000000000000000  0000000000000000  000000a4  2**2
                  CONTENTS, ALLOC, LOAD, DATA
  2 .bss          00000008  0000000000000000  0000000000000000  000000ac  2**2
                  ALLOC
  3 .rodata       00000004  0000000000000000  0000000000000000  000000ac  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  4 .comment      0000002e  0000000000000000  0000000000000000  000000b0  2**0
                  CONTENTS, READONLY
  5 .note.GNU-stack 00000000  0000000000000000  0000000000000000  000000de  2**0
                  CONTENTS, READONLY
  6 .note.gnu.property 00000020  0000000000000000  0000000000000000  000000e0  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  7 .eh_frame     00000058  0000000000000000  0000000000000000  00000100  2**3
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
```

其中

- `.text`: 程序源码编译后的机器指令
- `.data`: 数据段

  初始化不为 0 的全局变量 + 初始化不为 0 的局部静态变量

- `.bss`: 另一个数据段

  没有初始化的全局变量 + 初始化为 0 的全局变量 + 没有初始化的局部静态变量 + 初始化为 0 的局部静态变量

  > 没有初始化的默认值为 0, 和初始化为 0 效果相同

  单独分出来 .bss 段是因为 .data 段中需要为一个已经初始化的变量分配空间用于存放初始化的值, 如果初始值是 0 那么其实就没有必要分配. 这些变量在程序运行时确实需要占据内存空间, 但是没有必要在在可执行程序中为它们分配空间. **.bss 段只是为初始 值0的变量预留一个位置**

- `.rodata`: 只读数据段
- `.comment`: 注释信息段

  .comment 段是一种特殊的段,用于保存目标文件的注释信息.具体来说,它包含了编译器和汇编器使用的注释信息,例如编译器版本、编译选项、时间戳和作者等.

  .comment 段的主要作用是为了方便开发者在需要时查看目标文件的注释信息,以便了解目标文件的编译环境和作者等相关信息.该段的大小通常很小,对程序的执行没有影响,因为它只是用于存储元数据.

  在实际开发中,.comment 段的信息可以用于调试、版本控制和审计等方面.例如,如果您在调试时遇到了问题,您可以查看目标文件的注释信息,以确定该文件是由哪个编译器和版本生成的.同样,如果您需要对软件进行审计,您可以查看 .comment 段的信息,以确保该软件是由可信的开发者编写的,并且没有被修改或篡改过.在某些情况下如果希望禁用 .comment 段,以减小目标文件的大小或隐藏一些编译器和版本信息.您可以使用编译器选项 `-fno-ident`

  ![20230503195503](https://raw.githubusercontent.com/learner-lu/picbed/master/20230503195503.png)

  可以使用如下指令查看 .comment 段的内容

  ```bash
  $ objdump -s --section=.comment SimpleSection.o

  SimpleSection.o:     file format elf64-x86-64

  Contents of section .comment:
   0000 00474343 3a202855 62756e74 75203131  .GCC: (Ubuntu 11
   0010 2e332e30 2d317562 756e7475 317e3232  .3.0-1ubuntu1~22
   0020 2e30342e 31292031 312e332e 3000      .04.1) 11.3.0.
  ```

- `.note.GNU-stack`: 堆栈提示段

**区分不同段的作用主要是为了将指令和数据的存放区分开, 这样的好处主要有如下三点**:

1. 易于设置读写权限: 指令区域只读, 代码区域可读写,防止程序指令被有意/无意改写
2. 现代CPU的缓存属性: 有益于程序的局部性
3. 多副本可共享

使用 size 查看一个 ELF 文件的代码段/数据段/BSS段的长度

```bash
$ size SimpleSection.o
   text    data     bss     dec     hex filename
    224       8       8     240      f0 SimpleSection.o
```

> 这里的大小不是.o文件的大小, 只是段的大小, 可以使用 `du -b SimpleSection.o` 查看总大小(1936b)

其中 224 是 `.text` + `.rodata` + `.note.GNU-stack` + `.note.gnu.property` + `.eh_frame` = 0xe0 = 224

### 代码段

```bash
objdump -s -d SimpleSection.o
```

可以看到 .text 段的内容对应

![20230503221149](https://raw.githubusercontent.com/learner-lu/picbed/master/20230503221149.png)

### 数据段和只读数据段

.data 段中保存的是 `int global_init_var = 84`(0x54), `static int static_var = 85`(0x55)

.rodata 段保存的是程序中的字符串常量 "%d\n", 其 ASCII 值分别为 0x25 0x64 0x0a 0x00

> 均采用小端存储

![20230503220741](https://raw.githubusercontent.com/learner-lu/picbed/master/20230503220741.png)

### BSS段

```bash
objdump -x -s -d SimpleSection.o
```

可以在符号表中看到 `static_var2` 和 `global_uninit_var` 都保存在了 .bss 段中, .bss 段的大小是 8 字节

![20230503231927](https://raw.githubusercontent.com/learner-lu/picbed/master/20230503231927.png)

笔者在另一台机器中尝试: gcc version 10.2.1 20210110 (Debian 10.2.1-6), 得到的符号表如下所示

![20230503232709](https://raw.githubusercontent.com/learner-lu/picbed/master/20230503232709.png)

此时如果我们稍微修改一下程序, 添加一个初始化为0的全局变量和一个初始化为0的静态局部变量

```c
int printf(const char *format, ...);

int global_init_var = 84;
int global_zero_var = 0;
int global_uninit_var;

void func1(int i) {
    printf("%d\n",i);
}

int main(void) {
    static int static_var = 85;
    static int static_zero_var = 0;
    static int static_var2;
    int a = 1;
    int b;
    func1(static_var + static_var2 + a + b);
}
```

再次编译查看 SYMBOL TABLE, 不难发现其中 static_zero_var 和 global_zero_var 都被放在 .bss 段中

```bash
SYMBOL TABLE:
0000000000000000 l    df *ABS*  0000000000000000 SimpleSection.c
0000000000000000 l    d  .text  0000000000000000 .text
0000000000000000 l    d  .data  0000000000000000 .data
0000000000000000 l    d  .bss   0000000000000000 .bss
0000000000000000 l    d  .rodata        0000000000000000 .rodata
0000000000000004 l     O .data  0000000000000004 static_var.2
0000000000000008 l     O .bss   0000000000000004 static_var2.1
000000000000000c l     O .bss   0000000000000004 static_zero_var.0
0000000000000000 g     O .data  0000000000000004 global_init_var
0000000000000000 g     O .bss   0000000000000004 global_zero_var
0000000000000004 g     O .bss   0000000000000004 global_uninit_var
0000000000000000 g     F .text  000000000000002b func1
0000000000000000         *UND*  0000000000000000 printf
000000000000002b g     F .text  0000000000000039 main
```

还可以使用 `objcopy` 将一张图片添加到 .o 文件中

```bash
$ objcopy -I binary -O elf64-x86-64 a.jpg SimpleSection.o
$ objdump -ht SimpleSection.o

SimpleSection.o:     file format elf64-x86-64

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .data         00009261  0000000000000000  0000000000000000  00000040  2**0
                  CONTENTS, ALLOC, LOAD, DATA
SYMBOL TABLE:
0000000000000000 g       .data  0000000000000000 _binary_a_jpg_start
0000000000009261 g       .data  0000000000000000 _binary_a_jpg_end
0000000000009261 g       *ABS*  0000000000000000 _binary_a_jpg_size
```

其中 `_binary_a_jpg_start` `_binary_a_jpg_end` `_binary_a_jpg_size` 分别表示起始地址, 结束地址, 大小, **我们可以在程序中直接声明并使用它们**, 例如打印出图像数据的前10个字节:

```c
#include <stdio.h>

extern char _binary_a_jpg_start;
extern char _binary_a_jpg_end;
extern int _binary_a_jpg_size;

int main() {
    for (int i = 0; i < 10; i++) {
        printf("%02x ", _binary_a_jpg_start[i]);
    }
    printf("\n");
    return 0;
}
```

> 这种将图片直接打入目标文件是一种方式,不过会导致文件变大

### 自定义段

```c
int printf(const char *format, ...);

int global_init_var = 84;
int global_uninit_var;

__attribute__((section("FOO"))) int global_foo_var = 42;

__attribute__((section("BAR"))) void foo() {
    
}

void func1(int i) {
    printf("%d\n",i);
}

int main(void) {
    static int static_var = 85;
    static int static_var2;
    int a = 1;
    int b;
    func1(static_var + static_var2 + a + b);
}
```

可以看到新增了两个段

![20230504002317](https://raw.githubusercontent.com/learner-lu/picbed/master/20230504002317.png)

`__attribute__((section("name")))` 是GCC和Clang编译器的扩展,它不是C标准的一部分,因此不是所有的编译器都支持它.在MSVC编译器中,可以使用#pragma section指令来将变量或函数放置在自定义段中(#pragma section指令是MSVC编译器的扩展).例如:

```c
#pragma section("FOO", read, write)
int global_foo_var = 42;

#pragma section("BAR", execute)
void foo() {
    // ...
}
```

## 其他说明

当然 GCC 编译器提供了关于 ELF 的众多选项

`-fdata-sections` 和 `-ffunction-sections` 是GCC编译器的选项,用于将数据和函数放置在单独的段中.

当使用这些选项时,**GCC会将每个全局变量和函数放置在单独的段中,而不是将它们放置在默认的.data和.text段中**.这样做的好处是可以将不同的数据和函数放置在不同的段中,从而使得目标文件更加灵活.例如,您可以将只读数据放置在只读段中,将可写数据放置在可写段中,将可执行代码放置在可执行段中.

```bash
gcc -c -fdata-sections -ffunction-sections file.c
```

在将目标文件链接到可执行文件时,您需要使用 `-Wl` , `--gc-sections` 选项来删除未使用的段.这将从目标文件中删除未使用的段,从而减少可执行文件的大小.例如:

```bash
gcc -Wl,--gc-sections file.o -o program
```

请注意,使用 `-fdata-sections` 和 `-ffunction-sections` 选项可能会增加目标文件的大小,并在链接时增加一些开销.然而,通过将数据和函数放置在单独的段中,可以使得目标文件更加灵活,并且可以优化可执行文件的大小和性能.

比如我们稍微修改一下代码

```c
int printf(const char *format, ...);

int global_init_var = 84;
int global_init_var2 = 88;
int global_uninit_var;

void func1(int i) {
    printf("%d\n",i);
}

int main(void) {
    static int static_var = 85;
    static int static_var2 = 89;
    static int static_var3;
    int a = 1;
    int b;
    func1(static_var + static_var2 + a + b);
}
```

不难发现 ELF 中 .text .data 已经分开了

![20230504003246](https://raw.githubusercontent.com/learner-lu/picbed/master/20230504003246.png)