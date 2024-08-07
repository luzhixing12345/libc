#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 1024

// 缓冲区和标志
char buffer[BUFFER_SIZE];
volatile sig_atomic_t data_ready = 0;

// 信号处理函数
void handle_sigio(int sig) {
    printf("Received signal %d\n", sig);
    data_ready = 1;
}

int main() {
    struct sigaction sa;
    int flags;

    // 设置信号处理函数
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigio;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGIO, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // 将标准输入设置为异步 I/O
    flags = fcntl(STDIN_FILENO, F_GETFL);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        exit(EXIT_FAILURE);
    }
    if (fcntl(STDIN_FILENO, F_SETFL, flags | FASYNC) == -1) {
        perror("fcntl F_SETFL");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for input (SIGIO signal will be used)...\n");

    // 主循环,等待信号处理
    while (1) {
        if (data_ready) {
            ssize_t num_bytes = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
            if (num_bytes == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            buffer[num_bytes] = '\0';  // 确保缓冲区以空字符结尾
            printf("Received data: %s", buffer);
            data_ready = 0;  // 重置标志
        }
        // printf("continuing...\n");
        sleep(1);  // 等待信号的间隔时间
    }

    return 0;
}
