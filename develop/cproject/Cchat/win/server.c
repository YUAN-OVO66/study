#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <time.h>
#include <pthread.h>

#pragma comment(lib, "Ws2_32.lib") // 链接 Winsock 库

#define PORT 8888
#define MAXMEM 20
#define MAXROOM 5
#define BUFFSIZE 256

int user_count;     // 记录总的用户数
int chatroom_count; // 记录聊天室个数
SOCKET listenfd, connfd[MAXMEM];
struct user users[MAXMEM];               // 记录所有用户
struct user_socket online_users[MAXMEM]; // 记录在线用户
struct chatroom chatrooms[MAXROOM];      // 记录聊天室

// 函数声明
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
    // Winsock 初始化
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

    // 清空服务器地址结构
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 创建监听套接字
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == INVALID_SOCKET)
    {
        perror("fail to socket");
        WSACleanup();
        exit(-1);
    }

    // 绑定套接字
    if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
    {
        perror("fail to bind");
        closesocket(listenfd);
        WSACleanup();
        exit(-2);
    }

    // 开始监听
    if (listen(listenfd, MAXMEM) == SOCKET_ERROR)
    {
        perror("fail to listen");
        closesocket(listenfd);
        WSACleanup();
        exit(-3);
    }

    // 创建一个线程，用于管理服务器，调用 quit 函数
    pthread_create(&thread, NULL, (void *)(quit), NULL);

    // 初始化连接套接字数组
    for (i = 0; i < MAXMEM; i++)
        connfd[i] = INVALID_SOCKET;

    // 接收客户端连接
    while (1)
    {
        int len;
        for (i = 0; i < MAXMEM; i++)
        {
            if (connfd[i] == INVALID_SOCKET)
                break;
        }

        // 接受连接
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

        // 针对当前套接字创建一个线程来处理消息
        pthread_t recv_thread;
        pthread_create(&recv_thread, NULL, (void *)(&rcv_snd), (void *)i);
    }

    // 清理 Winsock
    WSACleanup();

    return 0;
}


/*服务器接收和发送函数*/
void rcv_snd(int n)
{
    ssize_t len;
    int i;
    char mytime[32], buf[BUFFSIZE];
    char temp[BUFFSIZE];
    char command[20], arg1[20], arg2[BUFFSIZE];
    time_t timenow;

    // 第一个循环，用于接收和处理登录或注册命令
    while (1)
    {
        len = recv(connfd[n], buf, BUFFSIZE);
        if (len > 0)
        {
            buf[len - 1] = '\0'; // 去除换行符

            // 处理登录请求
            if (strcmp(buf, "login") == 0)
            {
                // 登录成功时退出该循环
                if (user_login(n) == 0)
                {
                    break;
                }
            }
                // 处理注册请求
            else if (strcmp(buf, "register") == 0)
            {
                register_user(n);
            }
                // 处理退出请求
            else if (strcmp(buf, "quit") == 0)
            {
                quit_client(n);
            }
        }
    }

    // 第二个循环，用于处理登录后的命令
    while (1)
    {
        if ((len = recv(connfd[n], temp, BUFFSIZE)) > 0)
        {
            temp[len - 1] = '\0'; // 去除换行符
            sscanf(temp, "%s %s %[^\n]", command, arg1, arg2); // 解析命令

            // 处理发送消息的命令
            if (strcmp(command, "send") == 0 && strcmp(arg1, "-all") == 0)
            {
                send_all_msg(arg2, n);  // 向所有在线用户发送消息
            }
            else if (strcmp(command, "send") == 0 && strcmp(arg1, "-chatroom") == 0)
            {
                send_chatroom_msg(arg2, n);  // 向聊天室中的所有用户发送消息
            }
            else if (strcmp(command, "send") == 0)
            {
                send_private_msg(arg1, arg2, n);  // 向指定用户发送私聊消息
            }
                // 处理退出命令
            else if (strcmp(command, "quit") == 0)
            {
                quit_client(n);
            }
                // 处理修改密码命令
            else if (strcmp(command, "chgpsw") == 0)
            {
                change_passwd(n, arg1);
            }
                // 处理创建聊天室命令
            else if (strcmp(command, "create") == 0)
            {
                create_chatroom(arg1, arg2, n);
            }
                // 处理加入聊天室命令
            else if (strcmp(command, "join") == 0)
            {
                join_chatroom(arg1, arg2, n);
            }
                // 处理列出聊天室命令
            else if (strcmp(command, "ls") == 0 && strcmp(arg1, "-chatrooms") == 0)
            {
                get_online_chatrooms(n);
            }
                // 处理列出在线用户命令
            else if (strcmp(command, "ls") == 0 && strcmp(arg1, "-users") == 0)
            {
                get_online_users(n);
            }
                // 处理列出聊天室内用户命令
            else if (strcmp(command, "ls") == 0 && strcmp(arg1, "-inrmusr") == 0)
            {
                get_inroom_users(n);
            }
                // 处理退出聊天室命令
            else if (strcmp(command, "exit") == 0)
            {
                exit_chatroom(n);
            }
                // 处理无效命令
            else
            {
                invalid_command(n);
            }
        }
    }
}


//初始化
void init()
{
    int i, j;
    user_count = 0;
    chatroom_count = 0;

    // 初始化在线用户状态
    for (i = 0; i < MAXMEM; i++)
    {
        online_users[i].status = -1; // -1 表示离线
    }

    // 初始化聊天室状态
    for (i = 0; i < MAXROOM; i++)
    {
        chatrooms[i].status = -1; // -1 表示聊天室不存在
        for (j = 0; j < 10; j++)
        {
            chatrooms[i].user[j] = -1; // 初始化聊天室中的用户为 -1（无用户）
        }
    }

    // 打开文件并从文件读取用户数据
    char buf[20];
    FILE *fp = NULL;
    fp = fopen("users.txt", "r");
    if (fp == NULL)
    {
        printf("Error opening users.txt file.\n");
        return;
    }

    // 从文件中读取用户名和密码并存储
    while (fscanf(fp, "%s", buf) != EOF)
    {
        strcpy(users[user_count].username, buf);  // 读取用户名
        fscanf(fp, "%s", buf);
        strcpy(users[user_count].password, buf);  // 读取密码
        user_count++;  // 增加用户数量
    }

    // 关闭文件
    fclose(fp);
}


/*将用户保存到文件*/
void save_users()
{
    int i;
    FILE *fp = NULL;

    // 打开文件以进行写操作，如果文件不存在则创建
    fp = fopen("users.txt", "w+");
    if (fp == NULL)  // 检查文件是否成功打开
    {
        printf("Error opening users.txt file.\n");
        return;
    }

    // 遍历用户数组，写入每个用户的用户名和密码
    for (i = 0; i < user_count; i++)
    {
        // 使用 fprintf 直接将用户名和密码写入文件
        fprintf(fp, "%s\n", users[i].username);
        fprintf(fp, "%s\n", users[i].password);
    }

    // 关闭文件
    fclose(fp);
}

void quit_client(int n)
{
    int i;

    // 确保套接字连接有效
    if (connfd[n] != -1)
    {
        closesocket(connfd[n]);  // 关闭套接字连接
        connfd[n] = -1;    // 更新连接状态，标记该位置为空
    }

    // 更新在线用户状态
    for (i = 0; i < MAXMEM; i++)
    {
        if (online_users[i].socketfd == connfd[n])
        {
            online_users[i].status = -1;  // 标记用户下线
            break;  // 找到用户并更新后跳出循环
        }
    }

    // 退出线程，若不需要返回退出状态，直接调用pthread_exit()
    pthread_exit(NULL);
}

//用户登录
int user_login(int n)
{
    int len, i, j;
    char buf[BUFFSIZE], username[20], password[20];

    // 提示用户输入用户名
    sprintf(buf, "Your username: ");
    send(connfd[n], buf, strlen(buf));

    // 读取用户名
    len = recv(connfd[n], username, 20);
    if (len > 0)
    {
        username[len - 1] = '\0'; // 去除换行符
    }

    // 提示用户输入密码
    sprintf(buf, "Your password: ");
    send(connfd[n], buf, strlen(buf));

    // 读取密码
    len = recv(connfd[n], password, 20);
    if (len > 0)
    {
        password[len - 1] = '\0'; // 去除换行符
    }

    // 查找用户名并验证密码
    for (i = 0; i < MAXMEM; i++)
    {
        if (strcmp(username, users[i].username) == 0)
        {
            // 用户名匹配，验证密码
            if (strcmp(password, users[i].password) == 0)
            {
                sprintf(buf, "Login successfully.\n\n");
                write(connfd[n], buf, strlen(buf));

                // 查找第一个空闲的位置并将用户标记为在线
                for (j = 0; j < MAXMEM; j++)
                {
                    if (online_users[j].status == -1)
                    {
                        strcpy(online_users[j].username, username);
                        online_users[j].socketfd = n;
                        online_users[j].status = 0;
                        break;  // 找到空闲位置后退出
                    }
                }
                return 0;  // 登录成功
            }
            else
            {
                // 密码错误
                sprintf(buf, "Wrong password.\n\n");
                write(connfd[n], buf, strlen(buf));
                return -1;  // 密码错误
            }
        }
    }

    // 用户名不存在
    sprintf(buf, "Account does not exist.\n\n");
    write(connfd[n], buf, strlen(buf));
    return -1;  // 用户名不存在
}

//用户注册
void register_user(int n)
{
    int len, i;
    char buf[BUFFSIZE], username[20], password[20];

    // 提示用户输入用户名
    sprintf(buf, "Your username: ");
    send(connfd[n], buf, strlen(buf));

    // 读取用户名
    len = recv(connfd[n], username, 20);
    if (len > 0)
    {
        username[len - 1] = '\0'; // 去除换行符
    }

    // 提示用户输入密码
    sprintf(buf, "Your password: ");
    send(connfd[n], buf, strlen(buf));

    // 读取密码
    len = recv(connfd[n], password, 20);
    if (len > 0)
    {
        password[len - 1] = '\0'; // 去除换行符
    }

    // 检查用户名是否已经存在
    for (i = 0; i < user_count; i++)
    {
        if (strcmp(users[i].username, username) == 0)
        {
            sprintf(buf, "The username already exists.\n\n");
            write(connfd[n], buf, strlen(buf));
            return;
        }
    }

    // 如果用户数量已满，无法注册
    if (user_count >= MAXMEM)
    {
        sprintf(buf, "User registration limit reached.\n\n");
        write(connfd[n], buf, strlen(buf));
        return;
    }

    // 保存新用户信息
    strcpy(users[user_count].username, username);
    strcpy(users[user_count].password, password);
    user_count++;

    // 返回注册成功信息
    sprintf(buf, "Account created successfully.\n\n");
    write(connfd[n], buf, strlen(buf));
}

/*用户发送私聊信息*/
void send_private_msg(char *username, char *data, SOCKET sfd)
{
    int i, j;
    time_t now;
    char send_man[20];
    char buf[BUFFSIZE], nowtime[20], temp[30];
    now = time(NULL);  // 获取当前时间
    struct tm *tempTime = localtime(&now);
    strftime(nowtime, sizeof(nowtime), "[%H:%M:%S]", tempTime);  // 格式化时间

    // 查找发送者的用户名
    for (j = 0; j < MAXMEM; j++)
    {
        if (sfd == online_users[j].socketfd)
        {
            strcpy(send_man, online_users[j].username);
            break;
        }
    }

    // 查找目标用户并发送消息
    int found = 0;  // 标记是否找到目标用户
    for (i = 0; i < MAXMEM; i++)
    {
        if (strcmp(username, online_users[i].username) == 0)
        {
            snprintf(buf, sizeof(buf), "%s\tfrom %s:\n%s\n", nowtime, send_man, data);
            send(connfd[online_users[i].socketfd], buf, strlen(buf) + 1);  // 发送私聊消息
            strcpy(temp, "Sent successfully.\n");
            send(connfd[sfd], temp, strlen(temp) + 1);  // 发送成功回执
            found = 1;
            break;
        }
    }

    // 如果未找到目标用户
    if (!found)
    {
        strcpy(buf, "User is not online or user does not exist.\n");
        send(connfd[sfd], buf, strlen(buf) + 1);  // 返回错误消息给发送者
    }
}

//消息群发
void send_all_msg(char *msg, SOCKET sfd)
{
    int i;
    char buf[BUFFSIZE], nowtime[20], send_man[20], temp[30];
    time_t now;
    now = time(NULL);  // 获取当前时间
    struct tm *tempTime = localtime(&now);
    strftime(nowtime, sizeof(nowtime), "[%H:%M:%S]", tempTime);  // 格式化时间

    // 查找发送者的用户名
    for (i = 0; i < MAXMEM; i++)
    {
        if (sfd == online_users[i].socketfd)
        {
            strcpy(send_man, online_users[i].username);
            break;
        }
    }

    // 构建消息内容
    snprintf(buf, sizeof(buf), "%s\tfrom %s(goup-sent):\n%s\n", nowtime, send_man, msg);

    // 群发消息给所有在线用户
    for (i = 0; i < MAXMEM; i++)
    {
        if (connfd[i] != -1 && i != sfd)  // 确保连接有效且不是发送者自己
        {
            send(connfd[i], buf, strlen(buf) + 1);  // 发送消息
        }
    }

    // 向发送者返回发送成功的回执
    snprintf(temp, sizeof(temp), "Sent successfully\n");
    send(connfd[sfd], temp, strlen(temp) + 1);
}

void get_online_users(SOCKET sfd)
{
    int i;
    char buf[BUFFSIZE], nowtime[20];
    time_t now;
    now = time(NULL);  // 获取当前时间
    struct tm *tempTime = localtime(&now);
    strftime(nowtime, sizeof(nowtime), "[%H:%M:%S]", tempTime);  // 格式化时间

    // 构建返回的消息内容
    snprintf(buf, sizeof(buf), "%s\tAll online user(s):\n", nowtime);

    // 添加每个在线用户的用户名
    for (i = 0; i < MAXMEM; i++)
    {
        if (online_users[i].status == 0)  // 只列出在线用户
        {
            snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "\t%s\n", online_users[i].username);
        }
    }

    // 发送给请求的客户端
    send(connfd[sfd], buf, strlen(buf) + 1);
}

//发送消息到聊天室
void send_chatroom_msg(char *msg, SOCKET sfd)
{
    int i, j, k;
    int flag = -1;
    char buf[BUFFSIZE], nowtime[20];
    time_t now;
    now = time(NULL);
    struct tm *tempTime = localtime(&now);
    strftime(nowtime, sizeof(nowtime), "[%H:%M:%S]", tempTime);  // 格式化当前时间

    // 检查用户是否加入了聊天室
    for (i = 0; i < MAXROOM; i++)
    {
        if (chatrooms[i].status == 0)  // 仅检查有效的聊天室
        {
            for (j = 0; j < 10; j++)
            {
                if (chatrooms[i].user[j] == sfd)  // 检查是否是聊天室的成员
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
        // 用户未加入聊天室
        strcpy(buf, "You have not joined the chat room.\n");
        send(connfd[sfd], buf, strlen(buf) + 1);
    }
    else
    {
        // 获取发送消息的用户名
        for (k = 0; k < MAXMEM; k++)
        {
            if (online_users[k].socketfd == sfd)  // 找到发送消息的用户
                break;
        }

        // 构建消息内容
        snprintf(buf, sizeof(buf), "%s\tchatroom %s:\nfrom %s:\t%s\n",
                 nowtime, chatrooms[i].name, online_users[k].username, msg);

        // 向聊天室内的所有成员发送消息
        for (k = 0; k < 10; k++)
        {
            if (chatrooms[i].user[k] != -1)
            {
                send(connfd[chatrooms[i].user[k]], buf, strlen(buf) + 1);
            }
        }
    }
}

//创建聊天室
void create_chatroom(char *name, char *passwd, SOCKET sfd)
{
    int i, j;
    char buf[BUFFSIZE];

    // 查找第一个空闲的聊天室位置
    for (i = 0; i < MAXROOM; i++)
    {
        if (chatrooms[i].status == -1)  // 找到未使用的聊天室
            break;
    }

    if (i == MAXROOM)
    {
        // 如果没有空闲的聊天室，返回错误信息
        strcpy(buf, "No available rooms to create.\n");
        send(connfd[sfd], buf, strlen(buf) + 1);
        return;
    }

    // 安全地拷贝聊天室名称和密码，避免溢出
    snprintf(chatrooms[i].name, sizeof(chatrooms[i].name), "%s", name);
    snprintf(chatrooms[i].passwd, sizeof(chatrooms[i].passwd), "%s", passwd);
    chatrooms[i].status = 0;  // 将聊天室状态设置为“已创建”

    // 将用户加入聊天室
    for (j = 0; j < 10; j++)
    {
        if (chatrooms[i].user[j] == -1)  // 找到空位
        {
            chatrooms[i].user[j] = sfd;  // 将用户加入聊天室
            break;
        }
    }

    // 通知用户聊天室创建成功
    snprintf(buf, sizeof(buf), "Successfully created chat room '%s'.\n", chatrooms[i].name);
    send(connfd[sfd], buf, strlen(buf) + 1);
}

void join_chatroom(char *name, char *passwd, SOCKET sfd)
{
    int i, j;
    char buf[BUFFSIZE];
    int room_found = -1;

    // 检查用户是否已经在某个聊天室
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
            break;  // 用户已经在某个聊天室中
    }

    if (room_found != -1)
    {
        snprintf(buf, sizeof(buf), "You have already joined the chat room '%s'.\n", chatrooms[room_found].name);
        send(connfd[sfd], buf, strlen(buf) + 1);
        return;  // 用户已经加入过聊天室，直接返回
    }

    // 查找目标聊天室并加入
    for (i = 0; i < MAXROOM; i++)
    {
        if (chatrooms[i].status != -1 && strcmp(chatrooms[i].name, name) == 0)  // 找到聊天室
        {
            if (strcmp(chatrooms[i].passwd, passwd) == 0)  // 密码正确
            {
                // 查找空位置加入聊天室
                for (j = 0; j < 10; j++)
                {
                    if (chatrooms[i].user[j] == -1)  // 找到空位
                    {
                        chatrooms[i].user[j] = sfd;
                        snprintf(buf, sizeof(buf), "Successfully joined the chat room '%s'.\n", chatrooms[i].name);
                        send(connfd[sfd], buf, strlen(buf) + 1);
                        return;  // 成功加入聊天室
                    }
                }
                // 如果聊天室已满
                snprintf(buf, sizeof(buf), "Chat room '%s' is full.\n", chatrooms[i].name);
                send(connfd[sfd], buf, strlen(buf) + 1);
                return;
            }
            else
            {
                // 密码错误
                snprintf(buf, sizeof(buf), "Incorrect password for chat room '%s'.\n", name);
                send(connfd[sfd], buf, strlen(buf) + 1);
                return;
            }
        }
    }

    // 如果没有找到该聊天室
    snprintf(buf, sizeof(buf), "Chat room '%s' does not exist.\n", name);
    send(connfd[sfd], buf, strlen(buf) + 1);
}

//获取聊天室信息
void get_online_chatrooms(SOCKET sfd)
{
    int i;
    char buf[BUFFSIZE];
    time_t now = time(NULL);
    struct tm *tempTime = localtime(&now);

    // 获取当前时间
    snprintf(buf, sizeof(buf), "[%02d:%02d:%02d]\tAll online chat room(s):\n",
             tempTime->tm_hour, tempTime->tm_min, tempTime->tm_sec);

    // 获取所有在线聊天室
    for (i = 0; i < MAXROOM; i++)
    {
        if (chatrooms[i].status == 0)  // 判断聊天室是否在线
        {
            snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "\t%s\n", chatrooms[i].name);
        }
    }

    send(connfd[sfd], buf, strlen(buf) + 1);
}

//修改密码
void change_passwd(SOCKET sfd, char *passwd)
{
    int i;
    char buf[BUFFSIZE], name[20];

    // 获取当前用户的用户名
    for (i = 0; i < MAXMEM; i++)
    {
        if (sfd == online_users[i].socketfd)
        {
            strncpy(name, online_users[i].username, sizeof(name) - 1);
            name[sizeof(name) - 1] = '\0';  // 确保字符串终止
            break;
        }
    }

    // 查找并更新密码
    for (i = 0; i < MAXMEM; i++)
    {
        if (strcmp(name, users[i].username) == 0)
        {
            strncpy(users[i].password, passwd, sizeof(users[i].password) - 1);
            users[i].password[sizeof(users[i].password) - 1] = '\0';  // 确保字符串终止
            snprintf(buf, sizeof(buf), "Password has been updated for user: %s\n", name);
            send(connfd[sfd], buf, strlen(buf) + 1);
            break;
        }
    }
}

/*查询所有加入某聊天室的用户*/
void get_inroom_users(SOCKET sfd)
{
    int i, j;
    int room = -1;
    char buf[BUFFSIZE], nowtime[20];
    time_t now;
    time(&now);
    struct tm *tempTime = localtime(&now);
    strftime(nowtime, 20, "[%H:%M:%S]", tempTime);

    // 查找用户所在的聊天室
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
            break;  // 用户已加入某聊天室，跳出循环
    }

    // 如果用户没有加入任何聊天室
    if (room == -1)
    {
        snprintf(buf, sizeof(buf), "Sorry, you have not joined any chat room.\n");
        send(connfd[sfd], buf, strlen(buf) + 1);
    }
    else
    {
        snprintf(buf, sizeof(buf), "%s\tAll users in the %s chat room:\n", nowtime, chatrooms[room].name);

        // 查找并列出聊天室中的所有在线用户
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

//退出聊天室
void exit_chatroom(SOCKET sfd)
{
    int i, j;
    int room = -1;
    char buf[BUFFSIZE];

    // 查找用户所在的聊天室
    for (i = 0; i < MAXROOM; i++)
    {
        if (chatrooms[i].status == 0)
        {
            for (j = 0; j < 10; j++)
            {
                if (chatrooms[i].user[j] == sfd)
                {
                    chatrooms[i].user[j] = -1;  // 将用户移除聊天室
                    room = i;
                    break;
                }
            }
            if (room != -1)
                break;  // 找到聊天室后立即退出循环
        }
    }

    // 如果用户没有加入任何聊天室
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
            continue;  // 继续等待输入
        }

        // 去掉输入字符串的换行符
        msg[strcspn(msg, "\n")] = '\0';

        if (strcmp(msg, "quit") == 0)
        {
            save_users();
            printf("Byebye... \n");

            // 关闭监听套接字
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

    // 提示无效命令并提供帮助信息
    strcpy(buf, "Invalid command. Please use one of the following valid commands:\n");
    strcat(buf, "/join <room_name> <password> - Join a chat room\n");
    strcat(buf, "/create <room_name> <password> - Create a new chat room\n");
    strcat(buf, "/exit - Exit the chat room\n");
    strcat(buf, "/quit - Quit the server\n");

    send(connfd[sfd], buf, strlen(buf) + 1);
}




