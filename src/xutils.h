

#pragma once

#include <dirent.h>
#include <linux/limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define XBOX_LOG(fmt, ...) printf("[%s]:[%4d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define XBOX_TYPE(t) #t
#define XBOX_NAME(name) _##name

#define XBOX_DIR_ALL 0
#define XBOX_DIR_IGNORE_HIDDEN 1
#define XBOX_DIR_IGNORE_CURRENT (1<<1)


#define XBOX_PRINT_BUFFER_SIZE 1024
#define XBOX_MAX_INPUT_SIZE 1024

#define XBOX_IS_DIR(dp) (dp->type == DT_DIR)

typedef struct XBOX_File {
    unsigned char type;   // 文件类型
    char name[NAME_MAX];  // 文件名
} XBOX_File;

typedef struct XBOX_Dir {
    char name[NAME_MAX];
    int count;    // 目录+文件数量
    int d_count;  // 目录数量
    int f_count;  // 文件数量
    XBOX_File** dp;
    struct XBOX_Dir* parent;  // 父目录
    int is_last;
} XBOX_Dir;

/**
 * @brief 打开一个目录并读取其中所有的文件和目录
 *
 * @param path 路径名
 * @param flag XBOX_DIR_IGNORE_HIDDEN: 不包含.开头的 XBOX_DIR_IGNORE_CURRENT: 不包含.和.. XBOX_DIR_ALL: 全部包含
 * @return XBOX_Dir* (需要释放)
 */
XBOX_Dir* XBOX_opendir(const char* path, int flag);

/**
 * @brief 释放目录分配的内存
 * 
 * @param dir 
 */
void XBOX_freedir(XBOX_Dir* directory);

/**
 * @brief 连接路径, 可变参数, 最后一个参数传 NULL
 *
 * @param path
 * @param ...
 * @return char*
 */
char* XBOX_path_join(const char* path, ...);

/**
 * @brief 获取路径的最后一个目录
 *
 * @param path
 * @return const char*
 */
const char* XBOX_get_last_path(const char* path);

/**
 * @brief 返回文件的[读/写/执行]权限的字符串
 *
 * @param mode
 * @return char*
 */
char* XBOX_stat_access_mode(mode_t mode);

/**
 * @brief 获取一个数字的长度
 * 
 * @param number 
 * @return int 
 */
int XBOX_number_length(long long number);


/**
 * @brief 判断文件是否存在
 * 
 * @param path 
 * @return int 
 */
int XBOX_is_file_exist(const char* path);