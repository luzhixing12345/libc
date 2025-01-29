# clib

[![GitHub license](https://img.shields.io/github/license/Naereen/StrapDown.js.svg)](https://github.com/luzhixing12345/clib/blob/main/LICENSE)

高级语言(如C++、Python等)提供了丰富的标准库和数据结构, 但是在 C 中缺乏灵活且易于使用的内建数据结构, 导致开发者需要自行编写很多常用工具库

clib 是一个小型的 C 工具库, 用精简的方式实现了一些 C 标准库没有提供但是日常开发中常见的功能需求, 包括

- 日志(线程安全)
- 命令行参数解析
- hashtable
- 终端彩色打印
- 目录, 文件相关的处理(判断文件存在, 递归删除目录...)

本项目大部分代码是笔者从网络上搜集到的其他大佬开发好的轮子并修改封装为简单易用的 API, 绝赞维护更新中(๑˃̵ᴗ˂̵)و

## 安装(TODO)

```bash
sudo apt install clib
```

## 源码编译

本库无第三方依赖, 且代码量相对较小, 可以轻松的嵌入到其他 C 项目中, 完全可以(推荐)直接将 clib/ 拷贝到其他项目中并整体参与链接

或者可以将其作为静态库编译

```bash
make
```

## 示例

examples/ 下包含了所有的示例代码

```bash
cd example
bear -- make
```

每一个 .c 文件对应一项功能, 可以运行对应的可执行文件来查看

## 文档

[libc document](https://luzhixing12345.github.io/clib/)

## 参考

- [星星说编程](https://space.bilibili.com/50657960/)
- man