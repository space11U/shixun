/*===============================================
*   文件名称：server.c
*   创 建 者：     
*   创建日期：2026年01月09日
*   描    述：gcc -o server server.c
================================================*/
// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8000        // 服务器监听端口
#define BUF_SIZE 1024    // 缓冲区大小

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUF_SIZE] = {0};

    // 1. 创建TCP套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    printf("服务器套接字创建成功\n");

    // 2. 设置端口复用
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // 3. 绑定IP和端口
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("绑定端口成功：%d\n", PORT);

    // 4. 开始监听
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("正在监听端口：%d...\n", PORT);

    // 5. 循环接受客户端连接
    while(1) {
        printf("\n等待客户端连接...\n");
        
        // 阻塞等待客户端连接
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                                (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("客户端连接成功！IP: %s, Port: %d\n", 
               inet_ntoa(address.sin_addr), ntohs(address.sin_port));

        // 6. 与客户端循环通信（核心实现部分）
        while(1) {
            // 清空缓冲区
            memset(buffer, 0, BUF_SIZE);
            
            // 读取客户端发送的数据
            int valread = read(new_socket, buffer, BUF_SIZE);
            
            // 读取失败 / 客户端断开连接
            if (valread <= 0) {
                printf("客户端断开连接\n");
                break;
            }

            // 打印收到的消息
            printf("收到客户端消息：%s\n", buffer);

            // 服务端回复消息（回显）
            char reply[BUF_SIZE];
            snprintf(reply, BUF_SIZE, "服务端已收到：%s", buffer);
            send(new_socket, reply, strlen(reply), 0);
            printf("回复客户端：%s\n", reply);
        }

        // 关闭当前连接
        close(new_socket);
    }

    close(server_fd);
    return 0;
}
