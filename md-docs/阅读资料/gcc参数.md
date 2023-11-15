
# gcc参数

```bash
$ gcc -v

Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/11/lto-wrapper
OFFLOAD_TARGET_NAMES=nvptx-none:amdgcn-amdhsa
OFFLOAD_TARGET_DEFAULT=1
Target: x86_64-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu 11.3.0-1ubuntu1~22.04' --with-bugurl=file:///usr/share/doc/gcc-11/README.Bugs --enable-languages=c,ada,c++,go,brig,d,fortran,objc,obj-c++,m2 --prefix=/usr --with-gcc-major-version-only --program-suffix=-11 --program-prefix=x86_64-linux-gnu- --enable-shared --enable-linker-build-id --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --libdir=/usr/lib --enable-nls --enable-bootstrap --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --with-default-libstdcxx-abi=new --enable-gnu-unique-object --disable-vtable-verify --enable-plugin --enable-default-pie --with-system-zlib --enable-libphobos-checking=release --with-target-system-zlib=auto --enable-objc-gc=auto --enable-multiarch --disable-werror --enable-cet --with-arch-32=i686 --with-abi=m64 --with-multilib-list=m32,m64,mx32 --enable-multilib --with-tune=generic --enable-offload-targets=nvptx-none=/build/gcc-11-xKiWfi/gcc-11-11.3.0/debian/tmp-nvptx/usr,amdgcn-amdhsa=/build/gcc-11-xKiWfi/gcc-11-11.3.0/debian/tmp-gcn/usr --without-cuda-driver --enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu --with-build-config=bootstrap-lto-lean --enable-link-serialization=2
Thread model: posix
Supported LTO compression algorithms: zlib zstd
gcc version 11.3.0 (Ubuntu 11.3.0-1ubuntu1~22.04)
```

- `Using built-in specs.`:表明GCC编译器使用内置的默认选项.
- `COLLECT_GCC=gcc`:表示GCC编译器的实际名称为gcc.
- `COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/11/lto-wrapper`:是GCC的链接器插件,用于支持LTO(Link Time Optimization).
- `OFFLOAD_TARGET_NAMES=nvptx-none:amdgcn-amdhsa`:是用于支持GPU加速的选项.
- `OFFLOAD_TARGET_DEFAULT=1`:表示默认的GPU加速目标为nvptx-none.
- `Target: x86_64-linux-gnu`:表示编译器的目标平台为x86_64架构的Linux操作系统.
- `Thread model: posix`: GCC编译器支持的线程模型,最常见的是"posix"线程模型,它是POSIX标准定义的线程模型,也是Linux系统默认的线程模型.其他线程模型还包括"single"线程模型和"multi"线程模型等
- `Supported LTO compression algorithms: zlib zstd`:表示支持的LTO压缩算法为zlib和zstd.
- `gcc version 11.3.0 (Ubuntu 11.3.0-1ubuntu1~22.04)`:表示GCC编译器的版本号为11.3.0,是Ubuntu 22.04发行版的特定版本.

这里的"configure with"是指在编译GCC的过程中,使用了一个名为"configure"的脚本,并向该脚本传递了一系列选项参数.

在上面的输出中,"Configured with"行列出了所有传递给"configure"脚本的选项参数.这些选项参数告诉"configure"脚本如何配置GCC的构建过程,包括:

- `--with-pkgversion`:指定生成的GCC二进制文件包含的版本信息.
- `--enable-languages`:指定GCC编译器支持的语言,包括C、C++、Ada、Fortran等.
- `--prefix`:指定安装GCC编译器的根目录.
- `--enable-shared`:表示编译生成共享库的支持.
- `--enable-threads`:指定支持的线程模型为POSIX线程模型.
- `--enable-bootstrap`:表示使用已经安装的GCC编译器作为编译器的基础来构建新的GCC编译器,以确保生成的编译器的正确性.
- `--enable-checking`:指定检查级别为"release",即启用所有可用的编译器检查,以检测潜在的编程错误.
- `--with-build-config`:指定编译器的构建配置为"bootstrap-lto-lean",以减少编译器构建时间并提高编译速度.

```bash
# 查看 C 库的版本
ldd --version

ldd (Ubuntu GLIBC 2.35-0ubuntu3.1) 2.35
```

```bash
(base) kamilu@LZX:~/miniCRT$ dpkg -s libc6
Package: libc6
Status: install ok installed
Priority: optional
Section: libs
Installed-Size: 13592
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Architecture: amd64
Multi-Arch: same
Source: glibc
Version: 2.35-0ubuntu3.1
Replaces: libc6-amd64
Depends: libgcc-s1, libcrypt1 (>= 1:4.4.10-10ubuntu4)
Recommends: libidn2-0 (>= 2.0.5~), libnss-nis, libnss-nisplus
Suggests: glibc-doc, debconf | debconf-2.0, locales
Breaks: busybox (<< 1.30.1-6), fakeroot (<< 1.25.3-1.1ubuntu2~), hurd (<< 1:0.9.git20170910-1), ioquake3 (<< 1.36+u20200211.f2c61c1~dfsg-2~), iraf-fitsutil (<< 2018.07.06-4), libgegl-0.4-0 (<< 0.4.18), libtirpc1 (<< 0.2.3), locales (<< 2.35), locales-all (<< 2.35), macs (<< 2.2.7.1-3~), nocache (<< 1.1-1~), nscd (<< 2.35), openarena (<< 0.8.8+dfsg-4~), openssh-server (<< 1:8.2p1-4), r-cran-later (<< 0.7.5+dfsg-2), wcc (<< 0.0.2+dfsg-3)
Conffiles:
 /etc/ld.so.conf.d/x86_64-linux-gnu.conf d4e7a7b88a71b5ffd9e2644e71a0cfab
Description: GNU C Library: Shared libraries
 Contains the standard libraries that are used by nearly all programs on
 the system. This package includes shared versions of the standard C library
 and the standard math library, as well as many others.
Homepage: https://www.gnu.org/software/libc/libc.html
Original-Maintainer: GNU Libc Maintainers <debian-glibc@lists.debian.org>
Original-Vcs-Browser: https://salsa.debian.org/glibc-team/glibc
Original-Vcs-Git: https://salsa.debian.org/glibc-team/glibc.git
```

```c
#include <stdio.h>
#include <gnu/libc-version.h>

int main() {
    printf("GLIBC version: %s\n", gnu_get_libc_version());

    return 0;
}
```

链接器的搜索路径

```bash
ld --verbose | grep SEARCH_DIR
```

```bash
SEARCH_DIR("=/usr/local/lib/x86_64-linux-gnu"); SEARCH_DIR("=/lib/x86_64-linux-gnu"); SEARCH_DIR("=/usr/lib/x86_64-linux-gnu"); SEARCH_DIR("=/usr/lib/x86_64-linux-gnu64"); SEARCH_DIR("=/usr/local/lib64"); SEARCH_DIR("=/lib64"); SEARCH_DIR("=/usr/lib64"); SEARCH_DIR("=/usr/local/lib"); SEARCH_DIR("=/lib"); SEARCH_DIR("=/usr/lib"); SEARCH_DIR("=/usr/x86_64-linux-gnu/lib64"); SEARCH_DIR("=/usr/x86_64-linux-gnu/lib");
```

安装包的位置

```bash
# -I/usr/include/graphviz
# -L/usr/lib/x86_64-linux-gnu
# -lgvc -lcgraph -lcdt
```
