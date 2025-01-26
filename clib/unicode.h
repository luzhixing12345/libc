
#pragma once

#include <stddef.h>
#include <sys/types.h>

typedef unsigned int uchar;

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
