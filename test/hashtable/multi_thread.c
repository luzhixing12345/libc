#include <assert.h>
#include <clib/clib.h>  // 假设你的线程安全哈希表实现使用了这个头文件
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS    10
#define NUM_OPERATIONS 1000

HashTable table;

void* thread_operations(void* arg) {
    int thread_id = *((int*)arg);
    for (int i = 0; i < NUM_OPERATIONS; ++i) {
        int key = (thread_id + 1) * NUM_OPERATIONS + i;
        int value = key * 2;  // Just an arbitrary value for testing

        // 插入操作
        ht_insert(&table, &key, &value);

        // 查找操作
        int* found_value = (int*)ht_lookup(&table, &key);
        assert(found_value && *found_value == value);

        // 删除操作
        ht_erase(&table, &key);
    }
    return NULL;
}

void test_multithreaded_operations() {
    ht_setup(&table, sizeof(int), sizeof(int), 10);

    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // 创建线程
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_operations, &thread_ids[i]);
    }

    // 等待线程完成
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    ht_destroy(&table);
    printf("test_multithreaded_operations PASSED\n");
}

int main() {
    test_multithreaded_operations();
    return 0;
}
