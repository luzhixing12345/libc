/*
 *Copyright (c) 2023 All rights reserved
 *@description: argument parser
 *@author: Zhixing Lu
 *@date: 2023-10-02
 *@email: luzhixing12345@163.com
 *@Github: luzhixing12345
 */

// usage: https://github.com/luzhixing12345/libc

#ifndef XBOX_XARGPARSE_H
#define XBOX_XARGPARSE_H

#define XBOX_VERSION "XBOX v0.0.1"

// #include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xstring.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define XBOX_LENGTH_MISMATCH 65
#define XBOX_UNSUPPORTED_TYPE 66
#define XBOX_REQUIRE_ARGUMENT 67
#define XBOX_FORMAT_ERROR 68

/*
    -s            --long-name       help information...
    -a <text>     --append <text>   Sentences that are too
                                    long will be automatically
                                    wrapped without breaking
                                    the words.
    -h            --help

◄──►         ◄───►               ◄─►

left_space   mid_space           right_space

◄─────────────────────────────────────────────────────────────►

                      total_space
 */

#define XBOX_HELP_INFO_LEFT_SPACE 2
#define XBOX_HELP_INFO_MID_SPACE 3
#define XBOX_HELP_INFO_RIGHT_SPACE 3
#define XBOX_HELP_INFO_LENGTH 80   // 帮助信息 80 字符换行
#define XBOX_HELP_INFO_INTERVAL 3  // 帮助信息列间距

#define __XBOX_ARGS_BUILD_ERROR "[Args Build Error]"
#define __XBOX_ARGS_PARSE_ERROR "[Args Parse Error]"
#define __XBOX_ARGS_PARSE_WARNING "[Args Parse Warning]"

#define __XBOX_ARGS_NEED_FREE(type) \
    (!((type == __ARGPARSE_OPT_BOOLEAN) || (type == __ARGPARSE_OPT_INT) || (type == __ARGPARSE_OPT_INT_GROUP)))

enum argparse_option_type {
    __ARGPARSE_OPT_END,
    __ARGPARSE_OPT_BOOLEAN,
    // 匹配单个参数
    __ARGPARSE_OPT_INT,
    __ARGPARSE_OPT_STR,
    // 匹配组
    __ARGPARSE_OPT_INT_GROUP,
    __ARGPARSE_OPT_STR_GROUP,
    // 匹配多个参数
    __ARGPARSE_OPT_INTS,
    __ARGPARSE_OPT_STRS,
    __ARGPARSE_OPT_INTS_GROUP,
    __ARGPARSE_OPT_STRS_GROUP
};

enum argparse_flag {
    XBOX_ARGPARSE_IGNORE_UNKNOWN = 1,         // 忽略未知参数, 比如未声明过的 -m
    XBOX_ARGPARSE_ENABLE_STICK = 1 << 1,      // 允许参数粘连 -O1 -Iinclude/
    XBOX_ARGPARSE_ENABLE_EQUAL = 1 << 2,      // 允许参数等号 -i=123
    XBOX_ARGPARSE_ENABLE_ARG_STICK = 1 << 3,  // 允许boolean类型参数粘连
    XBOX_ARGPARSE_SORT = 1 << 4               // 将参数按照字典序排序
};

typedef struct {
    enum argparse_option_type type;
    void *p;            // 用户绑定的值
    char *short_name;   // 短名字
    char *long_name;    // 长名字
    char *help_info;    // 帮助信息
    char *append_info;  // 补充信息
    char *name;         // 记录用的名字
    // 下面三个字段由内部维护
    char *value;  // 匹配的值
    int pos;      // 匹配的位置
    int match;    // 匹配的次数
} argparse_option;

/**
 * argpparse
 */
typedef struct {
    const char *name;
    argparse_option *options;
    const char *description;  // bind description after usage
    const char *epilog;       // bind description at the end
    int args_number;
    int flag;

} XBOX_argparse;

// built-in option macros

#define XBOX_ARG_BOOLEAN(bind, short_name, long_name, help_info, append_info, name) \
    { __ARGPARSE_OPT_BOOLEAN, bind, short_name, long_name, help_info, append_info, name }
#define XBOX_ARG_INT(bind, short_name, long_name, help_info, append_info, name) \
    { __ARGPARSE_OPT_INT, bind, short_name, long_name, help_info, append_info, name }
#define XBOX_ARG_STR(bind, short_name, long_name, help_info, append_info, name) \
    { __ARGPARSE_OPT_STR, bind, short_name, long_name, help_info, append_info, name }
#define XBOX_ARG_INTS(bind, short_name, long_name, help_info, append_info, name) \
    { __ARGPARSE_OPT_INTS, bind, short_name, long_name, help_info, append_info, name }
#define XBOX_ARG_STRS(bind, short_name, long_name, help_info, append_info, name) \
    { __ARGPARSE_OPT_STRS, bind, short_name, long_name, help_info, append_info, name }
#define XBOX_ARG_INT_GROUP(bind, short_name, long_name, help_info, append_info, name) \
    { __ARGPARSE_OPT_INT_GROUP, bind, short_name, long_name, help_info, append_info, name }
#define XBOX_ARG_INTS_GROUP(bind, short_name, long_name, help_info, append_info, name) \
    { __ARGPARSE_OPT_INTS_GROUP, bind, short_name, long_name, help_info, append_info, name }
#define XBOX_ARG_STR_GROUP(bind, short_name, long_name, help_info, append_info, name) \
    { __ARGPARSE_OPT_STR_GROUP, bind, short_name, long_name, help_info, append_info, name }
#define XBOX_ARG_STRS_GROUP(bind, short_name, long_name, help_info, append_info, name) \
    { __ARGPARSE_OPT_STRS_GROUP, bind, short_name, long_name, help_info, append_info, name }
#define XBOX_ARG_END() \
    { __ARGPARSE_OPT_END }

/**
 * @brief 初始化 argparser
 *
 * @param parser
 * @param options
 * @param flag
 */
void XBOX_argparse_init(XBOX_argparse *parser, argparse_option *options, int flag);

/**
 * @brief 添加程序的描述信息
 *
 * @param parser
 * @param name 程序名字
 * @param description 开头的描述信息
 * @param epilog 结尾的描述信息
 */
void XBOX_argparse_describe(XBOX_argparse *parser, const char *name, const char *description, const char *epilog);

/**
 * @brief 默认的 help 信息输出
 *
 * @param parser
 */
void XBOX_argparse_info(XBOX_argparse *parser);

/**
 * @brief 解析命令行参数
 *
 * @param parser
 * @param argc
 * @param argv
 */
void XBOX_argparse_parse(XBOX_argparse *parser, int argc, const char **argv);

/**
 * @brief 参数是否匹配
 *
 * @param parser
 * @param name
 * @return int 如果未匹配返回0; 如果匹配,返回值为匹配的个数
 */
int XBOX_ismatch(XBOX_argparse *parser, char *name);

/**
 * @brief 查找参数匹配的位置

   ./main 100 -i 200
          |    | |
          1    2 3

   ./main -abc -d 100
           |||  | |
           123  4 5
 *
 * @param parser
 * @param name
 * @return int
 */
int XBOX_match_pos(XBOX_argparse *parser, char *name);

/**
 * @brief 释放 parser 的内存, 请在程序结束/不再使用 parser 时调用
 *
 * @param parser
 */
void XBOX_free_argparse(XBOX_argparse *parser);

#endif  // XBOX_XARGPARSE_H