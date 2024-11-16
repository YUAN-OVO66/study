// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345

WSADATA wsaData;
SOCKET clientSocket;
struct sockaddr_in serverAddr;

void initializeWinsock() {
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("初始化 Winsock 失败。\n");
        exit(1);
    }
}

void connectToServer() {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        printf("创建客户端套接字失败。\n");
        exit(1);
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(SERVER_PORT);

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("连接服务器失败。\n");
        exit(1);
    }
    printf("已连接到服务器。\n");
}

void gameLoop() {
    char message[256];
    while (1) {
        int bytesReceived = recv(clientSocket, message, sizeof(message), 0);
        if (bytesReceived > 0) {
            message[bytesReceived] = '\0';
            printf("%s", message);

            // 玩家输入操作（如：出 2 张 A）
            printf("请输入您的操作（如：出 2 张 A）：");
            fgets(message, sizeof(message), stdin);
            send(clientSocket, message, strlen(message), 0);
        }
    }
}

void cleanup() {
    closesocket(clientSocket);
    WSACleanup();
}

int main() {
    initializeWinsock();
    connectToServer();
    gameLoop();
    cleanup();
    return 0;
}
