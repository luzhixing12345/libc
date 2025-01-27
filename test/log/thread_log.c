#include <stdio.h>
#include <pthread.h>

#include <clib/clib.h>

#define THREAD_NUM 1000

void *thread_func(void *arg) {
    INFO("hello world from thread %d", *(int *)arg);
    return NULL;
}

// 示例使用
int main() {
    
    // 创建多个线程
    pthread_t threads[THREAD_NUM];
    int thread_ids[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // 等待所有线程完成
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(threads[i], NULL);
    }
    INFO("all threads completed\n");

    return 0;
}