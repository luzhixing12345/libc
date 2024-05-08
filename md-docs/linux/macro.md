
# macro

## offsetof

计算结构体成员相对于结构体的偏移

```c
#include <stdio.h>

// #include <stddef.h>
#define offsetof(type, member)      (size_t) &((type*)0)->member

typedef struct {
    char a;
    int b;
    int c;
} test;

int main(void) {
    printf("offset: %ld %ld\r\n", offsetof(test, b), offsetof(test, c));
    return 0;
}
```

```bash
offset: 4 8
```

此宏的实现为

```c
#define offsetof(type, member)      (size_t) &((type*)0)->member
```

## container_of

通过结构体成员地址找到结构体对象首地址

```c
#include <stdio.h>

#define offsetof(type, member) (size_t) & ((type *)0)->member

#define container_of(ptr, type, member)                    \
    ({                                                     \
        const typeof(((type *)0)->member) *__mptr = (ptr); \
        (type *)((char *)__mptr - offsetof(type, member)); \
    })

typedef struct {
    char a;
    int b;
    float c;
} test_t;

int main(void) {
    test_t test, *p_test;
    int *container_test = &(test.b);
    p_test = container_of(container_test, test_t, b);
    if (&test == p_test) {
        printf("Addr is samed\r\n");
    }
}
```

```bash
Addr is samed
```

整个宏展开为 `({})`, 这是一个 GNU 扩展的 C 语法,称为 braced-group in expression.编译器将计算整个块,并使用块中包含的最后一个语句的值, 例如如下代码将打印 5

```c
int x = ({1; 2;}) + 3;
printf("%d\n", x); // 5
```

typeof 也是一个 [GNU C 扩展](https://gcc.gnu.org/onlinedocs/gcc/Typeof.html), 它接受一个参数并返回其类型

第一行对于宏的结果本质上并不重要,但它用于类型检查目的.第二行则是从结构体的地址中减去结构成员的偏移量,得到容器结构的地址

> 一个比较详细的解释见: [understand container_of macro](https://linux-concepts.blogspot.com/2018/01/understanding-containerof-macro-in.html)

就宏的作用而言,确实只需要最后的 (type *)((char*)ptr - offsetof(type,member) )就可以得到正确结果.但是以上都是假设输入都是正确的,对于不正确的输入,我们的宏应当有相应的检查与报错机制.这正是 const typeof( ((type *)0)->member ) *__mptr = (ptr); 的意义所在.它可以检查 ptr的类型是否正确.以下面代码为例

```c
#define container_of
struct A { int i; float j; };
struct A k;
float *i = &A.j;
struct A *ptr = container_of(i, struct A, i);
```

```bash
warning: initialization of 'const int *' from incompatible pointer type 'float *'
```

```c
#define container_of(ptr, type, member) ({				\
	void *__mptr = (void *)(ptr);					\
	static_assert(__same_type(*(ptr), ((type *)0)->member) ||	\
		      __same_type(*(ptr), void),			\
		      "pointer type mismatch in container_of()");	\
	((type *)(__mptr - offsetof(type, member))); })
```

其中 static_cast就是对 C11 中 _Static_assert的简单 wrapper,这也是内核标准切换到 C11 后获得的好处之一.甚至还有一个更加 safe 的 container_of_safe宏.当 ptr是空指针或者指针值不是合法值时不转换.

```c
#define container_of_safe(ptr, type, member) ({				\
	void *__mptr = (void *)(ptr);					\
	static_assert(__same_type(*(ptr), ((type *)0)->member) ||	\
		      __same_type(*(ptr), void),			\
		      "pointer type mismatch in container_of_safe()");	\
	IS_ERR_OR_NULL(__mptr) ? ERR_CAST(__mptr) :			\
		((type *)(__mptr - offsetof(type, member))); })
```

## 参考

- [linux/C语言中的container_of的标准写法有什么好处?为什么要间接做一次指针转换?](https://www.zhihu.com/question/564087562)