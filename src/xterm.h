


#ifndef XBOX_XTERM_H
#define XBOX_XTERM_H

#include "xstring.h"

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
    char *word;  // 文本信息
} XBOX_term_word;

// 最大长度, 一般来说后缀和虚拟控制序列序号都不会很长, 不想再 malloc/free 了
#define MAX_DC_KV_LENGTH 16

typedef struct {
    char key[MAX_DC_KV_LENGTH];
    char value[MAX_DC_KV_LENGTH];
} dc_kv;

typedef struct {
    char *rs, *di, *ln, *mh, *pi, *so, *door, *bd, *cd;
    char *orp, *mi, *su, *sg, *ca, *tw, *ow, *st, *ex;
    dc_kv *dc_kvs;  // 所有 LS_COLORS 的键值对
    int item_number;
} XBOX_dircolor_database;



void XBOX_print_invalid_color_option();

/**
 * @brief 终端彩色打印
 *
 * @param word
 * @return char* 返回拼接后的字符串
 */
char *XBOX_colorful_print(XBOX_term_word *word);


/**
 * @brief 初始化 dircolors 的颜色数据库
 *
 * @param database (need free)
 */
void XBOX_init_dc_database(XBOX_dircolor_database **database);

/**
 * @brief 释放 dircolors 数据库
 * 
 * @param database 
 */
void XBOX_free_dc_database(XBOX_dircolor_database *database);

/**
 * @brief 使用虚拟控制序列打印文件名
 * 
 * @param file_name 
 * @param full_path 
 * @param database 
 * @return char* 
 */
const char *XBOX_filename_print(const char *file_name, const char *full_path, XBOX_dircolor_database *database);

#endif // XBOX_XTERM_H
