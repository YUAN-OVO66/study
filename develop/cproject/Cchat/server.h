#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*存储用户*/
struct user
{
    char username[20];
    char password[20];
};

/*存储用户及其用户套接字文件描述符*/
struct user_socket
{
    char username[20];
    int socketfd;
    int status; //标识是否在线 0:在线 -1:下线
};

/*存储聊天室*/
struct chatroom
{
    char name[20];
    char passwd[20];
    int user[10]; //加入聊天室的人数
    int status;   //标识是否还存在 0:存在 -1:销毁
};

#define PORT 8888
#define MAXMEM 20
#define MAXROOM 5
#define BUFFSIZE 256

int user_count;     //记录总的用户数
int chatroom_count; //记录聊天室个数
int listenfd, connfd[MAXMEM];
struct user users[MAXMEM];               //记录所有用户
struct user_socket online_users[MAXMEM]; //记录在线用户
struct chatroom chatrooms[MAXROOM];      //记录聊天室

void init();
void quit();
void save_users();
void register_user(int n);
void rcv_snd(int p);
void quit_client(int n);
int user_login(int n);
void get_help();
void send_private_msg(char *username, char *data, int sfd);
void send_all_msg(char *msg, int sfd);
void get_online_users(int sfd);
void send_chatroom_msg(char *msg, int sfd);
void create_chatroom(char *name, char *passwd, int sfd);
void join_chatroom(char *name, char *passwd, int sfd);
void get_online_chatrooms(int sfd);
void change_passwd(int sfd, char *passwd);
void get_inroom_users(int sfd);
void exit_chatroom(int sfd);
void invalid_command(int sfd);

