#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <time.h>
#include <pthread.h>

#pragma comment(lib, "Ws2_32.lib") // 链接 Winsock 库

/* 存储用户 */
struct LjLUser
{
    char ljlUsername[20];
    char ljlPassword[20];
};

/* 存储用户及其用户套接字文件描述符 */
struct LjLUserSocket
{
    char ljlUsername[20];
    SOCKET ljlSocketfd;
    int ljlStatus; // 标识是否在线 0:在线 -1:下线
};

/* 存储聊天室 */
struct LjLChatroom
{
    char ljlName[20];
    char ljlPasswd[20];
    int ljlUser[10]; // 加入聊天室的人数
    int ljlStatus;   // 标识是否还存在 0:存在 -1:销毁
};

#define ljlPort 8888
#define ljlMaxMem 20
#define ljlMaxRoom 5
#define ljlBuffSize 256

int ljlUserCount;     // 记录总的用户数
int ljlChatroomCount; // 记录聊天室个数
SOCKET ljlListenfd, ljlConnfd[ljlMaxMem];
struct LjLUser ljlUsers[ljlMaxMem];               // 记录所有用户
struct LjLUserSocket ljlOnlineUsers[ljlMaxMem];   // 记录在线用户
struct LjLChatroom ljlChatrooms[ljlMaxRoom];      // 记录聊天室

void ljlInit();
void ljlQuit();
void ljlSaveUsers();
void ljlRegisterUser(int ljlN);
void ljlRcvSnd(int ljlP);
void ljlQuitClient(int ljlN);
int ljlUserLogin(int ljlN);
void ljlGetHelp();
void ljlSendPrivateMsg(char *ljlUsername, char *ljlData, SOCKET ljlSfd);
void ljlSendAllMsg(char *ljlMsg, SOCKET ljlSfd);
void ljlGetOnlineUsers(SOCKET ljlSfd);
void ljlSendChatroomMsg(char *ljlMsg, SOCKET ljlSfd);
void ljlCreateChatroom(char *ljlName, char *ljlPasswd, SOCKET ljlSfd);
void ljlJoinChatroom(char *ljlName, char *ljlPasswd, SOCKET ljlSfd);
void ljlGetOnlineChatrooms(SOCKET ljlSfd);
void ljlChangePasswd(SOCKET ljlSfd, char *ljlPasswd);
void ljlGetInroomUsers(SOCKET ljlSfd);
void ljlExitChatroom(SOCKET ljlSfd);
void ljlInvalidCommand(SOCKET ljlSfd);
