/*
 *Copyright (c) 2023 All rights reserved
 *@description: argument parser
 *@author: Zhixing Lu
 *@date: 2023-04-28
 *@email: luzhixing12345@163.com
 *@Github: luzhixing12345
 */

#ifndef XBOX_XARGPARSE_H
#define XBOX_XARGPARSE_H

const char *XBOX_VERSION = "XBOX v0.0.1";

// #include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xstring.h"

#define MAX(bind, b) ((bind) > (b) ? (bind) : (b))

#define XBOX_LENGTH_MISMATCH 65
#define XBOX_UNSUPPORTED_TYPE 66
#define XBOX_REQUIRE_ARGUMENT 67
#define XBOX_FORMAT_ERROR 68

#define XBOX_HELP_INFO_INTERVAL 3  // 帮助信息列间距
#define XBOX_HELP_INFO_LENGTH 80   // 帮助信息 80 字符换行

#define XBOX_ARGS_BUILD_ERROR "[Args Build Error]"
#define XBOX_ARGS_PARSE_ERROR "[Args Parse Error]"
#define XBOX_ARGS_PARSE_WARNING "[Args Parse Warning]"

#define XBOX_ARGS_NEED_FREE(type) \
    (!((type == ARGPARSE_OPT_BOOLEAN) || (type == ARGPARSE_OPT_INT) || (type == ARGPARSE_OPT_INT_GROUP)))

enum argparse_option_type {
    ARGPARSE_OPT_END,
    ARGPARSE_OPT_BOOLEAN,
    // 匹配单个参数
    ARGPARSE_OPT_INT,
    ARGPARSE_OPT_STR,
    // 匹配组
    ARGPARSE_OPT_INT_GROUP,
    ARGPARSE_OPT_STR_GROUP,
    // 匹配多个参数
    ARGPARSE_OPT_INTS,
    ARGPARSE_OPT_STRS,
    ARGPARSE_OPT_INTS_GROUP,
    ARGPARSE_OPT_STRS_GROUP
};

enum argparse_flag {
    XBOX_ARGPARSE_IGNORE_WARNING = 1,         // 忽略警告
    XBOX_ARGPARSE_ENABLE_STICK = 1 << 1,      // 允许参数粘连 -O1 -Iinclude/
    XBOX_ARGPARSE_ENABLE_EQUAL = 1 << 2,      // 允许参数等号 -i=123
    XBOX_ARGPARSE_ENABLE_ARG_STICK = 1 << 3,  // 允许boolean类型参数粘连
    XBOX_ARGPARSE_SORT = 1 << 4               // 将参数按照字典序排序
};

typedef struct {
    enum argparse_option_type type;
    void *p;
    char *short_name;   // 短名字
    char *long_name;    // 长名字
    char *help_info;    // 帮助信息
    char *append_info;  // 补充信息
    char *name;         // 记录用的名字
    char *value;
    int match;
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
    { ARGPARSE_OPT_BOOLEAN, bind, short_name, long_name, help_info, append_info, name }
#define XBOX_ARG_INT(bind, short_name, long_name, help_info, append_info, name) \
    { ARGPARSE_OPT_INT, bind, short_name, long_name, help_info, append_info, name }
#define XBOX_ARG_STR(bind, short_name, long_name, help_info, append_info, name) \
    { ARGPARSE_OPT_STR, bind, short_name, long_name, help_info, append_info, name }
#define XBOX_ARG_INTS(bind, short_name, long_name, help_info, append_info, name) \
    { ARGPARSE_OPT_INTS, bind, short_name, long_name, help_info, append_info, name }
#define XBOX_ARG_STRS(bind, short_name, long_name, help_info, append_info, name) \
    { ARGPARSE_OPT_STRS, bind, short_name, long_name, help_info, append_info, name }
#define XBOX_ARG_INT_GROUP(bind, short_name, long_name, help_info, append_info, name) \
    { ARGPARSE_OPT_INT_GROUP, bind, short_name, long_name, help_info, append_info, name }
#define XBOX_ARG_INTS_GROUP(bind, short_name, long_name, help_info, append_info, name) \
    { ARGPARSE_OPT_INTS_GROUP, bind, short_name, long_name, help_info, append_info, name }
#define XBOX_ARG_STR_GROUP(bind, short_name, long_name, help_info, append_info, name) \
    { ARGPARSE_OPT_STR_GROUP, bind, short_name, long_name, help_info, append_info, name }
#define XBOX_ARG_STRS_GROUP(bind, short_name, long_name, help_info, append_info, name) \
    { ARGPARSE_OPT_STRS_GROUP, bind, short_name, long_name, help_info, append_info, name }
#define XBOX_ARG_END() \
    { ARGPARSE_OPT_END }

/**
 * @brief 释放 parser 的内存, 请在程序结束/不再使用 parser 时调用
 *
 * @param parser
 */
void XBOX_free_argparse(XBOX_argparse *parser) {
    // fprintf(stderr, "argument parse error, free options\n");
    for (int i = 0; i < parser->args_number; i++) {
        argparse_option *option = &(parser->options[i]);
        if (option->value) {
            free(option->value);
        }
        if (XBOX_ARGS_NEED_FREE(option->type) && option->match) {
            if (option->type == ARGPARSE_OPT_STR || option->type == ARGPARSE_OPT_STR_GROUP) {
                free(*(char **)option->p);
            } else if (option->type == ARGPARSE_OPT_INTS || option->type == ARGPARSE_OPT_INTS_GROUP) {
                free(*(int **)option->p);
            } else {
                for (int i = 0; i < option->match; i++) {
                    free((*(char ***)option->p)[i]);
                }
                free(*(char ***)option->p);
            }
        }
    }

    // fprintf(stderr, "finished free options\n");
}

/**
 * @brief (原地操作)去除字符串开头结尾的的空格和双引号 ""
 *
 * @param str
 * @return void
 */
void XBOX_trim(char **str_p) {
    char *new_str;
    char *str = *str_p;
    while (*str == ' ') {
        str++;
    }
    if (*str == '\"') {
        str++;
    }
    int length = strlen(str);
    while (str[length - 1] == ' ') {
        length--;
    }
    if (str[length - 1] == '\"') {
        length--;
    }
    new_str = (char *)malloc(length + 1);
    strncpy(new_str, str, length);
    new_str[length] = '\0';
    free((char *)*str_p);
    *str_p = (char *)new_str;
}

/**
 * @brief 切片
 *
 * @param str
 * @param start 起点index(包含)
 * @param end 终点index(包含), end = -1 表示结尾
 * @return char*(need free)
 */
char *XBOX_splice(const char *str, int start, int end) {
    if (end == -1) {
        end = strlen(str) - 1;
    }
    char *s = (char *)malloc(sizeof(char) * (end - start + 2));
    strncpy(s, str + start, end - start + 1);
    s[end - start + 1] = '\0';
    return s;
}

int check_valid_character(const char *str) {
    int length = strlen(str);
    for (int i = 0; i < length; i++) {
        if (!(islower(str[i]) || str[i] == '_' || str[i] == '-')) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief 检验参数合法性
 *
 * @param parser
 */
void check_valid_options(XBOX_argparse *parser) {
    // group 不重名
    for (int i = 0; i < parser->args_number; i++) {
        argparse_option *option = &(parser->options[i]);
        if (option->long_name) {
            char *l_name = XBOX_splice(option->long_name, 2, -1);
            char *p = l_name;

            // long_name 合法性 -> 小写 - _
            if (check_valid_character(p)) {
                fprintf(stderr, "%s: only [bind-z_-] are legal characters instead of [%s]\n", XBOX_ARGS_BUILD_ERROR, p);
                free(p);
                exit(XBOX_FORMAT_ERROR);
            }
            free(p);
        }
    }

    for (int i = 0; i < parser->args_number; i++) {
        argparse_option *option1 = &(parser->options[i]);
        for (int j = i + 1; j < parser->args_number; j++) {
            argparse_option *option2 = &(parser->options[j]);
            // 没有两个参数同名
            if (option1->name && option2->name && !strcmp(option1->name, option2->name)) {
                fprintf(stderr, "%s: options have the same name [%s]\n", XBOX_ARGS_BUILD_ERROR, option1->name);
                XBOX_free_argparse(parser);
                exit(XBOX_FORMAT_ERROR);
            }
            // 没有两个长参数同名
            if (option1->long_name && option2->long_name) {
                if (!strcmp(option1->long_name, option2->long_name)) {
                    fprintf(stderr,
                            "%s: options have the same long_name [%s]\n",
                            XBOX_ARGS_BUILD_ERROR,
                            option1->long_name);
                    XBOX_free_argparse(parser);
                    exit(XBOX_FORMAT_ERROR);
                }
            }
            // 没有两个短参数同名
            if (option1->short_name && option2->short_name) {
                if (!strcmp(option1->short_name, option2->short_name)) {
                    fprintf(stderr,
                            "%s: options have the same short_name [%s]\n",
                            XBOX_ARGS_BUILD_ERROR,
                            option1->short_name);
                    XBOX_free_argparse(parser);
                    exit(XBOX_FORMAT_ERROR);
                }
            }
        }
    }

    // if (parser->flag & XBOX_ARGPARSE_ENABLE_ARG_STICK &&
    //     ((parser->flag & XBOX_ARGPARSE_ENABLE_EQUAL) || (parser->flag &
    //     XBOX_ARGPARSE_ENABLE_STICK))) { fprintf(stderr,
    //             "%s: flag collasp for XBOX_ARGPARSE_ENABLE_EQUAL with
    //             XBOX_ARGPARSE_ENABLE_EQUAL or " "XBOX_ARGPARSE_ENABLE_STICK\n",
    //             XBOX_ARGS_BUILD_ERROR);
    //     XBOX_free_argparse(parser);
    //     exit(XBOX_FORMAT_ERROR);
    // }
}

/**
 * @brief 初始化 argparser
 *
 * @param parser
 * @param options
 * @param flag
 */
void XBOX_argparse_init(XBOX_argparse *parser, argparse_option *options, int flag) {
    memset(parser, 0, sizeof(*parser));
    parser->name = NULL;
    parser->description = NULL;
    parser->epilog = NULL;
    parser->options = options;
    parser->args_number = 0;
    parser->flag = flag;
    while (options->type != ARGPARSE_OPT_END) {
        options++;
        parser->args_number++;
    }
    for (int i = 0; i < parser->args_number; i++) {
        argparse_option *option = &(parser->options[i]);
        option->value = NULL;
        option->match = 0;
        // printf("option short name = [%s], long name = [%s]\n", option->short_name, option->long_name);
    }
    check_valid_options(parser);
    return;
}

/**
 * @brief 添加程序的描述信息
 *
 * @param parser
 * @param name 程序名字
 * @param description 开头的描述信息
 * @param epilog 结尾的描述信息
 */
void XBOX_argparse_describe(XBOX_argparse *parser, const char *name, const char *description, const char *epilog) {
    parser->name = name;
    if (strlen(description) == 0) {
        description = "About how to use the program";
    }
    parser->description = description;
    if (epilog == NULL || strlen(epilog) == 0) {
        epilog = "xargparse online help: <https://github.com/luzhixing12345/libc>";
    }
    parser->epilog = epilog;
    return;
}

/**
 * @brief 传递参数
 *
 * @param parser
 * @param option
 */
void value_pass(XBOX_argparse *parser, argparse_option *option);

/**
 * @brief 去除字符串中的 \\
 *
 * @param value
 * @return char*
 */
char *clear_escape_charaters(char *value) {
    int length = strlen(value);
    int escape_char_number = 0;
    for (int i = 0; i < length; i++) {
        if (value[i] == '\\') {
            escape_char_number += 1;
        }
    }
    if (!escape_char_number) {
        return value;
    }
    int pos = 0;
    for (int i = 0; i < length; i++) {
        if (value[i] != '\\') {
            value[pos++] = value[i];
        } else {
            value[pos++] = value[++i];
        }
    }
    // printf("length = %d pos = %d x = %d\n", length, pos, length -
    // escape_char_number);
    value[pos] = 0;
    value = (char *)realloc(value, length - escape_char_number + 1);
    return value;
}

/**
 * @brief 比较参数顺序, 小写字典序
 *
 * @param bind
 * @param b
 * @return int
 */
static int option_cmp(const void *bind, const void *b) {
    argparse_option *p1 = (argparse_option *)bind;
    argparse_option *p2 = (argparse_option *)b;
    if (p1->short_name) {
        if (p2->short_name) {
            char c1 = (p1->short_name)[1];
            char c2 = (p2->short_name)[1];
            char C1 = c1, C2 = c2;
            if (c1 & 0x20) {
                C1 = c1 ^ 0x20;
            }
            if (c2 & 0x20) {
                C2 = c2 ^ 0x20;
            }
            if (C1 == C2) {
                return c1 < c2;
            } else {
                return C1 > C2;
            }
        } else
            return 0;
    }
    return 1;
}

/**
 * @brief 默认的 help 信息输出
 *
 * @param parser
 */
void XBOX_argparse_info(XBOX_argparse *parser) {
    printf("Usage: %s ", parser->name);
    int counter = 0;
    for (int i = 0; i < parser->args_number; i++) {
        argparse_option *option = &(parser->options[i]);
        if (option->type == ARGPARSE_OPT_STR_GROUP || option->type == ARGPARSE_OPT_INT_GROUP ||
            option->type == ARGPARSE_OPT_STRS_GROUP || option->type == ARGPARSE_OPT_INTS_GROUP) {
            counter++;
        }
    }
    if (counter != parser->args_number) {
        printf("[OPTION]... ");
    }
    for (int i = 0; i < parser->args_number; i++) {
        argparse_option *option = &(parser->options[i]);
        if (option->type == ARGPARSE_OPT_STR_GROUP || option->type == ARGPARSE_OPT_INT_GROUP) {
            printf("[%s] ", option->name);
        } else if (option->type == ARGPARSE_OPT_STRS_GROUP || option->type == ARGPARSE_OPT_INTS_GROUP) {
            printf("[%s]... ", option->name);
        }
    }
    printf("\n");
    if (parser->description) {
        printf("%s\n", parser->description);
    }
    printf("\n");

    int left_width = 0;  // 最左侧短参数对齐长度
    int mid_width = 0;   // 中间长参数对齐长度
    for (int i = 0; i < parser->args_number; i++) {
        argparse_option *option = &(parser->options[i]);
        if (option->short_name && option->append_info) {
            left_width = MAX(left_width, strlen(option->append_info));
        }
        if (option->long_name) {
            int option_length = strlen(option->long_name);
            if (!option->short_name && option->append_info) {
                option_length += strlen(option->append_info);
            }
            mid_width = MAX(mid_width, option_length);
        }
    }
    // printf("max_length = %d\n",max_length);
    left_width += XBOX_HELP_INFO_INTERVAL;
    mid_width += XBOX_HELP_INFO_INTERVAL;

    if ((parser->flag & XBOX_ARGPARSE_SORT)) {
        qsort(parser->options, parser->args_number, sizeof(argparse_option), option_cmp);
    }

    for (int i = 0; i < parser->args_number; i++) {
        argparse_option *option = &(parser->options[i]);
        if (option->type == ARGPARSE_OPT_INT_GROUP || option->type == ARGPARSE_OPT_STR_GROUP ||
            option->type == ARGPARSE_OPT_STRS_GROUP || option->type == ARGPARSE_OPT_INTS_GROUP) {
            continue;
        }

        printf("  %s", option->short_name == NULL ? "  " : option->short_name);
        printf("%-*s", left_width, (option->append_info == NULL || !option->short_name) ? "" : option->append_info);
        int mid_rest_space = mid_width;
        if (option->long_name) {
            printf("%s", option->long_name);
            mid_rest_space -= strlen(option->long_name);
            if (!option->short_name && option->append_info) {
                printf("%s", option->append_info);
                mid_rest_space -= strlen(option->append_info);
            }
        }
        printf("%*s", mid_rest_space, "");
        if (option->help_info) {
            int help_info_length = strlen(option->help_info);
            int right_help_info_space = XBOX_HELP_INFO_LENGTH - left_width - mid_width - 4;
            if (help_info_length > right_help_info_space) {
                char help_info[XBOX_HELP_INFO_LENGTH];
                memset(help_info, 0, XBOX_HELP_INFO_LENGTH);
                int p = 0;
                while (help_info_length > right_help_info_space) {
                    strncpy(help_info, option->help_info + p, right_help_info_space);
                    help_info[right_help_info_space] = 0;
                    p += right_help_info_space;
                    printf("%s\n%*s", help_info, left_width + mid_width + 4, "");
                    help_info_length -= right_help_info_space;
                }
                printf("%s", option->help_info + p);
            } else {
                printf("%s", option->help_info);
            }
        }
        printf("\n");
    }
    if (parser->epilog) {
        printf("\n%s\n", parser->epilog);
    }
}

/**
 * @brief 判断长参数是否匹配
 *
 * @param parser
 * @param str
 * @return argparse_option 返回匹配的option, 否则返回 NULL
 */
argparse_option *check_argparse_loptions(XBOX_argparse *parser, const char *str) {
    for (int i = 0; i < parser->args_number; i++) {
        argparse_option *option = &(parser->options[i]);
        if (option->type == ARGPARSE_OPT_INT_GROUP || option->type == ARGPARSE_OPT_STR_GROUP ||
            option->type == ARGPARSE_OPT_INTS_GROUP || option->type == ARGPARSE_OPT_STRS_GROUP) {
            continue;
        }
        if (option->long_name && !strcmp(option->long_name, str)) {
            return option;
        }
    }
    return NULL;
}

/**
 * @brief 判断短参数是否匹配, 并修改 option->match
 *
 * @param parser
 * @param str
 * @return argparse_option* 返回匹配的option, 否则返回 NULL
 */
argparse_option *check_argparse_soptions(XBOX_argparse *parser, const char *str) {
    for (int i = 0; i < parser->args_number; i++) {
        argparse_option *option = &(parser->options[i]);
        if (option->type == ARGPARSE_OPT_INT_GROUP || option->type == ARGPARSE_OPT_STR_GROUP ||
            option->type == ARGPARSE_OPT_INTS_GROUP || option->type == ARGPARSE_OPT_STRS_GROUP) {
            continue;
        }
        if (option->short_name && !strcmp(option->short_name, str)) {
            // printf("matched %s\n", option->short_name);
            return option;
        }
    }
    return NULL;
}

/**
 * @brief 解析组, 修改match和value
 *
 * @param parser
 * @param str
 * @return int 如果已经没有可解析的组则出现错误,返回1
 */
int check_argparse_groups(XBOX_argparse *parser, const char *str) {
    for (int i = 0; i < parser->args_number; i++) {
        argparse_option *option = &(parser->options[i]);

        if (option->type == ARGPARSE_OPT_INT_GROUP || option->type == ARGPARSE_OPT_STR_GROUP) {
            if (!option->match) {
                if (option->value) {
                    free(option->value);
                }
                option->value = (char *)malloc(sizeof(char) * (strlen(str) + 1));
                strcpy(option->value, str);
                // printf("matched [%s] for group [%s]\n", option->value, option->name);
                value_pass(parser, option);
                return 0;
            }
        }
        if (option->type == ARGPARSE_OPT_INTS_GROUP || option->type == ARGPARSE_OPT_STRS_GROUP) {
            if (option->value) {
                free(option->value);
            }
            option->value = (char *)malloc(sizeof(char) * (strlen(str) + 1));
            strcpy(option->value, str);
            // printf("[%d]: matched [%s] for group [%s]\n", __LINE__, option->value,
            // option->name);
            value_pass(parser, option);
            return 0;
        }
    }
    return 1;
}

/**
 * @brief 传递解析后的参数
 *
 * @param option
 * @param enable_multi 是否允许多个分离参数
 */
void value_pass(XBOX_argparse *parser, argparse_option *option) {
    // 警告信息, 覆盖
    // if (option->match && (option->type == ARGPARSE_OPT_STR || option->type == ARGPARSE_OPT_INT) &&
    //     !(parser->flag & XBOX_ARGPARSE_IGNORE_WARNING)) {
    //     fprintf(stderr, "%s: ignore argument [%s]\n", XBOX_ARGS_PARSE_WARNING, option->value);
    //     return;
    // }

    // 单个匹配
    if ((option->type == ARGPARSE_OPT_STR) || option->type == ARGPARSE_OPT_STR_GROUP) {
        if (option->match >= 1) {
            free(*(char **)option->p);
        }
        *(char **)option->p = (char *)malloc(strlen(option->value) + 1);
        strcpy(*(char **)option->p, option->value);
        option->match = 1;
        return;
    } else if ((option->type == ARGPARSE_OPT_INT) || option->type == ARGPARSE_OPT_INT_GROUP) {
        int value = 0;
        int signal = 1;
        char *temp = option->value;
        if (*temp == '-') {
            signal = -1;
            temp++;
        }
        while (*temp != '\0') {
            if (*temp < '0' || *temp > '9') {
                fprintf(stderr, "%s: argument assign to be int but get [%s]\n", XBOX_ARGS_PARSE_ERROR, option->value);
                XBOX_free_argparse(parser);
                exit(XBOX_FORMAT_ERROR);
            }
            value = value * 10 + (*temp) - '0';
            temp++;
        }
        *(int *)option->p = value * signal;
        option->match = 1;
        return;
    }
    // 多个匹配的情况
    // -D __GNU__ -D __KERNEL
    int match_number = ++option->match;
    if (option->type == ARGPARSE_OPT_STRS || option->type == ARGPARSE_OPT_STRS_GROUP) {
        char **new_p;
        if (match_number == 1) {
            new_p = (char **)realloc(NULL, sizeof(char *) * match_number);
        } else {
            new_p = (char **)realloc(*(char ***)option->p, sizeof(char *) * match_number);
        }
        *(char ***)option->p = new_p;
        char *value_str = XBOX_splice(option->value, 0, -1);
        (*(char ***)option->p)[match_number - 1] = value_str;

    } else if (option->type == ARGPARSE_OPT_INTS || option->type == ARGPARSE_OPT_INTS_GROUP) {
        int value = 0;
        int signal = 1;
        char *temp = option->value;
        if (*temp == '-') {
            signal = -1;
            temp++;
        }
        while (*temp != '\0') {
            if (*temp < '0' || *temp > '9') {
                fprintf(stderr, "%s: argument assign to be int but get [%s]\n", XBOX_ARGS_PARSE_ERROR, option->value);
                XBOX_free_argparse(parser);
                exit(XBOX_FORMAT_ERROR);
            }
            value = value * 10 + (*temp) - '0';
            temp++;
        }
        int *new_p;
        if (match_number == 1) {
            new_p = (int *)realloc(NULL, sizeof(int) * match_number);
        } else {
            new_p = (int *)realloc(*(int **)option->p, sizeof(int) * match_number);
        }
        (*(int **)option->p) = new_p;
        (*(int **)option->p)[match_number - 1] = value * signal;
    } else {
        fprintf(stderr, "%s: unknown option type for [%s]\n", XBOX_ARGS_PARSE_ERROR, option->name);
        XBOX_free_argparse(parser);
        exit(XBOX_FORMAT_ERROR);
    }
}

void XBOX_argparse_parse(XBOX_argparse *parser, int argc, const char **argv) {
    for (int i = 1; i < argc; i++) {
        int argv_length = strlen(argv[i]);
        if (argv_length >= 2 && argv[i][0] == '-') {
            // --long_name
            argparse_option *option;
            if (argv[i][1] == '-') {
                option = check_argparse_loptions(parser, argv[i]);
            } else {
                option = check_argparse_soptions(parser, argv[i]);
            }

            if (option == NULL) {
                if (parser->flag & XBOX_ARGPARSE_ENABLE_EQUAL) {
                    int pos = XBOX_findChar(argv[i], '=', 0);
                    if (pos != -1) {
                        char *name = XBOX_splice(argv[i], 0, pos - 1);
                        if (strlen(name) >= 2 && name[0] == '-') {
                            if (name[1] == '-') {
                                option = check_argparse_loptions(parser, name);
                            } else {
                                option = check_argparse_soptions(parser, name);
                            }
                        }

                        free(name);
                        if (option) {
                            if (option->value) {
                                free(option->value);
                            }
                            char *value = XBOX_splice(argv[i], pos + 1, -1);
                            option->value = value;
                            value_pass(parser, option);
                            continue;
                        }
                    }
                }
                if (parser->flag & XBOX_ARGPARSE_ENABLE_STICK) {
                    char *short_name = XBOX_splice(argv[i], 0, 1);
                    option = check_argparse_soptions(parser, short_name);
                    free(short_name);
                    if (option) {
                        // boolean 类型的不去判断粘连情况
                        if (option->type != ARGPARSE_OPT_BOOLEAN) {
                            if (option->value) {
                                free(option->value);
                            }
                            char *value = XBOX_splice(argv[i], 2, -1);
                            option->value = value;
                            value_pass(parser, option);
                            continue;
                        }
                        // if (option->type == ARGPARSE_OPT_BOOLEAN && !(parser->flag &
                        // XBOX_ARGPARSE_IGNORE_WARNING)) {
                        //     fprintf(stderr,
                        //             "%s: Boolean argument [%s] is sticky in [%s], do you
                        //             mean " "XBOX_ARGPARSE_ENABLE_ARG_STICK?\n",
                        //             XBOX_ARGS_PARSE_ERROR,
                        //             option->short_name,
                        //             argv[i]);
                        //     XBOX_free_argparse(parser);
                        //     exit(XBOX_FORMAT_ERROR);
                        // }
                    }
                }
                if ((parser->flag & XBOX_ARGPARSE_ENABLE_ARG_STICK) && argv[i][1] != '-') {
                    char s[3] = {'-', '0', '\0'};
                    int n = strlen(argv[i]);
                    for (int j = 1; j < n; j++) {
                        s[1] = argv[i][j];
                        option = check_argparse_soptions(parser, s);
                        if (option == NULL) {
                            fprintf(stderr, "%s: no match options for [%s]\n", XBOX_ARGS_PARSE_ERROR, argv[i]);
                            XBOX_free_argparse(parser);
                            exit(XBOX_FORMAT_ERROR);
                        } else {
                            if (option->type != ARGPARSE_OPT_BOOLEAN) {
                                fprintf(stderr,
                                        "%s: only boolean type should be sticky for [%c] in "
                                        "[%s]\n",
                                        XBOX_ARGS_PARSE_ERROR,
                                        argv[i][j],
                                        argv[i]);
                                XBOX_free_argparse(parser);
                                exit(XBOX_FORMAT_ERROR);
                            } else {
                                option->match = 1;
                                if (option->p) {
                                    *(int *)option->p = 1;
                                }
                            }
                        }
                    }
                    continue;
                }
                fprintf(stderr, "%s: no match options for [%s]\n", XBOX_ARGS_PARSE_ERROR, argv[i]);
                XBOX_free_argparse(parser);
                exit(XBOX_FORMAT_ERROR);

            } else {
                if (option->type == ARGPARSE_OPT_BOOLEAN) {
                    option->match = 1;
                    if (option->p) {
                        *(int *)option->p = 1;
                    }
                    continue;
                }
                // 正确解析, 读取下一个参数
                if (i == argc - 1) {
                    fprintf(stderr, "%s: option [%s] needs one argument\n", XBOX_ARGS_PARSE_ERROR, option->long_name);
                    XBOX_free_argparse(parser);
                    exit(XBOX_FORMAT_ERROR);
                }
                // if (argv[i + 1][0] == '-' && !(parser->flag &
                // XBOX_ARGPARSE_IGNORE_WARNING)) {
                //     fprintf(stderr,
                //             "%s: [%s] will be passed as the argument for [%s]\n",
                //             XBOX_ARGS_PARSE_WARNING,
                //             argv[i + 1],
                //             argv[i]);
                // }
                if (option->value) {
                    free(option->value);
                }
                option->value = (char *)malloc(sizeof(char) * (strlen(argv[i + 1]) + 1));
                strcpy(option->value, argv[i + 1]);
                value_pass(parser, option);
                // printf("matched [%s]:[%s]\n", option->long_name, argv[i + 1]);
                i++;
            }
            continue;
        }

        // 当作正常参数传入
        if (check_argparse_groups(parser, argv[i])) {
            if (!(parser->flag & XBOX_ARGPARSE_IGNORE_WARNING)) {
                fprintf(stderr, "%s: [%s] doesn't match any left group\n", XBOX_ARGS_PARSE_WARNING, argv[i]);
            }
        }
    }

    // XBOX_MUST 检查必传参数
    // for (int i = 0; i < parser->args_number; i++) {
    //     argparse_option *option = &(parser->options[i]);
    //     if (option->must) {
    //         if (!option->match) {
    //             fprintf(stderr, "Error: argument option [%s] is must\n",
    //             option->name); XBOX_free_argparse(parser);
    //             exit(XBOX_FORMAT_ERROR);
    //         }
    //     }
    // }
}

/**
 * @brief 参数是否匹配
 *
 * @param parser
 * @param name
 * @return int 如果未匹配返回0; 如果匹配,返回值为匹配的个数
 */
int XBOX_ismatch(XBOX_argparse *parser, char *name) {
    if (strlen(name) > 2) {
        if (name[0] == '-' && name[1] == '-') {
            if (!(parser->flag & XBOX_ARGPARSE_IGNORE_WARNING)) {
                char *new_name = XBOX_splice(name, 2, -1);
                fprintf(stderr,
                        "%s: detected [%s] in XBOX_ismatch, do you mean [%s]?\n",
                        XBOX_ARGS_PARSE_WARNING,
                        name,
                        new_name);
                free(new_name);
            }
        }
    }

    for (int i = 0; i < parser->args_number; i++) {
        argparse_option *option = &(parser->options[i]);
        if (option->name && !strcmp(option->name, name)) {
            return option->match;
        }
    }
    fprintf(stderr, "%s: no matched name in options for [%s]\n", XBOX_ARGS_PARSE_WARNING, name);
    return 0;
}

#endif  // XBOX_XARGPARSE_H
