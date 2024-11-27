#include "client.h"

int main() {
    HANDLE ljlThread; // Windows 线程句柄
    struct sockaddr_in ljlServAddr;
    char ljlBuf[BUFFSIZE], ljlTemp[BUFFSIZE];

    // 初始化 Winsock 库
    WSADATA ljlWsaData;
    if (WSAStartup(MAKEWORD(2, 2), &ljlWsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // 初始化服务端地址结构
    ZeroMemory(&ljlServAddr, sizeof(ljlServAddr));
    ljlServAddr.sin_family = AF_INET;
    ljlServAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, HOST_IP, &ljlServAddr.sin_addr);

    // 创建客户端套接字
    ljlSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (ljlSockfd == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    // 与服务器建立连接
    printf("connecting... \n");
    if (connect(ljlSockfd, (struct sockaddr *)&ljlServAddr, sizeof(ljlServAddr)) == SOCKET_ERROR) {
        printf("Connection failed\n");
        closesocket(ljlSockfd);
        WSACleanup();
        return 1;
    }

    printf("Enter \"login\" to login\n");
    printf("Enter \"register\" to create an account\n");
    printf("Enter \"quit\" to quit\n");
    printf("Enter \"help\" to get more help\n\n");

    /* === 从此处开始 程序分做两个线程 === */
    // 创建发送消息的线程
    ljlThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ljlSnd, NULL, 0, NULL);
    if (ljlThread == NULL) {
        printf("Thread creation failed\n");
        closesocket(ljlSockfd);
        WSACleanup();
        return 1;
    }

    // 接收消息的线程
    while (1) {
        int ljlLen;
        if ((ljlLen = recv(ljlSockfd, ljlBuf, BUFFSIZE, 0)) > 0) { // recv 读取通信套接字
            ljlBuf[ljlLen] = '\0'; // 添加字符串结束符
            printf("%s\n", ljlBuf);
        }
    }

    closesocket(ljlSockfd);
    WSACleanup();
    return 0;
}

/*发送消息的函数*/
void ljlSnd() {
    char ljlBuf[BUFFSIZE];
    while (1) {
        fgets(ljlBuf, BUFFSIZE, stdin);
        if (strcmp(ljlBuf, "help\n") == 0) {
            ljlGetHelp();
            continue;
        }
        if (strcmp(ljlBuf, "\n") != 0)
            send(ljlSockfd, ljlBuf, strlen(ljlBuf), 0);
        if (strcmp(ljlBuf, "quit\n") == 0) {
            closesocket(ljlSockfd); // 关闭套接字
            WSACleanup();
            exit(0);
        }
    }
}

/*获取帮助信息*/
void ljlGetHelp() {
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
