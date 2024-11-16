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
        printf("��ʼ�� Winsock ʧ�ܡ�\n");
        exit(1);
    }
}

void connectToServer() {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        printf("�����ͻ����׽���ʧ�ܡ�\n");
        exit(1);
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(SERVER_PORT);

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("���ӷ�����ʧ�ܡ�\n");
        exit(1);
    }
    printf("�����ӵ���������\n");
}

void gameLoop() {
    char message[256];
    while (1) {
        int bytesReceived = recv(clientSocket, message, sizeof(message), 0);
        if (bytesReceived > 0) {
            message[bytesReceived] = '\0';
            printf("%s", message);

            // �������������磺�� 2 �� A��
            printf("���������Ĳ������磺�� 2 �� A����");
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
