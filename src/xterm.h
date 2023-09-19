/*
 *Copyright (c) 2023 All rights reserved
 *@description: 终端显示设置
 *@author: Zhixing Lu
 *@date: 2023-08-27
 *@email: luzhixing12345@163.com
 *@Github: luzhixing12345
 */

#pragma once

#include <stdio.h>
#include <string.h>

// 终端字体设置

#define XBOX_TERM_FONT_DEFAULT "\033[0m"        // 默认字体
#define XBOX_TERM_FONT_BOLD "\033[1m"           // 高亮前景色
#define XBOX_TERM_FONT_NO_BLOD "\033[22m"       // 取消高亮前景色
#define XBOX_TERM_FONT_UNDERLINE "\033[4m"      // 下划线
#define XBOX_TERM_FONT_NO_UNDERLINE "\033[24m"  // 取消下划线

#define XBOX_TERM_COLOR_BLACK "\033[30m"
#define XBOX_TERM_COLOR_RED "\033[31m"
#define XBOX_TERM_COLOR_GREEN "\033[32m"
#define XBOX_TERM_COLOR_YELLOW "\033[33m"
#define XBOX_TERM_COLOR_BLUE "\033[34m"
#define XBOX_TERM_COLOR_MAGENTA "\033[35m"  // 品红色
#define XBOX_TERM_COLOR_CYAN "\033[36m"
#define XBOX_TERM_COLOR_WHITE "\033[37m"
#define XBOX_TERM_COLOR_EXTEND "\033[38m"  // 前景色扩展
#define XBOX_TERM_COLOR_DEFAULT "\033[39m"

typedef struct {
    char *font_type;
    char *front_color;  // 前景色
    char *back_color;   // 背景色
    char *front_rgb;
    char *back_rgb;
    char *word; // 文本信息
} XBOX_term_word;

/**
 * @brief 终端彩色打印
 * 
 * @param word 
 * @return char* 返回拼接后的字符串
 */
char *XBOX_colorful_print(XBOX_term_word *word) {
    static char vt_seq[1024];
    int pos = 0;
    char *p;
    if (!word->word) {
        return NULL;
    }
    if (word->font_type) {
        strcpy(vt_seq + pos, word->font_type);
        pos += strlen(word->font_type);
    }
    if (word->front_color) {
        strcpy(vt_seq + pos, word->front_color);
        pos += strlen(word->front_color);
    }
    if (word->back_color) {
        strcpy(vt_seq + pos, word->back_color);
        pos += strlen(word->back_color);
    }
    if (word->front_rgb) {
        strcpy(vt_seq + pos, word->front_rgb);
        pos += strlen(word->front_rgb);
    }
    if (word->back_rgb) {
        strcpy(vt_seq + pos, word->back_rgb);
        pos += strlen(word->back_rgb);
    }
    strcpy(vt_seq + pos, word->word);
    pos += strlen(word->word);
    p = (char*)XBOX_TERM_FONT_DEFAULT;
    strcpy(vt_seq + pos, p);
    pos += strlen(p);
    vt_seq[pos] = 0;
    return vt_seq;
    
}