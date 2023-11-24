# libc

本仓库为笔者和 C 相关的一些学习记录, 见 [libc document](https://luzhixing12345.github.io/libc/), 
代码部分对应文档按文件夹内容大致分为三部分

```bash
├── examples    # linux C 使用示例程序
└── src         # 第三方库
```

- examples: linux 标准库函数和 C 标准库函数的使用示例, 建议配合文档阅读
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

- [星星说编程](https://space.bilibili.com/50657960/)
- man