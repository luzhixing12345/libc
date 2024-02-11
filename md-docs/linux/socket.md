
# socket

```c
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int listen(int sockfd, int backlog);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

socket 用于创建一个套接字,它是通信的端点

- **domain**: 指定协议族,常用的有 `AF_INET`(IPv4)和 `AF_INET6`(IPv6).
- **type**: 指定套接字类型,常用的有 `SOCK_STREAM` (流套接字,用于TCP)和 `SOCK_DGRAM` (数据报套接字,用于UDP).
- **protocol**: 指定协议,通常为 0,表示选择默认协议.

bind 用于绑定一个地址到套接字

- **sockfd**: 套接字描述符,由socket函数返回.
- **addr**: 指定要绑定的地址信息,通常是一个struct sockaddr结构体,可以是struct sockaddr_in(IPv4)或struct sockaddr_in6(IPv6).
- **addrlen**: 指定地址结构体的长度.

listen 用于监听套接字,使其能够接受连接请求

- **sockfd**: 套接字描述符,由socket函数返回.
- **backlog**: 指定连接队列的最大长度,即在服务器还未调用accept函数接受连接之前,能够排队等待连接的最大数量

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_PENDING_CONNECTIONS 5

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // 创建套接字
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址结构
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 绑定套接字
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 监听套接字
    if (listen(server_socket, MAX_PENDING_CONNECTIONS) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // 接受连接并处理
    while (1) {
        // 等待客户端连接
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // 处理连接,这里简单打印客户端的信息
        printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // 在这里可以进行进一步的通信处理,读取/写入数据等

        // 关闭客户端套接字
        close(client_socket);
    }

    // 关闭服务器套接字
    close(server_socket);

    return 0;
}

```

```c
#include <arpa/inet.h>

uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);
```