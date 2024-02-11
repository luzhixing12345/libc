
# unicode

```c
#include "unicode.h"
```

## API

```c
/**
 * @brief char * 转为 unicode string, 返回值需要调用 free 释放内存
 *
 * @param text
 * @param length 长度
 * @return uchar* (need free)
 */
uchar *XBOX_char2uchar(const char *text, size_t *length);

/**
 * @brief 将单个 unicode char 输出到 buf
 *
 * @param codepoint
 * @param buf buf[5]
 * @return int
 */
int XBOX_uchar2char(unsigned int codepoint, char *buf);
```

## example

```c
#include "unicode.h"

int main(int argc, char **argv) {
    char *text = "大街上Dahl金钱和维拉省的";
    size_t length;
    uchar *ucharing = XBOX_char2uchar(text, &length);
    char buf[5];
    for (int i = 0; i < length; i++) {
        XBOX_uchar2char(ucharing[i], buf);
        printf("%s\n", buf);
    }
    return 0;
}
```