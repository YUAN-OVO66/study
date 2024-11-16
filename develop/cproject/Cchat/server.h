#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*�洢�û�*/
struct user
{
    char username[20];
    char password[20];
};

/*�洢�û������û��׽����ļ�������*/
struct user_socket
{
    char username[20];
    int socketfd;
    int status; //��ʶ�Ƿ����� 0:���� -1:����
};

/*�洢������*/
struct chatroom
{
    char name[20];
    char passwd[20];
    int user[10]; //���������ҵ�����
    int status;   //��ʶ�Ƿ񻹴��� 0:���� -1:����
};

#define PORT 8888
#define MAXMEM 20
#define MAXROOM 5
#define BUFFSIZE 256

int user_count;     //��¼�ܵ��û���
int chatroom_count; //��¼�����Ҹ���
int listenfd, connfd[MAXMEM];
struct user users[MAXMEM];               //��¼�����û�
struct user_socket online_users[MAXMEM]; //��¼�����û�
struct chatroom chatrooms[MAXROOM];      //��¼������

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

