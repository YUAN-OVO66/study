#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFSIZE 128
#define HOST_IP "192.168.159.3"
#define PORT 8888

#pragma comment(lib, "Ws2_32.lib")

SOCKET sockfd;
void snd();
void get_help();

int main() {
    HANDLE thread; // Windows 线程句柄
    struct sockaddr_in serv_addr;
    char buf[BUFFSIZE], temp[BUFFSIZE];

    // 初始化 Winsock 库
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // 初始化服务端地址结构
    ZeroMemory(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, HOST_IP, &serv_addr.sin_addr);

    // 创建客户端套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    // 与服务器建立连接
    printf("connecting... \n");
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        printf("Connection failed\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    printf("Enter \"login\" to login\n");
    printf("Enter \"register\" to create an account\n");
    printf("Enter \"quit\" to quit\n");
    printf("Enter \"help\" to get more help\n\n");

    /* === 从此处开始 程序分做两个线程 === */
    // 创建发送消息的线程
    thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)snd, NULL, 0, NULL);
    if (thread == NULL) {
        printf("Thread creation failed\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // 接收消息的线程
    while (1) {
        int len;
        if ((len = recv(sockfd, buf, BUFFSIZE, 0)) > 0) { // recv 读取通信套接字
            buf[len] = '\0'; // 添加字符串结束符
            printf("%s\n", buf);
        }
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}

/*发送消息的函数*/
void snd() {
    char buf[BUFFSIZE];
    while (1) {
        fgets(buf, BUFFSIZE, stdin);
        if (strcmp(buf, "help\n") == 0) {
            get_help();
            continue;
        }
        if (strcmp(buf, "\n") != 0)
            send(sockfd, buf, strlen(buf), 0);
        if (strcmp(buf, "quit\n") == 0) {
            closesocket(sockfd); // 关闭套接字
            WSACleanup();
            exit(0);
        }
    }
}

/*获取帮助信息*/
void get_help() {
    printf("Commands introduction:\n");
    printf("\t'ls -users':\t\tShow all online users\n");
    printf("\t'ls -chatrooms':\tShow all chat rooms\n");
    printf("\t'ls -inrmusr'\t\tShow all online users in chat room you joined\n");
    printf("\t'send username msg':\tSend a message to the user named 'username' msg:the content of the message\n");
    printf("\t'join chatroom passwd':\tJoin in a chat room named 'chatroom' with password 'passwd'\n");
    printf("\t'create chatrname passwd':\tCreate a chat room named 'chatrname' with password 'passwd'\n");
    printf("\t'chgpsw passwd':\t\tChange your password to 'passwd'\n");
    printf("\t'send -chatroom msg':\tSend a message to the chat room\n");
    printf("\t'exit':\t\t\tExit the chat room you joined\n");
    printf("\t'send -all msg':\tSend a message to all online users\n");
    printf("\t'login':\t\tLogin chat system\n");
    printf("\t'register':\t\tCreate an account\n");
    printf("\t'quit':\t\t\tExit chat system\n");
    printf("\t'help':\t\t\tGet more help information\n\n");
}

