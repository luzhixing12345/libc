
#include "xutils.h"

#include <linux/limits.h>
#include <stdio.h>
#include <sys/stat.h>

/**
 * @brief 打开一个目录并读取该目录下所有的文件和目录
 *
 * @param path 路径名
 * @param flag
    - XBOX_DIR_IGNORE_HIDDEN: 不包含.开头的
    - XBOX_DIR_IGNORE_CURRENT: 不包含.和..
    - XBOX_DIR_ALL: 全部包含
 * @return XBOX_Dir* 需要调用 XBOX_freedir 释放
 */
XBOX_Dir* XBOX_opendir(const char* path, int flag) {
    DIR* dir;
    struct dirent* entry;

    int length;
    dir = opendir(path);
    if (dir == NULL) {
        // 12 是 "open failed " 的长度
        static char error_info[PATH_MAX + 12];
        memset(error_info, 0, PATH_MAX + 12);
        sprintf(error_info, "open failed %s", path);
        perror(error_info);
        exit(1);
    }

    XBOX_Dir* directory = (XBOX_Dir*)malloc(sizeof(XBOX_Dir));
    memset(directory, 0, sizeof(XBOX_Dir));
    while ((entry = readdir(dir)) != NULL) {
        if ((flag & XBOX_DIR_IGNORE_HIDDEN) && entry->d_name[0] == '.') {
            continue;
        }
        if ((flag & XBOX_DIR_IGNORE_CURRENT) &&
            (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))) {
            continue;
        }
        directory->count++;
        if (entry->d_type == DT_DIR) {
            // 目录
            directory->d_count++;
        } else {
            directory->f_count++;
        }
    }
    rewinddir(dir);
    directory->dp = (XBOX_File**)malloc(sizeof(XBOX_File*) * directory->count);
    for (int i = 0; i < directory->count; i++) {
        directory->dp[i] = (XBOX_File*)malloc(sizeof(XBOX_File));
    }
    int i = 0;
    while ((entry = readdir(dir)) != NULL) {
        if ((flag & XBOX_DIR_IGNORE_HIDDEN) && entry->d_name[0] == '.') {
            continue;
        }
        if ((flag & XBOX_DIR_IGNORE_CURRENT) &&
            (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))) {
            continue;
        }
        length = strlen(entry->d_name);
        strncpy(directory->dp[i]->name, entry->d_name, length);
        directory->dp[i]->name[length] = 0;
        directory->dp[i]->type = entry->d_type;
        i++;
    }
    length = strlen(path);
    strncpy(directory->name, path, length);
    closedir(dir);
    return directory;
}

/**
 * @brief 释放目录分配的内存
 *
 * @param dir
 */
void XBOX_freedir(XBOX_Dir* directory) {
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
    static char result[PATH_MAX];
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

/**
 * @brief 返回文件的[读/写/执行]权限的字符串
 *
 * @param mode
 * @return char*
 */
char* XBOX_stat_access_mode(mode_t mode) {
    static char buf[11];
    memset(buf, 0, 11);
    buf[1] = (mode & S_IRUSR) ? 'r' : '-';
    buf[2] = (mode & S_IWUSR) ? 'w' : '-';
    buf[3] = (mode & S_IXUSR) ? 'x' : '-';
    buf[4] = (mode & S_IRGRP) ? 'r' : '-';
    buf[5] = (mode & S_IWGRP) ? 'w' : '-';
    buf[6] = (mode & S_IXGRP) ? 'x' : '-';
    buf[7] = (mode & S_IROTH) ? 'r' : '-';
    buf[8] = (mode & S_IWOTH) ? 'w' : '-';
    buf[9] = (mode & S_IXOTH) ? 'x' : '-';
    buf[10] = '\0';
    if (S_ISREG(mode)) {
        buf[0] = '-';
        if (mode & S_ISUID) {
            buf[3] = 'S';
        } else if (mode & S_ISGID) {
            buf[6] = 'S';
        }
    } else if (S_ISDIR(mode)) {
        buf[0] = 'd';
        if (mode & S_ISVTX) {
            buf[9] = 't';
        }
    } else if (S_ISCHR(mode)) {
        buf[0] = 'c';
    } else if (S_ISBLK(mode)) {
        buf[0] = 'b';
    } else if (S_ISFIFO(mode)) {
        buf[0] = 'p';
    } else if (S_ISLNK(mode)) {
        return (char*)"lrwxrwxrwx";
    } else if (S_ISSOCK(mode)) {
        buf[0] = 's';
    } else {
        buf[0] = '-';
        // UNKNOWN type?
    }
    // mode_t umask_val = umask(0);                // 获取当前 umask 值
    // umask(umask_val);                           // 恢复原来的 umask 值
    // mode_t effective_mode = mode & ~umask_val;  // 计算生效的权限值

    return buf;
}

/**
 * @brief 获取一个数字的长度
 *
 * @param number
 * @return int
 */
int XBOX_number_length(long long number) {
    int size_length = 0;
    while (number) {
        size_length++;
        number /= 10;
    }
    return size_length;
}

int XBOX_is_file_exist(const char *path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}