// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 12345
#define MAX_PLAYERS 2  // 设置连接人数为2

WSADATA wsaData;
SOCKET serverSocket, clientSocket[MAX_PLAYERS];
struct sockaddr_in serverAddr, clientAddr[MAX_PLAYERS];
int playerCount = 0;
char* playerNames[MAX_PLAYERS];
int playerCards[MAX_PLAYERS][4];  // 每个玩家最多4张牌（A, K, Q, 王）
int currentPlayer = 0;  // 当前出牌玩家

// 模拟牌面类型
enum CardType {
    ACE = 0, KING = 1, QUEEN = 2, JOKER = 3
};

void initializeWinsock() {
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("初始化 Winsock 失败。\n");
        exit(1);
    }
}

void setupServer() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("创建服务器套接字失败。\n");
        exit(1);
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("绑定失败。\n");
        exit(1);
    }
    if (listen(serverSocket, MAX_PLAYERS) == SOCKET_ERROR) {
        printf("监听失败。\n");
        exit(1);
    }
}

void acceptConnections() {
    int addrLen = sizeof(clientAddr[0]);
    while (playerCount < MAX_PLAYERS) {
        clientSocket[playerCount] = accept(serverSocket, (struct sockaddr *)&clientAddr[playerCount], &addrLen);
        if (clientSocket[playerCount] == INVALID_SOCKET) {
            printf("接收客户端连接失败。\n");
            continue;
        }
        printf("玩家 %d 已连接。\n", playerCount + 1);
        send(clientSocket[playerCount], "欢迎进入游戏，请输入您的名字：", 32, 0);
        char name[64];
        int bytesReceived = recv(clientSocket[playerCount], name, sizeof(name), 0);
        name[bytesReceived] = '\0';
        playerNames[playerCount] = strdup(name);  // 存储玩家名字
        printf("玩家 %d 的名字是：%s\n", playerCount + 1, playerNames[playerCount]);
        playerCount++;
    }
}

void shuffleAndDealCards() {
    // 简单的模拟发牌，只是用来表示每个玩家有4张牌（A, K, Q, 王）
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
    shuffleAndDealCards();  // 发牌

    while (1) {
        // 当前出牌玩家
        sprintf(message, "轮到玩家 %s 出牌。\n", playerNames[currentPlayer]);
        sendMessageToAllClients(message);

        // 给当前玩家出牌要求
        sprintf(message, "请输入出牌类型和数量（例如：出 2 张 A）：\n");
        send(clientSocket[currentPlayer], message, strlen(message), 0);

        // 接收玩家输入
        char playerInput[256];
        int bytesReceived = recv(clientSocket[currentPlayer], playerInput, sizeof(playerInput), 0);
        playerInput[bytesReceived] = '\0';
        printf("玩家 %s 选择：%s\n", playerNames[currentPlayer], playerInput);

        // 游戏逻辑：这里简化为玩家是否相信
        char action[32];
        int count;
        if (sscanf(playerInput, "%s %d", action, &count) == 2) {
            if (strcmp(action, "出") == 0) {
                sprintf(message, "%s 宣布出 %d 张牌。\n", playerNames[currentPlayer], count);
                sendMessageToAllClients(message);

                // 判断玩家是否出牌合法，这里简化为每个玩家只能按顺序出牌
                if (count > 0 && count <= 3) {
                    currentPlayer = (currentPlayer + 1) % MAX_PLAYERS;  // 切换到下一个玩家
                } else {
                    sprintf(message, "出牌数量非法，轮到下一个玩家出牌。\n");
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
    printf("服务器已启动，等待玩家连接...\n");
    acceptConnections();
    gameLoop();
    cleanup();
    return 0;
}
