
#pragma once

#include <stdlib.h>
#include <string.h>
/**
 * @brief 分割字符串, 返回数组.
 *
 * @param str 待分割的字符串
 * @param c 分割所使用的字符
 * @param result 返回数组 (call XBOX_freeSplitStr to free)
 * @param length 数组长度
 */
void XBOX_splitStr(char *str, char c, char ***result, int *length) {
    int n = strlen(str);
    int number = 1;
    int i;
    for (i = 0; i < n; i++) {
        number += c == str[i];
    }
    *length = number;
    *result = (char **)malloc(sizeof(char *) * number);

    char split_str[] = {c};
    char *token;
    token = strtok(str, split_str);
    i = 0;
    while (token != NULL) {
        (*result)[i] = (char *)malloc(sizeof(char) * (strlen(token) + 1));
        strcpy((*result)[i], token);
        token = strtok(NULL, split_str);
        i++;
    }
}

/**
 * @brief free XBOX_splitStr
 *
 * @param result
 * @param length
 * @return int
 */
int XBOX_freeSplitStr(char ***result, int length) {
    for (int i = 0; i < length; i++) {
        free((*result)[i]);
    }
    free(*result);
    *result = NULL;
    return 0;
}
