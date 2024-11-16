// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 12345
#define MAX_PLAYERS 2  // ������������Ϊ2

WSADATA wsaData;
SOCKET serverSocket, clientSocket[MAX_PLAYERS];
struct sockaddr_in serverAddr, clientAddr[MAX_PLAYERS];
int playerCount = 0;
char* playerNames[MAX_PLAYERS];
int playerCards[MAX_PLAYERS][4];  // ÿ��������4���ƣ�A, K, Q, ����
int currentPlayer = 0;  // ��ǰ�������

// ģ����������
enum CardType {
    ACE = 0, KING = 1, QUEEN = 2, JOKER = 3
};

void initializeWinsock() {
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("��ʼ�� Winsock ʧ�ܡ�\n");
        exit(1);
    }
}

void setupServer() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("�����������׽���ʧ�ܡ�\n");
        exit(1);
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("��ʧ�ܡ�\n");
        exit(1);
    }
    if (listen(serverSocket, MAX_PLAYERS) == SOCKET_ERROR) {
        printf("����ʧ�ܡ�\n");
        exit(1);
    }
}

void acceptConnections() {
    int addrLen = sizeof(clientAddr[0]);
    while (playerCount < MAX_PLAYERS) {
        clientSocket[playerCount] = accept(serverSocket, (struct sockaddr *)&clientAddr[playerCount], &addrLen);
        if (clientSocket[playerCount] == INVALID_SOCKET) {
            printf("���տͻ�������ʧ�ܡ�\n");
            continue;
        }
        printf("��� %d �����ӡ�\n", playerCount + 1);
        send(clientSocket[playerCount], "��ӭ������Ϸ���������������֣�", 32, 0);
        char name[64];
        int bytesReceived = recv(clientSocket[playerCount], name, sizeof(name), 0);
        name[bytesReceived] = '\0';
        playerNames[playerCount] = strdup(name);  // �洢�������
        printf("��� %d �������ǣ�%s\n", playerCount + 1, playerNames[playerCount]);
        playerCount++;
    }
}

void shuffleAndDealCards() {
    // �򵥵�ģ�ⷢ�ƣ�ֻ��������ʾÿ�������4���ƣ�A, K, Q, ����
    for (int i = 0; i < MAX_PLAYERS; i++) {
        playerCards[i][0] = ACE;
        playerCards[i][1] = KING;
        playerCards[i][2] = QUEEN;
        playerCards[i][3] = JOKER;
    }
}

void sendMessageToAllClients(const char* message) {
    for (int i = 0; i < playerCount; i++) {
        send(clientSocket[i], message, strlen(message), 0);
    }
}

void gameLoop() {
    char message[256];
    shuffleAndDealCards();  // ����

    while (1) {
        // ��ǰ�������
        sprintf(message, "�ֵ���� %s ���ơ�\n", playerNames[currentPlayer]);
        sendMessageToAllClients(message);

        // ����ǰ��ҳ���Ҫ��
        sprintf(message, "������������ͺ����������磺�� 2 �� A����\n");
        send(clientSocket[currentPlayer], message, strlen(message), 0);

        // �����������
        char playerInput[256];
        int bytesReceived = recv(clientSocket[currentPlayer], playerInput, sizeof(playerInput), 0);
        playerInput[bytesReceived] = '\0';
        printf("��� %s ѡ��%s\n", playerNames[currentPlayer], playerInput);

        // ��Ϸ�߼��������Ϊ����Ƿ�����
        char action[32];
        int count;
        if (sscanf(playerInput, "%s %d", action, &count) == 2) {
            if (strcmp(action, "��") == 0) {
                sprintf(message, "%s ������ %d ���ơ�\n", playerNames[currentPlayer], count);
                sendMessageToAllClients(message);

                // �ж�����Ƿ���ƺϷ��������Ϊÿ�����ֻ�ܰ�˳�����
                if (count > 0 && count <= 3) {
                    currentPlayer = (currentPlayer + 1) % MAX_PLAYERS;  // �л�����һ�����
                } else {
                    sprintf(message, "���������Ƿ����ֵ���һ����ҳ��ơ�\n");
                    sendMessageToAllClients(message);
                    currentPlayer = (currentPlayer + 1) % MAX_PLAYERS;
                }
            }
        }
    }
}

void cleanup() {
    for (int i = 0; i < playerCount; i++) {
        closesocket(clientSocket[i]);
        free(playerNames[i]);
    }
    closesocket(serverSocket);
    WSACleanup();
}

int main() {
    initializeWinsock();
    setupServer();
    printf("���������������ȴ��������...\n");
    acceptConnections();
    gameLoop();
    cleanup();
    return 0;
}
