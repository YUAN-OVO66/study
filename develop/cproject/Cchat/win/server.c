#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <time.h>
#include <pthread.h>

#pragma comment(lib, "Ws2_32.lib") // ���� Winsock ��

#define PORT 8888
#define MAXMEM 20
#define MAXROOM 5
#define BUFFSIZE 256

int user_count;     // ��¼�ܵ��û���
int chatroom_count; // ��¼�����Ҹ���
SOCKET listenfd, connfd[MAXMEM];
struct user users[MAXMEM];               // ��¼�����û�
struct user_socket online_users[MAXMEM]; // ��¼�����û�
struct chatroom chatrooms[MAXROOM];      // ��¼������

// ��������
void init();
void quit();
void save_users();
void register_user(int n);
void rcv_snd(int p);
void quit_client(int n);
int user_login(int n);
void get_help();
void send_private_msg(char *username, char *data, SOCKET sfd);
void send_all_msg(char *msg, SOCKET sfd);
void get_online_users(SOCKET sfd);
void send_chatroom_msg(char *msg, SOCKET sfd);
void create_chatroom(char *name, char *passwd, SOCKET sfd);
void join_chatroom(char *name, char *passwd, SOCKET sfd);
void get_online_chatrooms(SOCKET sfd);
void change_passwd(SOCKET sfd, char *passwd);
void get_inroom_users(SOCKET sfd);
void exit_chatroom(SOCKET sfd);
void invalid_command(SOCKET sfd);
#include "server.h"

int main()
{
    // Winsock ��ʼ��
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        perror("WSAStartup failed");
        exit(-1);
    }

    init();

    struct sockaddr_in serv_addr, cli_addr;
    int i;
    time_t timenow;
    pthread_t thread;
    char buff[BUFFSIZE];

    printf("Running...\nEnter command \"quit\" to exit server.\n\n");

    // ��շ�������ַ�ṹ
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // ���������׽���
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == INVALID_SOCKET)
    {
        perror("fail to socket");
        WSACleanup();
        exit(-1);
    }

    // ���׽���
    if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
    {
        perror("fail to bind");
        closesocket(listenfd);
        WSACleanup();
        exit(-2);
    }

    // ��ʼ����
    if (listen(listenfd, MAXMEM) == SOCKET_ERROR)
    {
        perror("fail to listen");
        closesocket(listenfd);
        WSACleanup();
        exit(-3);
    }

    // ����һ���̣߳����ڹ�������������� quit ����
    pthread_create(&thread, NULL, (void *)(quit), NULL);

    // ��ʼ�������׽�������
    for (i = 0; i < MAXMEM; i++)
        connfd[i] = INVALID_SOCKET;

    // ���տͻ�������
    while (1)
    {
        int len;
        for (i = 0; i < MAXMEM; i++)
        {
            if (connfd[i] == INVALID_SOCKET)
                break;
        }

        // ��������
        connfd[i] = accept(listenfd, (struct sockaddr *)&cli_addr, &len);
        if (connfd[i] == INVALID_SOCKET)
        {
            perror("fail to accept.");
            continue;
        }

        timenow = time(NULL);
        printf("%.24s\n\tconnect from: %s, port %d\n",
               ctime(&timenow), inet_ntoa(cli_addr.sin_addr),
               ntohs(cli_addr.sin_port));

        // ��Ե�ǰ�׽��ִ���һ���߳���������Ϣ
        pthread_t recv_thread;
        pthread_create(&recv_thread, NULL, (void *)(&rcv_snd), (void *)i);
    }

    // ���� Winsock
    WSACleanup();

    return 0;
}


/*���������պͷ��ͺ���*/
void rcv_snd(int n)
{
    ssize_t len;
    int i;
    char mytime[32], buf[BUFFSIZE];
    char temp[BUFFSIZE];
    char command[20], arg1[20], arg2[BUFFSIZE];
    time_t timenow;

    // ��һ��ѭ�������ڽ��պʹ����¼��ע������
    while (1)
    {
        len = recv(connfd[n], buf, BUFFSIZE);
        if (len > 0)
        {
            buf[len - 1] = '\0'; // ȥ�����з�

            // �����¼����
            if (strcmp(buf, "login") == 0)
            {
                // ��¼�ɹ�ʱ�˳���ѭ��
                if (user_login(n) == 0)
                {
                    break;
                }
            }
                // ����ע������
            else if (strcmp(buf, "register") == 0)
            {
                register_user(n);
            }
                // �����˳�����
            else if (strcmp(buf, "quit") == 0)
            {
                quit_client(n);
            }
        }
    }

    // �ڶ���ѭ�������ڴ����¼�������
    while (1)
    {
        if ((len = recv(connfd[n], temp, BUFFSIZE)) > 0)
        {
            temp[len - 1] = '\0'; // ȥ�����з�
            sscanf(temp, "%s %s %[^\n]", command, arg1, arg2); // ��������

            // ��������Ϣ������
            if (strcmp(command, "send") == 0 && strcmp(arg1, "-all") == 0)
            {
                send_all_msg(arg2, n);  // �����������û�������Ϣ
            }
            else if (strcmp(command, "send") == 0 && strcmp(arg1, "-chatroom") == 0)
            {
                send_chatroom_msg(arg2, n);  // ���������е������û�������Ϣ
            }
            else if (strcmp(command, "send") == 0)
            {
                send_private_msg(arg1, arg2, n);  // ��ָ���û�����˽����Ϣ
            }
                // �����˳�����
            else if (strcmp(command, "quit") == 0)
            {
                quit_client(n);
            }
                // �����޸���������
            else if (strcmp(command, "chgpsw") == 0)
            {
                change_passwd(n, arg1);
            }
                // ����������������
            else if (strcmp(command, "create") == 0)
            {
                create_chatroom(arg1, arg2, n);
            }
                // �����������������
            else if (strcmp(command, "join") == 0)
            {
                join_chatroom(arg1, arg2, n);
            }
                // �����г�����������
            else if (strcmp(command, "ls") == 0 && strcmp(arg1, "-chatrooms") == 0)
            {
                get_online_chatrooms(n);
            }
                // �����г������û�����
            else if (strcmp(command, "ls") == 0 && strcmp(arg1, "-users") == 0)
            {
                get_online_users(n);
            }
                // �����г����������û�����
            else if (strcmp(command, "ls") == 0 && strcmp(arg1, "-inrmusr") == 0)
            {
                get_inroom_users(n);
            }
                // �����˳�����������
            else if (strcmp(command, "exit") == 0)
            {
                exit_chatroom(n);
            }
                // ������Ч����
            else
            {
                invalid_command(n);
            }
        }
    }
}


//��ʼ��
void init()
{
    int i, j;
    user_count = 0;
    chatroom_count = 0;

    // ��ʼ�������û�״̬
    for (i = 0; i < MAXMEM; i++)
    {
        online_users[i].status = -1; // -1 ��ʾ����
    }

    // ��ʼ��������״̬
    for (i = 0; i < MAXROOM; i++)
    {
        chatrooms[i].status = -1; // -1 ��ʾ�����Ҳ�����
        for (j = 0; j < 10; j++)
        {
            chatrooms[i].user[j] = -1; // ��ʼ���������е��û�Ϊ -1�����û���
        }
    }

    // ���ļ������ļ���ȡ�û�����
    char buf[20];
    FILE *fp = NULL;
    fp = fopen("users.txt", "r");
    if (fp == NULL)
    {
        printf("Error opening users.txt file.\n");
        return;
    }

    // ���ļ��ж�ȡ�û��������벢�洢
    while (fscanf(fp, "%s", buf) != EOF)
    {
        strcpy(users[user_count].username, buf);  // ��ȡ�û���
        fscanf(fp, "%s", buf);
        strcpy(users[user_count].password, buf);  // ��ȡ����
        user_count++;  // �����û�����
    }

    // �ر��ļ�
    fclose(fp);
}


/*���û����浽�ļ�*/
void save_users()
{
    int i;
    FILE *fp = NULL;

    // ���ļ��Խ���д����������ļ��������򴴽�
    fp = fopen("users.txt", "w+");
    if (fp == NULL)  // ����ļ��Ƿ�ɹ���
    {
        printf("Error opening users.txt file.\n");
        return;
    }

    // �����û����飬д��ÿ���û����û���������
    for (i = 0; i < user_count; i++)
    {
        // ʹ�� fprintf ֱ�ӽ��û���������д���ļ�
        fprintf(fp, "%s\n", users[i].username);
        fprintf(fp, "%s\n", users[i].password);
    }

    // �ر��ļ�
    fclose(fp);
}

void quit_client(int n)
{
    int i;

    // ȷ���׽���������Ч
    if (connfd[n] != -1)
    {
        closesocket(connfd[n]);  // �ر��׽�������
        connfd[n] = -1;    // ��������״̬����Ǹ�λ��Ϊ��
    }

    // ���������û�״̬
    for (i = 0; i < MAXMEM; i++)
    {
        if (online_users[i].socketfd == connfd[n])
        {
            online_users[i].status = -1;  // ����û�����
            break;  // �ҵ��û������º�����ѭ��
        }
    }

    // �˳��̣߳�������Ҫ�����˳�״̬��ֱ�ӵ���pthread_exit()
    pthread_exit(NULL);
}

//�û���¼
int user_login(int n)
{
    int len, i, j;
    char buf[BUFFSIZE], username[20], password[20];

    // ��ʾ�û������û���
    sprintf(buf, "Your username: ");
    send(connfd[n], buf, strlen(buf));

    // ��ȡ�û���
    len = recv(connfd[n], username, 20);
    if (len > 0)
    {
        username[len - 1] = '\0'; // ȥ�����з�
    }

    // ��ʾ�û���������
    sprintf(buf, "Your password: ");
    send(connfd[n], buf, strlen(buf));

    // ��ȡ����
    len = recv(connfd[n], password, 20);
    if (len > 0)
    {
        password[len - 1] = '\0'; // ȥ�����з�
    }

    // �����û�������֤����
    for (i = 0; i < MAXMEM; i++)
    {
        if (strcmp(username, users[i].username) == 0)
        {
            // �û���ƥ�䣬��֤����
            if (strcmp(password, users[i].password) == 0)
            {
                sprintf(buf, "Login successfully.\n\n");
                write(connfd[n], buf, strlen(buf));

                // ���ҵ�һ�����е�λ�ò����û����Ϊ����
                for (j = 0; j < MAXMEM; j++)
                {
                    if (online_users[j].status == -1)
                    {
                        strcpy(online_users[j].username, username);
                        online_users[j].socketfd = n;
                        online_users[j].status = 0;
                        break;  // �ҵ�����λ�ú��˳�
                    }
                }
                return 0;  // ��¼�ɹ�
            }
            else
            {
                // �������
                sprintf(buf, "Wrong password.\n\n");
                write(connfd[n], buf, strlen(buf));
                return -1;  // �������
            }
        }
    }

    // �û���������
    sprintf(buf, "Account does not exist.\n\n");
    write(connfd[n], buf, strlen(buf));
    return -1;  // �û���������
}

//�û�ע��
void register_user(int n)
{
    int len, i;
    char buf[BUFFSIZE], username[20], password[20];

    // ��ʾ�û������û���
    sprintf(buf, "Your username: ");
    send(connfd[n], buf, strlen(buf));

    // ��ȡ�û���
    len = recv(connfd[n], username, 20);
    if (len > 0)
    {
        username[len - 1] = '\0'; // ȥ�����з�
    }

    // ��ʾ�û���������
    sprintf(buf, "Your password: ");
    send(connfd[n], buf, strlen(buf));

    // ��ȡ����
    len = recv(connfd[n], password, 20);
    if (len > 0)
    {
        password[len - 1] = '\0'; // ȥ�����з�
    }

    // ����û����Ƿ��Ѿ�����
    for (i = 0; i < user_count; i++)
    {
        if (strcmp(users[i].username, username) == 0)
        {
            sprintf(buf, "The username already exists.\n\n");
            write(connfd[n], buf, strlen(buf));
            return;
        }
    }

    // ����û������������޷�ע��
    if (user_count >= MAXMEM)
    {
        sprintf(buf, "User registration limit reached.\n\n");
        write(connfd[n], buf, strlen(buf));
        return;
    }

    // �������û���Ϣ
    strcpy(users[user_count].username, username);
    strcpy(users[user_count].password, password);
    user_count++;

    // ����ע��ɹ���Ϣ
    sprintf(buf, "Account created successfully.\n\n");
    write(connfd[n], buf, strlen(buf));
}

/*�û�����˽����Ϣ*/
void send_private_msg(char *username, char *data, SOCKET sfd)
{
    int i, j;
    time_t now;
    char send_man[20];
    char buf[BUFFSIZE], nowtime[20], temp[30];
    now = time(NULL);  // ��ȡ��ǰʱ��
    struct tm *tempTime = localtime(&now);
    strftime(nowtime, sizeof(nowtime), "[%H:%M:%S]", tempTime);  // ��ʽ��ʱ��

    // ���ҷ����ߵ��û���
    for (j = 0; j < MAXMEM; j++)
    {
        if (sfd == online_users[j].socketfd)
        {
            strcpy(send_man, online_users[j].username);
            break;
        }
    }

    // ����Ŀ���û���������Ϣ
    int found = 0;  // ����Ƿ��ҵ�Ŀ���û�
    for (i = 0; i < MAXMEM; i++)
    {
        if (strcmp(username, online_users[i].username) == 0)
        {
            snprintf(buf, sizeof(buf), "%s\tfrom %s:\n%s\n", nowtime, send_man, data);
            send(connfd[online_users[i].socketfd], buf, strlen(buf) + 1);  // ����˽����Ϣ
            strcpy(temp, "Sent successfully.\n");
            send(connfd[sfd], temp, strlen(temp) + 1);  // ���ͳɹ���ִ
            found = 1;
            break;
        }
    }

    // ���δ�ҵ�Ŀ���û�
    if (!found)
    {
        strcpy(buf, "User is not online or user does not exist.\n");
        send(connfd[sfd], buf, strlen(buf) + 1);  // ���ش�����Ϣ��������
    }
}

//��ϢȺ��
void send_all_msg(char *msg, SOCKET sfd)
{
    int i;
    char buf[BUFFSIZE], nowtime[20], send_man[20], temp[30];
    time_t now;
    now = time(NULL);  // ��ȡ��ǰʱ��
    struct tm *tempTime = localtime(&now);
    strftime(nowtime, sizeof(nowtime), "[%H:%M:%S]", tempTime);  // ��ʽ��ʱ��

    // ���ҷ����ߵ��û���
    for (i = 0; i < MAXMEM; i++)
    {
        if (sfd == online_users[i].socketfd)
        {
            strcpy(send_man, online_users[i].username);
            break;
        }
    }

    // ������Ϣ����
    snprintf(buf, sizeof(buf), "%s\tfrom %s(goup-sent):\n%s\n", nowtime, send_man, msg);

    // Ⱥ����Ϣ�����������û�
    for (i = 0; i < MAXMEM; i++)
    {
        if (connfd[i] != -1 && i != sfd)  // ȷ��������Ч�Ҳ��Ƿ������Լ�
        {
            send(connfd[i], buf, strlen(buf) + 1);  // ������Ϣ
        }
    }

    // �����߷��ط��ͳɹ��Ļ�ִ
    snprintf(temp, sizeof(temp), "Sent successfully\n");
    send(connfd[sfd], temp, strlen(temp) + 1);
}

void get_online_users(SOCKET sfd)
{
    int i;
    char buf[BUFFSIZE], nowtime[20];
    time_t now;
    now = time(NULL);  // ��ȡ��ǰʱ��
    struct tm *tempTime = localtime(&now);
    strftime(nowtime, sizeof(nowtime), "[%H:%M:%S]", tempTime);  // ��ʽ��ʱ��

    // �������ص���Ϣ����
    snprintf(buf, sizeof(buf), "%s\tAll online user(s):\n", nowtime);

    // ���ÿ�������û����û���
    for (i = 0; i < MAXMEM; i++)
    {
        if (online_users[i].status == 0)  // ֻ�г������û�
        {
            snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "\t%s\n", online_users[i].username);
        }
    }

    // ���͸�����Ŀͻ���
    send(connfd[sfd], buf, strlen(buf) + 1);
}

//������Ϣ��������
void send_chatroom_msg(char *msg, SOCKET sfd)
{
    int i, j, k;
    int flag = -1;
    char buf[BUFFSIZE], nowtime[20];
    time_t now;
    now = time(NULL);
    struct tm *tempTime = localtime(&now);
    strftime(nowtime, sizeof(nowtime), "[%H:%M:%S]", tempTime);  // ��ʽ����ǰʱ��

    // ����û��Ƿ������������
    for (i = 0; i < MAXROOM; i++)
    {
        if (chatrooms[i].status == 0)  // �������Ч��������
        {
            for (j = 0; j < 10; j++)
            {
                if (chatrooms[i].user[j] == sfd)  // ����Ƿ��������ҵĳ�Ա
                {
                    flag = 0;
                    break;
                }
            }
        }
        if (flag == 0)
            break;
    }

    if (flag == -1)
    {
        // �û�δ����������
        strcpy(buf, "You have not joined the chat room.\n");
        send(connfd[sfd], buf, strlen(buf) + 1);
    }
    else
    {
        // ��ȡ������Ϣ���û���
        for (k = 0; k < MAXMEM; k++)
        {
            if (online_users[k].socketfd == sfd)  // �ҵ�������Ϣ���û�
                break;
        }

        // ������Ϣ����
        snprintf(buf, sizeof(buf), "%s\tchatroom %s:\nfrom %s:\t%s\n",
                 nowtime, chatrooms[i].name, online_users[k].username, msg);

        // ���������ڵ����г�Ա������Ϣ
        for (k = 0; k < 10; k++)
        {
            if (chatrooms[i].user[k] != -1)
            {
                send(connfd[chatrooms[i].user[k]], buf, strlen(buf) + 1);
            }
        }
    }
}

//����������
void create_chatroom(char *name, char *passwd, SOCKET sfd)
{
    int i, j;
    char buf[BUFFSIZE];

    // ���ҵ�һ�����е�������λ��
    for (i = 0; i < MAXROOM; i++)
    {
        if (chatrooms[i].status == -1)  // �ҵ�δʹ�õ�������
            break;
    }

    if (i == MAXROOM)
    {
        // ���û�п��е������ң����ش�����Ϣ
        strcpy(buf, "No available rooms to create.\n");
        send(connfd[sfd], buf, strlen(buf) + 1);
        return;
    }

    // ��ȫ�ؿ������������ƺ����룬�������
    snprintf(chatrooms[i].name, sizeof(chatrooms[i].name), "%s", name);
    snprintf(chatrooms[i].passwd, sizeof(chatrooms[i].passwd), "%s", passwd);
    chatrooms[i].status = 0;  // ��������״̬����Ϊ���Ѵ�����

    // ���û�����������
    for (j = 0; j < 10; j++)
    {
        if (chatrooms[i].user[j] == -1)  // �ҵ���λ
        {
            chatrooms[i].user[j] = sfd;  // ���û�����������
            break;
        }
    }

    // ֪ͨ�û������Ҵ����ɹ�
    snprintf(buf, sizeof(buf), "Successfully created chat room '%s'.\n", chatrooms[i].name);
    send(connfd[sfd], buf, strlen(buf) + 1);
}

void join_chatroom(char *name, char *passwd, SOCKET sfd)
{
    int i, j;
    char buf[BUFFSIZE];
    int room_found = -1;

    // ����û��Ƿ��Ѿ���ĳ��������
    for (i = 0; i < MAXROOM; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (chatrooms[i].user[j] == sfd)
            {
                room_found = i;
                break;
            }
        }
        if (room_found != -1)
            break;  // �û��Ѿ���ĳ����������
    }

    if (room_found != -1)
    {
        snprintf(buf, sizeof(buf), "You have already joined the chat room '%s'.\n", chatrooms[room_found].name);
        send(connfd[sfd], buf, strlen(buf) + 1);
        return;  // �û��Ѿ�����������ң�ֱ�ӷ���
    }

    // ����Ŀ�������Ҳ�����
    for (i = 0; i < MAXROOM; i++)
    {
        if (chatrooms[i].status != -1 && strcmp(chatrooms[i].name, name) == 0)  // �ҵ�������
        {
            if (strcmp(chatrooms[i].passwd, passwd) == 0)  // ������ȷ
            {
                // ���ҿ�λ�ü���������
                for (j = 0; j < 10; j++)
                {
                    if (chatrooms[i].user[j] == -1)  // �ҵ���λ
                    {
                        chatrooms[i].user[j] = sfd;
                        snprintf(buf, sizeof(buf), "Successfully joined the chat room '%s'.\n", chatrooms[i].name);
                        send(connfd[sfd], buf, strlen(buf) + 1);
                        return;  // �ɹ�����������
                    }
                }
                // �������������
                snprintf(buf, sizeof(buf), "Chat room '%s' is full.\n", chatrooms[i].name);
                send(connfd[sfd], buf, strlen(buf) + 1);
                return;
            }
            else
            {
                // �������
                snprintf(buf, sizeof(buf), "Incorrect password for chat room '%s'.\n", name);
                send(connfd[sfd], buf, strlen(buf) + 1);
                return;
            }
        }
    }

    // ���û���ҵ���������
    snprintf(buf, sizeof(buf), "Chat room '%s' does not exist.\n", name);
    send(connfd[sfd], buf, strlen(buf) + 1);
}

//��ȡ��������Ϣ
void get_online_chatrooms(SOCKET sfd)
{
    int i;
    char buf[BUFFSIZE];
    time_t now = time(NULL);
    struct tm *tempTime = localtime(&now);

    // ��ȡ��ǰʱ��
    snprintf(buf, sizeof(buf), "[%02d:%02d:%02d]\tAll online chat room(s):\n",
             tempTime->tm_hour, tempTime->tm_min, tempTime->tm_sec);

    // ��ȡ��������������
    for (i = 0; i < MAXROOM; i++)
    {
        if (chatrooms[i].status == 0)  // �ж��������Ƿ�����
        {
            snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "\t%s\n", chatrooms[i].name);
        }
    }

    send(connfd[sfd], buf, strlen(buf) + 1);
}

//�޸�����
void change_passwd(SOCKET sfd, char *passwd)
{
    int i;
    char buf[BUFFSIZE], name[20];

    // ��ȡ��ǰ�û����û���
    for (i = 0; i < MAXMEM; i++)
    {
        if (sfd == online_users[i].socketfd)
        {
            strncpy(name, online_users[i].username, sizeof(name) - 1);
            name[sizeof(name) - 1] = '\0';  // ȷ���ַ�����ֹ
            break;
        }
    }

    // ���Ҳ���������
    for (i = 0; i < MAXMEM; i++)
    {
        if (strcmp(name, users[i].username) == 0)
        {
            strncpy(users[i].password, passwd, sizeof(users[i].password) - 1);
            users[i].password[sizeof(users[i].password) - 1] = '\0';  // ȷ���ַ�����ֹ
            snprintf(buf, sizeof(buf), "Password has been updated for user: %s\n", name);
            send(connfd[sfd], buf, strlen(buf) + 1);
            break;
        }
    }
}

/*��ѯ���м���ĳ�����ҵ��û�*/
void get_inroom_users(SOCKET sfd)
{
    int i, j;
    int room = -1;
    char buf[BUFFSIZE], nowtime[20];
    time_t now;
    time(&now);
    struct tm *tempTime = localtime(&now);
    strftime(nowtime, 20, "[%H:%M:%S]", tempTime);

    // �����û����ڵ�������
    for (i = 0; i < MAXROOM; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (chatrooms[i].user[j] == sfd)
            {
                room = i;
                break;
            }
        }
        if (room != -1)
            break;  // �û��Ѽ���ĳ�����ң�����ѭ��
    }

    // ����û�û�м����κ�������
    if (room == -1)
    {
        snprintf(buf, sizeof(buf), "Sorry, you have not joined any chat room.\n");
        send(connfd[sfd], buf, strlen(buf) + 1);
    }
    else
    {
        snprintf(buf, sizeof(buf), "%s\tAll users in the %s chat room:\n", nowtime, chatrooms[room].name);

        // ���Ҳ��г��������е����������û�
        for (i = 0; i < 10; i++)
        {
            if (chatrooms[room].user[i] >= 0)
            {
                for (j = 0; j < MAXMEM; j++)
                {
                    if (online_users[j].status != -1 && (chatrooms[room].user[i] == online_users[j].socketfd))
                    {
                        strncat(buf, "\t", sizeof(buf) - strlen(buf) - 1);
                        strncat(buf, online_users[j].username, sizeof(buf) - strlen(buf) - 1);
                        strncat(buf, "\n", sizeof(buf) - strlen(buf) - 1);
                    }
                }
            }
        }
        send(connfd[sfd], buf, strlen(buf) + 1);
    }
}

//�˳�������
void exit_chatroom(SOCKET sfd)
{
    int i, j;
    int room = -1;
    char buf[BUFFSIZE];

    // �����û����ڵ�������
    for (i = 0; i < MAXROOM; i++)
    {
        if (chatrooms[i].status == 0)
        {
            for (j = 0; j < 10; j++)
            {
                if (chatrooms[i].user[j] == sfd)
                {
                    chatrooms[i].user[j] = -1;  // ���û��Ƴ�������
                    room = i;
                    break;
                }
            }
            if (room != -1)
                break;  // �ҵ������Һ������˳�ѭ��
        }
    }

    // ����û�û�м����κ�������
    if (room == -1)
    {
        snprintf(buf, sizeof(buf), "You have not joined any chat room.\n");
        send(connfd[sfd], buf, strlen(buf) + 1);
    }
    else
    {
        snprintf(buf, sizeof(buf), "Successfully quit chat room %s.\n", chatrooms[room].name);
        send(connfd[sfd], buf, strlen(buf) + 1);
    }
}

void quit()
{
    int i;
    char msg[10];

    while (1)
    {
        printf("Enter 'quit' to shut down the server: ");
        if (fgets(msg, sizeof(msg), stdin) == NULL)
        {
            perror("Error reading input");
            continue;  // �����ȴ�����
        }

        // ȥ�������ַ����Ļ��з�
        msg[strcspn(msg, "\n")] = '\0';

        if (strcmp(msg, "quit") == 0)
        {
            save_users();
            printf("Byebye... \n");

            // �رռ����׽���
            if (closesocket(listenfd) == -1)
            {
                perror("Error closing listenfd");
                exit(1);
            }

            exit(0);
        }
    }
}

void invalid_command(SOCKET sfd)
{
    char buf[BUFFSIZE];

    // ��ʾ��Ч����ṩ������Ϣ
    strcpy(buf, "Invalid command. Please use one of the following valid commands:\n");
    strcat(buf, "/join <room_name> <password> - Join a chat room\n");
    strcat(buf, "/create <room_name> <password> - Create a new chat room\n");
    strcat(buf, "/exit - Exit the chat room\n");
    strcat(buf, "/quit - Quit the server\n");

    send(connfd[sfd], buf, strlen(buf) + 1);
}




