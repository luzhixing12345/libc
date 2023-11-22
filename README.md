# libc

本仓库为笔者和 C 相关的一些学习记录, 见 [libc document](https://luzhixing12345.github.io/libc/), 
代码部分对应文档按文件夹内容大致分为三部分

```bash
.
├── examples    # linux C 使用示例程序
├── libc        # C 运行时库
└── src         # 第三方库
```

- examples: linux 标准库函数和 C 标准库函数的使用示例, 建议配合文档阅读使用
- libc: 一个跨平台 64 位的 C 运行时库的实现, 参考 <程序员的自我修养> miniCRT
- src: 笔者编写的一个基础 C 工具库 XBOX (第三方库), 用于完成日常 C 编程的一些造轮子的辅助任务

## 编译 & 运行

```bash
## 编译全部
make

## 编译 examples 下的所有示例程序
make examples

## 编译 libc
make libc

## 编译第三方库
make xbox
```

具体的安装/使用方式, 请参考文档对应内容

## 文档

[libc document](https://luzhixing12345.github.io/libc/)

## 参考

- [程序员的自我修养:链接/装载与库.pdf](https://awesome-programming-books.github.io/others/%E7%A8%8B%E5%BA%8F%E5%91%98%E7%9A%84%E8%87%AA%E6%88%91%E4%BF%AE%E5%85%BB%EF%BC%9A%E9%93%BE%E6%8E%A5%E3%80%81%E8%A3%85%E8%BD%BD%E4%B8%8E%E5%BA%93.pdf)
- [星星说编程](https://space.bilibili.com/50657960/)
- man