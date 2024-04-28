

#include "unicode.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static unsigned int get_next_uchar(char **pos);

uchar *XBOX_char2uchar(const char *text, size_t *length) {
    size_t l = strlen(text);
    uchar *str = (uchar *)malloc(sizeof(uchar) * (l + 1));
    char *p = (char *)text;
    int counter = 0;
    while (*p != 0) {
        unsigned int codepoint = get_next_uchar(&p);
        str[counter++] = codepoint;
    }
    str = realloc(str, sizeof(uchar) * counter);
    *length = counter;
    return str;
}

/**
 * @brief 从UTF-8编码的字符串中获取下一个Unicode字符
 *
 * @param pos 字符串中当前位置的指针,调用函数后会更新为下一个位置
 * @return unsigned int 下一个Unicode字符的值
 */
static unsigned int get_next_uchar(char **pos) {
    char *p = *pos;
    if (*p == '\0') {
        // 字符串已经结束
        return 0;
    }
    if ((*p & 0x80) == 0) {
        // 这是一个单字节字符
        unsigned int codepoint = *p;
        (*pos)++;
        return codepoint;
    } else if ((*p & 0xE0) == 0xC0) {
        // 这是一个两字节字符
        unsigned int codepoint = (*p & 0x1F) << 6;
        p++;
        if ((*p & 0xC0) != 0x80) {
            // 不是合法的UTF-8编码
            fprintf(stderr, "Invalid UTF-8 encoding\n");
            exit(1);
        }
        codepoint |= (*p & 0x3F);
        (*pos) = p + 1;
        return codepoint;
    } else if ((*p & 0xF0) == 0xE0) {
        // 这是一个三字节字符
        unsigned int codepoint = (*p & 0x0F) << 12;
        p++;
        if ((*p & 0xC0) != 0x80) {
            // 不是合法的UTF-8编码
            fprintf(stderr, "Invalid UTF-8 encoding\n");
            exit(1);
        }
        codepoint |= (*p & 0x3F) << 6;
        p++;
        if ((*p & 0xC0) != 0x80) {
            // 不是合法的UTF-8编码
            fprintf(stderr, "Invalid UTF-8 encoding\n");
            exit(1);
        }
        codepoint |= (*p & 0x3F);
        (*pos) = p + 1;
        return codepoint;
    } else if ((*p & 0xF8) == 0xF0) {
        // 这是一个四字节字符
        unsigned int codepoint = (*p & 0x07) << 18;
        p++;
        if ((*p & 0xC0) != 0x80) {
            // 不是合法的UTF-8编码
            fprintf(stderr, "Invalid UTF-8 encoding\n");
            exit(1);
        }
        codepoint |= (*p & 0x3F) << 12;
        p++;
        if ((*p & 0xC0) != 0x80) {
            // 不是合法的UTF-8编码
            fprintf(stderr, "Invalid UTF-8 encoding\n");
            exit(1);
        }
        codepoint |= (*p & 0x3F) << 6;
        p++;
        if ((*p & 0xC0) != 0x80) {
            // 不是合法的UTF-8编码
            fprintf(stderr, "Invalid UTF-8 encoding\n");
            exit(1);
        }
        codepoint |= (*p & 0x3F);
        (*pos) = p + 1;
        return codepoint;
    } else {
        // 这是一个不合法的字节
        fprintf(stderr, "Invalid byte: %x\n", *p);
        exit(1);
    }
}

/**
 * @brief 将一个Unicode字符转换为UTF-8编码的字符串
 *
 * @param codepoint Unicode字符的值
 * @param buf 存储UTF-8编码的缓冲区, 5字节
 * @return int 编码后的字节数
 */
int XBOX_uchar2char(unsigned int codepoint, char *buf) {
    if (codepoint <= 0x7F) {
        // 单字节字符
        buf[0] = codepoint & 0xFF;
        buf[1] = '\0';
        return 1;
    } else if (codepoint <= 0x7FF) {
        // 两字节字符
        buf[0] = (unsigned char)(((codepoint >> 6) & 0x1F) | 0xC0);
        buf[1] = (unsigned char)((codepoint & 0x3F) | 0x80);
        buf[2] = '\0';
        return 2;
    } else if (codepoint <= 0xFFFF) {
        // 三字节字符
        buf[0] = (unsigned char)(((codepoint >> 12) & 0x0F) | 0xE0);
        buf[1] = (unsigned char)(((codepoint >> 6) & 0x3F) | 0x80);
        buf[2] = (unsigned char)((codepoint & 0x3F) | 0x80);
        buf[3] = '\0';
        return 3;
    } else if (codepoint <= 0x10FFFF) {
        // 四字节字符
        buf[0] = (unsigned char)(((codepoint >> 18) & 0x07) | 0xF0);
        buf[1] = (unsigned char)(((codepoint >> 12) & 0x3F) | 0x80);
        buf[2] = (unsigned char)(((codepoint >> 6) & 0x3F) | 0x80);
        buf[3] = (unsigned char)((codepoint & 0x3F) | 0x80);
        buf[4] = '\0';
        return 4;
    } else {
        // 不是合法的Unicode字符
        fprintf(stderr, "Invalid Unicode codepoint: %u\n", codepoint);
        exit(1);
    }
}