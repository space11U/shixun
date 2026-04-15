/*===============================================
*   文件名称：client.c
*   创 建 者：     
*   创建日期：2026年01月09日
*   描    述：gcc -o client client.c
================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_PORT 8000    // 服务器端口
#define SERVER_IP "127.0.0.1" // 服务器IP（本机测试）
#define BUF_SIZE 1024       // 缓冲区大小

int main(int argc, char const *argv[]) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char send_buf[BUF_SIZE] = {0};  // 发送消息缓冲区
    char recv_buf[BUF_SIZE] = {0};  // 接收消息缓冲区

    // 1. 创建客户端套接字
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket 创建失败\n");
        return -1;
    }

    // 2. 配置服务器地址
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    // IP地址格式转换
    if(inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        printf("IP地址格式错误\n");
        return -1;
    }

    // 3. 连接服务器
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("连接服务器失败！\n");
        return -1;
    }
    printf("成功连接服务器 %s:%d\n", SERVER_IP, SERVER_PORT);
    printf("========================================\n");
    printf("请输入消息发送（输入 quit 退出）：\n");

    // 4. 通信循环（核心实现）
    while(1) {
        // 清空缓冲区
        memset(send_buf, 0, BUF_SIZE);
        memset(recv_buf, 0, BUF_SIZE);

        // 从控制台读取用户输入
        fgets(send_buf, BUF_SIZE, stdin);

        // 去除输入的换行符
        send_buf[strcspn(send_buf, "\n")] = '\0';

        // 判断是否退出
        if (strcmp(send_buf, "quit") == 0) {
            printf("客户端主动退出连接\n");
            break;
        }

        // 发送消息到服务器
        send(sock, send_buf, strlen(send_buf), 0);
        printf("→ 已发送：%s\n", send_buf);

        // 接收服务器回复
        int ret = read(sock, recv_buf, BUF_SIZE);
        if (ret <= 0) {
            printf("服务器已断开连接\n");
            break;
        }

        // 打印服务器回复
        printf("← 服务端回复：%s\n", recv_buf);
    }

    // 5. 关闭套接字
    close(sock);
    return 0;
}
