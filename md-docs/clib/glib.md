
# glib

Glib 是一个广泛使用的 C 语言库,提供了许多数据结构、线程、消息传递、对象系统和其他功能.许多大型和知名的开源项目都使用了 Glib, 如 QEMU/GNOME/GTK/Systemd.

glib 的代码封装的很完善, 这里我们先介绍几种十分常见且有用的函数接口

```bash
sudo apt install libglib2.0-dev
```

编译的时候

```Makefile
CFLAGS += `pkg-config --cflags glib-2.0`
LDFLAGS = -pthread `pkg-config --libs glib-2.0`
```

## hashtable

C 并没有原生的 hashtable 的实现, GLib 提供了 `GHashTable`,它是一个高效的哈希表实现.你可以使用它来存储键值对,并进行插入、删除、查找和遍历操作. 如下是一个简单的使用案例

```c
// src/glib/hashtable.c
#include <glib.h>
#include <stdio.h>

struct MyData {
    int x;
    int y;
};

// 回调函数,用于处理每个键值对
void print_key_value(gpointer key, gpointer value, gpointer user_data) {
    printf("Key: %s, Value: %s\n", (char *)key, (char *)value);
    struct MyData *data = (struct MyData *)user_data;
    printf("x = %d, y = %d\n", data->x, data->y);
}

int main() {
    // 创建一个哈希表
    GHashTable *hash_table = g_hash_table_new(g_str_hash, g_str_equal);

    // 添加键值对到哈希表
    g_hash_table_insert(hash_table, "name", "Alice");
    g_hash_table_insert(hash_table, "city", "Wonderland");
    g_hash_table_insert(hash_table, "occupation", "Adventurer");

    // 查找键值对
    char *name = g_hash_table_lookup(hash_table, "name");
    if (name) {
        printf("Found: Key: name, Value: %s\n", name);
    } else {
        printf("Key 'name' not found\n");
    }

    // 删除键值对
    g_hash_table_remove(hash_table, "city");

    // 检查键值对是否存在
    if (g_hash_table_contains(hash_table, "city")) {
        printf("Key 'city' found\n");
    } else {
        printf("Key 'city' not found\n");
    }

    // 使用 g_hash_table_foreach 遍历哈希表
    struct MyData data = {10, 20};
    g_hash_table_foreach(hash_table, print_key_value, &data);

    // 销毁哈希表
    g_hash_table_destroy(hash_table);

    return 0;
}
```

1. **创建哈希表**:使用 `g_hash_table_new` 创建一个哈希表,其中键和值都是字符串.
2. **添加键值对**:使用 `g_hash_table_insert` 向哈希表中添加键值对.
3. **查找键值**: 使用 `g_hash_table_lookup` 查找元素值, 使用 `g_hash_table_contains` 判断元素是否存在
4. **遍历哈希表**:使用 `g_hash_table_foreach` 遍历哈希表,并对每个键值对调用回调函数 `print_key_value`.
5. **回调函数**:`print_key_value` 打印每个键值对.
6. **销毁哈希表**:使用 `g_hash_table_destroy` 销毁哈希表,释放内存.

`g_hash_table_foreach` 是 GLib 提供的一个函数,用于遍历哈希表(`GHashTable`)中的所有键值对,并对每个键值对执行指定的回调函数.它的函数签名如下:

```c
void g_hash_table_foreach(GHashTable *hash_table, GHFunc func, gpointer user_data);
```

- `hash_table`:要遍历的哈希表.
- `func`:用于处理每个键值对的回调函数,类型为 `GHFunc`.
- `user_data`:传递给回调函数的用户数据.

`GHFunc` 是一个函数指针类型,其定义如下:

```c
typedef void (*GHFunc) (gpointer key, gpointer value, gpointer user_data);
```

- `key`:哈希表中的一个键.
- `value`:与该键关联的值.
- `user_data`:传递给 `g_hash_table_foreach` 的用户数据.

**如果需要传入多个参数传给 user_data 可以创建一个结构体传入**

## 事件循环

QEMU 作为一个持续运行的状态机, 需要不停地检查是否有新事件发生, 如果发生那么执行对应的一些处理. 我们可能会写出一个常见的 while 循环

```c
while (1) {
    if (something_happened) {
        do_something();
    }
    // ...
}
```

但是实际情况可能比较复杂, 比如希望间隔 0.1 秒执行一次, 事件触发对应的回调函数, 可以在循环过程中注册新事件, 删除某些事件等等. glib 提供了一种简洁的事件循环接口

Glib 提供了一个功能强大的事件循环机制,可以用于构建基于事件驱动的应用程序.事件循环通过 `GMainLoop` 和相关函数来实现

### 基础用法

下面是一个简单的示例, 首先创建了一个主循环 loop. 然后使用 `g_timeout_add` 注册了两个回调事件. 第一个参数为间隔时间, 第二个参数为回调函数指针, 第三个参数是传参

然后通过 `g_main_loop_run` 进入主循环, 这是一个 while 死循环, 此时程序会执行所有的注册事件. 执行效果如下图所示

```c{10,11,13}
#include <glib.h>
#include <stdio.h>

gboolean count_down(gpointer data);
gboolean cancel_fire(gpointer data);

int main(int argc, char *argv[]) {
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);

    g_timeout_add(500, count_down, NULL);
    g_timeout_add(8000, cancel_fire, loop);

    g_main_loop_run(loop);
    g_main_loop_unref(loop);

    return 0;
}

gboolean count_down(gpointer data) {
    static int counter = 10;

    if (counter < 1) {
        printf("-----[FIRE]-----\n");
        return FALSE;
    }

    printf("-----[% 4d]-----\n", counter--);
    return TRUE;
}

gboolean cancel_fire(gpointer data) {
    GMainLoop *loop = data;

    printf("-----[QUIT]-----\n");
    g_main_loop_quit(loop);

    return FALSE;
}
```

> 代码见 src/glib/event_loop.c

![sdmkl9l](https://raw.githubusercontent.com/learner-lu/picbed/master/sdmkl9l.gif)

`GMainLoop` 表示一个主事件循环.

```c
GMainLoop* g_main_loop_new(GMainContext *context, gboolean is_running);
```
- `context`: 事件循环的上下文.传 `NULL` 使用默认上下文.
- `is_running`: 初始状态是否运行.通常传 `FALSE`.

`g_timeout_add` 用于在指定时间间隔后调用回调函数.

```c
guint g_timeout_add(guint interval, GSourceFunc function, gpointer data);
```
- `interval`: 超时时间,单位为毫秒.
- `function`: 超时触发时调用的回调函数.
- `data`: 传递给回调函数的用户数据.

回调函数用于处理事件.示例中有两个回调函数 `count_down` 和 `cancel_fire`, 返回值代表是否继续运行

1. **创建主事件循环**:`g_main_loop_new` 创建一个新的 `GMainLoop` 实例.
2. **添加超时事件**:`g_timeout_add` 添加两个超时事件,一个每 500 毫秒调用 `count_down`,另一个 8000 毫秒后调用 `cancel_fire`.
3. **运行主事件循环**:`g_main_loop_run` 启动事件循环,直到调用 `g_main_loop_quit` 退出循环.
4. **退出和清理**:在 `cancel_fire` 中调用 `g_main_loop_quit` 退出事件循环,最后调用 `g_main_loop_unref` 释放 `GMainLoop` 实例.

### 自定义事件源

上例中我们创建了两个定时执行的函数, 但同时我们也希望可以注册一些自定义事件, 即当某些事情发生时执行某些操作, 并且可以灵活的添加删除终止

我们先来思考一下, 如何检查事件源是否有事件发生? 我们可以把事件源分为两类:

1. 一些定时事件, 一些
2. 需要 poll(文件事件源),返回值 FALSE,因为只有轮询文件之后,才能知道文件事件是否发生.

![20240807095839](https://raw.githubusercontent.com/learner-lu/picbed/master/20240807095839.png)


```c
#include <fcntl.h>
#include <glib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

typedef struct {
    GSource source;
    GMainLoop *loop;
    char buf;
} KeyboardSource;

// 函数用于设置终端为非阻塞模式
void set_terminal_mode() {
    struct termios new_termios;
    tcgetattr(0, &new_termios);
    new_termios.c_lflag &= ~ICANON;  // 关闭规范模式
    new_termios.c_lflag &= ~ECHO;    // 关闭回显
    tcsetattr(0, TCSANOW, &new_termios);
}

// 函数用于恢复终端为阻塞模式
void reset_terminal_mode() {
    struct termios new_termios;
    tcgetattr(0, &new_termios);
    new_termios.c_lflag |= ICANON;  // 开启规范模式
    new_termios.c_lflag |= ECHO;    // 开启回显
    tcsetattr(0, TCSANOW, &new_termios);
}

// 设置文件描述符为非阻塞模式
void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// 自定义事件源的准备函数
static gboolean keyboard_source_prepare(GSource *source, gint *timeout) {
    *timeout = 0;  // 没有延迟
    return FALSE;  // 返回 FALSE 说明需要 poll(fd)
}

// 自定义事件源的检查函数
static gboolean keyboard_source_check(GSource *source) {
    KeyboardSource *keyboard_source = (KeyboardSource *)source;
    if (read(STDIN_FILENO, &keyboard_source->buf, 1) > 0) {
        return TRUE;
    }
    return FALSE;
}

// 自定义事件源的分派函数
static gboolean keyboard_source_dispatch(GSource *source, GSourceFunc callback, gpointer user_data) {
    KeyboardSource *keyboard_source = (KeyboardSource *)source;

    char buf = keyboard_source->buf;
    if (buf != 'q') {
        printf("Key '%c' pressed\n", buf);
    } else {
        printf("Key 'q' pressed, exiting...\n");
        g_main_loop_quit(keyboard_source->loop);
        return FALSE;
    }
    return TRUE;
}

// 自定义事件源的销毁函数
static void keyboard_source_finalize(GSource *source) {
    printf("Keyboard source finalized\n");
}

static GSourceFuncs keyboard_source_funcs = {
    keyboard_source_prepare, keyboard_source_check, keyboard_source_dispatch, keyboard_source_finalize};

int main(int argc, char *argv[]) {
    set_terminal_mode();
    set_nonblocking(STDIN_FILENO);

    GMainContext *context = g_main_context_new();       // 创建新的上下文
    GMainLoop *loop = g_main_loop_new(context, FALSE);  // 使用新上下文创建主循环

    // 创建自定义事件源
    GSource *source = g_source_new(&keyboard_source_funcs, sizeof(KeyboardSource));
    KeyboardSource *keyboard_source = (KeyboardSource *)source;
    keyboard_source->loop = loop;

    g_source_attach(source, context);  // 将事件源添加到指定上下文中

    g_main_loop_run(loop);  // 运行主循环

    g_main_loop_unref(loop);        // 释放主循环
    g_source_unref(source);         // 释放事件源
    g_main_context_unref(context);  // 释放上下文

    reset_terminal_mode();

    return 0;
}

```

## 参考

- [GLib介绍与使用](https://blog.csdn.net/u014338577/article/details/52932358)
- [Glib 主事件循环轻度分析与编程应用](https://blog.csdn.net/song_lee/article/details/116809089)