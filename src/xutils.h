
#ifndef XBOX_XUTILS_H
#define XBOX_XUTILS_H

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

#define XBOX_ANSI_COLOR_RED "\033[1;91m"
#define XBOX_ANSI_COLOR_GREEN "\033[1;92m"
#define XBOX_ANSI_COLOR_YELLOW "\033[1;93m"
#define XBOX_ANSI_COLOR_BLUE "\033[1;94m"
#define XBOX_ANSI_COLOR_MAGENTA "\033[1;95m"
#define XBOX_ANSI_COLOR_CYAN "\033[1;96m"
#define XBOX_ANSI_COLOR_RESET "\033[1;0m"

#define XBOX_DIR_IGNORE_HIDDEN 0
#define XBOX_DIR_IGNORE_CURRENT 1
#define XBOX_DIR_ALL 2

#define XBOX_PRINT_BUFFER_SIZE 1024
#define XBOX_MAX_INPUT_SIZE 1024

#define XBOX_IS_DIR(dp) (dp->type == DT_DIR)

typedef struct XBOX_File {
    unsigned char type;  // 文件类型
    char name[NAME_MAX];      // 文件名
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
XBOX_Dir* XBOX_open_dir(const char* path, int flag) {
    DIR* dir;
    struct dirent* entry;

    int length;
    dir = opendir(path);
    if (dir == NULL) {
        // 12 是 "open failed " 的长度
        static char error_info[PATH_MAX+12];
        memset(error_info, 0, PATH_MAX+12);
        sprintf(error_info, "open failed %s", path);
        perror(error_info);
        exit(1);
    }
    
    XBOX_Dir* directory = (XBOX_Dir*)malloc(sizeof(XBOX_Dir));
    memset(directory, 0, sizeof(XBOX_Dir));
    while ((entry = readdir(dir)) != NULL) {
        if (flag == 0 && entry->d_name[0] == '.') {
            continue;
        } else if (flag == 1 && (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))) {
            continue;
        } else {
            // flag = 2
            directory->count++;
            if (entry->d_type == DT_DIR) {
                // 目录
                directory->d_count++;
            } else {
                directory->f_count++;
            }
        }
    }
    rewinddir(dir);
    directory->dp = (XBOX_File**)malloc(sizeof(XBOX_File*) * directory->count);
    for (int i = 0; i < directory->count; i++) {
        directory->dp[i] = (XBOX_File*)malloc(sizeof(XBOX_File));
    }
    int i = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (flag == 0 && entry->d_name[0] == '.') {
            continue;
        } else if (flag == 1 && (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))) {
            continue;
        } else {
            length = strlen(entry->d_name);
            strncpy(directory->dp[i]->name, entry->d_name, length);
            directory->dp[i]->name[length] = 0;
            directory->dp[i]->type = entry->d_type;
            i++;
        }
    }
    length = strlen(path);
    strncpy(directory->name, path, length);
    closedir(dir);
    return directory;
}

void XBOX_free_directory(XBOX_Dir* directory) {
    directory->parent = NULL;
    for (int i = 0; i < directory->count; i++) {
        free(directory->dp[i]);
    }
    free(directory->dp);
    free(directory);
}

/**
 * @brief 连接路径, 可变参数, 最后一个参数传 NULL
 *
 * @param path
 * @param ...
 * @return char*
 */
char* XBOX_path_join(const char* path, ...) {
    static char result[PATH_MAX];
    memset(result, 0, PATH_MAX);
    va_list args;
    char* arg;
    size_t len = strlen(path);
    strncpy(result, path, len);
    va_start(args, path);

    while ((arg = va_arg(args, char*))) {
        size_t arg_len = strlen(arg);
        if (len + arg_len + 1 > PATH_MAX) {
            fprintf(stderr, "Error: maximum path length exceeded\n");
            return NULL;
        }
        strcat(result, "/");
        len += arg_len + 1;
        strncat(result, arg, arg_len);
    }
    va_end(args);
    return result;
}

/**
 * @brief 获取路径的最后一个目录
 *
 * @param path
 * @return const char*
 */
const char* XBOX_get_last_path(const char* path) {
    static char result[256];
    const char* p = strrchr(path, '/');
    if (p == NULL) {
        // 如果路径中没有斜杠,则返回整个路径
        return path;
    } else {
        // 如果路径中有斜杠,则返回最后一个斜杠后面的部分
        const char* p2 = strrchr(path, '/') + 1;
        if (*p2 == '\0') {
            // 如果最后一个斜杠后面没有内容,则返回前一个斜杠后面的部分
            const char* p3 = strrchr(path, '/') - 1;
            int len = p3 - path + 1;
            strncpy(result, path, len);
            result[len] = '\0';
            return result;
        } else {
            // 如果最后一个斜杠后面有内容,则返回最后一个斜杠后面的部分
            return p2;
        }
    }
}
int is_image(const char* name) {
    const char* ext = strrchr(name, '.');
    if (ext == NULL) {
        return 0;
    }
    if (strcasecmp(ext, ".jpg") == 0 || strcasecmp(ext, ".jpeg") == 0 || strcasecmp(ext, ".png") == 0 ||
        strcasecmp(ext, ".bmp") == 0 || strcasecmp(ext, ".svg") == 0 || strcasecmp(ext, ".gif") == 0) {
        return 1;
    }
    return 0;
}

int is_archive(const char* path) {
    const char* ext = strrchr(path, '.');  // 获取文件名中的扩展名
    if (ext != NULL) {
        if (strcasecmp(ext, ".zip") == 0 || strcasecmp(ext, ".rar") == 0 || strcasecmp(ext, ".tar") == 0 ||
            strcasecmp(ext, ".gz") == 0 || strcasecmp(ext, ".bz2") == 0) {
            // 如果扩展名匹配,则认为是压缩包
            return 1;
        }
    }
    // 如果扩展名不匹配,则认为不是压缩包
    return 0;
}

/**
 * @brief 使用 ASNI 虚拟控制序列终端彩色打印
 *
 * @param word 打印的字
 * @param full_path 全路径
 */
char* XBOX_file_print(const char* word, const char* full_path) {
    char* color_code = NULL;
    struct stat file_stat;
    static char result[XBOX_PRINT_BUFFER_SIZE];
    if (!isatty(1)) {
        sprintf(result, "%s", word);
        return result;
    }
    if (stat(full_path, &file_stat) == -1) {
        // error occurred while getting file status
        color_code = XBOX_ANSI_COLOR_RESET;  // set the color to default
        sprintf(result, "%s%s%s", color_code, word, XBOX_ANSI_COLOR_RESET);
        return result;
    }
    if (S_ISREG(file_stat.st_mode)) {
        // regular file
        if (file_stat.st_mode & S_IXUSR || file_stat.st_mode & S_IXGRP || file_stat.st_mode & S_IXOTH) {
            // file has execute permission
            color_code = XBOX_ANSI_COLOR_GREEN;  // set the color to green
        } else if (is_image(full_path)) {
            // image file
            color_code = XBOX_ANSI_COLOR_MAGENTA;  // set the color to magenta
        } else if (is_archive(full_path)) {
            color_code = XBOX_ANSI_COLOR_RED;  // set the color to red
        } else {
            color_code = XBOX_ANSI_COLOR_RESET;  // set the color to default
        }
    } else if (S_ISDIR(file_stat.st_mode)) {
        // directory
        color_code = XBOX_ANSI_COLOR_BLUE;
    } else if (S_ISLNK(file_stat.st_mode)) {
        // symbolic link
        color_code = XBOX_ANSI_COLOR_CYAN;
    } else if (S_ISFIFO(file_stat.st_mode)) {
        // named pipe (FIFO)
        color_code = XBOX_ANSI_COLOR_MAGENTA;
    } else if (S_ISSOCK(file_stat.st_mode)) {
        // local-domain socket
        color_code = XBOX_ANSI_COLOR_YELLOW;
    } else {
        // unknown file type
        color_code = XBOX_ANSI_COLOR_RESET;
    }
    sprintf(result, "%s%s%s", color_code, word, XBOX_ANSI_COLOR_RESET);
    return result;
}

/**
 * @brief 返回文件的[读/写/执行]权限的字符串
 *
 * @param mode
 * @return char*
 */
char* XBOX_stat_access_mode(mode_t mode) {
    static char buf[12];
    strcpy(buf, "-rwxrwxrwx");
    if (S_ISREG(mode)) {
        buf[0] = '-';
    } else if (S_ISDIR(mode)) {
        buf[0] = 'd';
    } else if (S_ISCHR(mode)) {
        buf[0] = 'c';
    } else if (S_ISBLK(mode)) {
        buf[0] = 'b';
    } else if (S_ISFIFO(mode)) {
        buf[0] = 'p';
    } else if (S_ISLNK(mode)) {
        buf[0] = 'l';
        return buf;
    } else if (S_ISSOCK(mode)) {
        buf[0] = 's';
    } else {
        buf[0] = '-';
        // UNKNOWN type?
    }
    mode_t umask_val = umask(0);                // 获取当前 umask 值
    umask(umask_val);                           // 恢复原来的 umask 值
    mode_t effective_mode = mode & ~umask_val;  // 计算生效的权限值
    buf[1] = (effective_mode & S_IRUSR) ? 'r' : '-';
    buf[2] = (effective_mode & S_IWUSR) ? 'w' : '-';
    buf[3] = (effective_mode & S_IXUSR) ? 'x' : '-';
    buf[4] = (effective_mode & S_IRGRP) ? 'r' : '-';
    buf[5] = (effective_mode & S_IWGRP) ? 'w' : '-';
    buf[6] = (effective_mode & S_IXGRP) ? 'x' : '-';
    buf[7] = (effective_mode & S_IROTH) ? 'r' : '-';
    buf[8] = (effective_mode & S_IWOTH) ? 'w' : '-';
    buf[9] = (effective_mode & S_IXOTH) ? 'x' : '-';
    buf[10] = '\0';
    return buf;
}

int XBOX_number_length(int number) {
    int size_length = 0;
    while (number) {
        size_length++;
        number /= 10;
    }
    return size_length;
}

#endif  // XBOX_XUTILS_H
