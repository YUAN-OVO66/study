#include <pthread.h>
#include "server.h"

int main()
{
    init();

    struct sockaddr_in serv_addr, cli_addr;
    int i;
    time_t timenow;
    pthread_t thread;
    char buff[BUFFSIZE];

    printf("Running...\nEnter command \"quit\" to exit server.\n\n");

    bzero(&serv_addr, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        perror("fail to socket");
        exit(-1);
    }

    if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("fail to bind");
        exit(-2);
    }

    listen(listenfd, MAXMEM);

    // 创建一个线程，对服务器程序进行管理，调用quit函数
    pthread_create(&thread, NULL, (void *)(quit), NULL);

    // 将套接字描述符数组初始化为-1，表示空闲
    for (i = 0; i < MAXMEM; i++)
        connfd[i] = -1;

    while (1)
    {
        int len;
        for (i = 0; i < MAXMEM; i++)
        {
            if (connfd[i] == -1)
                break;
        }
        // accept 从listen接受的连接队列中取得一个连接
        connfd[i] = accept(listenfd, (struct sockaddr *)&cli_addr, &len);
        if (connfd[i] < 0)
        {
            perror("fail to accept.");
        }
        timenow = time(NULL);
        printf("%.24s\n\tconnect from: %s, port %d\n",
               ctime(&timenow), inet_ntop(AF_INET, &(cli_addr.sin_addr), buff, BUFFSIZE),
               ntohs(cli_addr.sin_port));

        // 针对当前套接字创建一个线程，对当前套接字的消息进行处理
        pthread_create(malloc(sizeof(pthread_t)), NULL, (void *)(&rcv_snd), (void *)i);
    }
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

    while (1)
    {
        len = read(connfd[n], buf, BUFFSIZE);
        if (len > 0)
        {
            buf[len - 1] = '\0'; // 去除换行符

            if (strcmp(buf, "login") == 0)
            {
                //登录成功时退出该循环
                if (user_login(n) == 0)
                {
                    break;
                }
            }
            else if (strcmp(buf, "register") == 0)
            {
                register_user(n);
            }
            else if (strcmp(buf, "quit") == 0)
            {
                quit_client(n);
            }
        }
    }

    while (1)
    {
        if ((len = read(connfd[n], temp, BUFFSIZE)) > 0)
        {
            temp[len - 1] = '\0';
            sscanf(temp, "%s %s %[^\n]", command, arg1, arg2); //解析命令

            /*根据解析出的命令执行不同的函数*/
            if (strcmp(command, "send") == 0 && strcmp(arg1, "-all") == 0)
            {
                send_all_msg(arg2, n);
            }
            else if (strcmp(command, "send") == 0 && strcmp(arg1, "-chatroom") == 0)
            {
                send_chatroom_msg(arg2, n);
            }
            else if (strcmp(command, "send") == 0)
            {
                send_private_msg(arg1, arg2, n);
            }
            else if (strcmp(command, "quit") == 0)
            {
                quit_client(n);
            }
            else if (strcmp(command, "chgpsw") == 0)
            {
                change_passwd(n, arg1);
            }
            else if (strcmp(command, "create") == 0)
            {
                create_chatroom(arg1, arg2, n);
            }
            else if (strcmp(command, "join") == 0)
            {
                join_chatroom(arg1, arg2, n);
            }
            else if (strcmp(command, "ls") == 0 && strcmp(arg1, "-chatrooms") == 0)
            {
                get_online_chatrooms(n);
            }
            else if (strcmp(command, "ls") == 0 && strcmp(arg1, "-users") == 0)
            {
                get_online_users(n);
            }
            else if (strcmp(command, "ls") == 0 && strcmp(arg1, "-inrmusr") == 0)
            {
                get_inroom_users(n);
            }
            else if (strcmp(command, "exit") == 0)
            {
                exit_chatroom(n);
            }
            else
            {
                invalid_command(n);
            }
        }
    }
}

/*初始化*/
void init()
{
    int i, j;
    user_count = 0;
    chatroom_count = 0;
    for (i = 0; i < MAXMEM; i++)
    {
        online_users[i].status = -1;
    }
    for (i = 0; i < MAXROOM; i++)
    {
        chatrooms[i].status = -1;
        for (j = 0; j < 10; j++)
        {
            chatrooms[i].user[j] = -1;
        }
    }
    char buf[20];
    FILE *fp = NULL;
    fp = fopen("users.txt", "r");
    //从文件中读取用户
    while (fscanf(fp, "%s", buf) != EOF)
    {
        strcpy(users[user_count].username, buf);
        fscanf(fp, "%s", buf);
        strcpy(users[user_count].password, buf);
        user_count++;
    }
    fclose(fp);
}

/*将用户保存到文件*/
void save_users()
{
    int i;
    char buf[20];
    FILE *fp = NULL;
    fp = fopen("users.txt", "w+");
    for (i = 0; i < user_count; i++)
    {
        strcpy(buf, users[i].username);
        strcat(buf, "\n");
        fprintf(fp, buf);
        strcpy(buf, users[i].password);
        strcat(buf, "\n");
        fprintf(fp, buf);
    }
    fclose(fp);
}

/*服务器处理用户退出*/
void quit_client(int n)
{
    int ret, i;
    close(connfd[n]);
    connfd[n] = -1;
    for (i = 0; i < MAXMEM; i++)
    {
        if (n == online_users[i].socketfd)
        {
            online_users[i].status = -1;
        }
    }
    pthread_exit(&ret);
}

/*用户登录*/
int user_login(int n)
{
    int len, i, j;
    char buf[BUFFSIZE], username[20], password[20];
    sprintf(buf, "your username: ");
    write(connfd[n], buf, strlen(buf) + 1);
    len = read(connfd[n], username, 20);
    if (len > 0)
    {
        username[len - 1] = '\0'; // 去除换行符
    }
    sprintf(buf, "your password: ");
    write(connfd[n], buf, strlen(buf) + 1);
    len = read(connfd[n], password, 20);
    if (len > 0)
    {
        password[len - 1] = '\0'; // 去除换行符
    }

    for (i = 0; i < MAXMEM; i++)
    {
        if (strcmp(username, users[i].username) == 0)
        {
            if (strcmp(password, users[i].password) == 0)
            {
                sprintf(buf, "Login successfully.\n\n");
                write(connfd[n], buf, strlen(buf + 1));
                for (j = 0; j < MAXMEM; j++)
                {
                    if (online_users[j].status == -1)
                        break;
                }
                strcpy(online_users[j].username, username);
                online_users[j].socketfd = n;
                online_users[j].status = 0;
                return 0;
            }
            else
            {
                sprintf(buf, "Wrong password.\n\n");
                write(connfd[n], buf, strlen(buf + 1));
                return -1;
            }
        }
    }
    sprintf(buf, "Account does not exist.\n\n");
    write(connfd[n], buf, strlen(buf + 1));
    return -1;
}

/*用户注册*/
void register_user(int n)
{
    int len, i;
    char buf[BUFFSIZE], username[20], password[20];
    sprintf(buf, "your username: ");
    write(connfd[n], buf, strlen(buf) + 1);
    len = read(connfd[n], username, 20);
    if (len > 0)
    {
        username[len - 1] = '\0'; // 去除换行符
    }
    sprintf(buf, "your password: ");
    write(connfd[n], buf, strlen(buf) + 1);
    len = read(connfd[n], password, 20);
    if (len > 0)
    {
        password[len - 1] = '\0'; // 去除换行符
    }
    for (i = 0; i < MAXMEM; i++)
    {
        if (strcmp(users[i].username, username) == 0)
        {
            strcpy(buf, "The username already exists.\n\n");
            write(connfd[n], buf, strlen(buf) + 1);
            return;
        }
    }
    strcpy(users[user_count].username, username);
    strcpy(users[user_count].password, password);
    user_count++;
    sprintf(buf, "Account created successfully.\n\n");
    write(connfd[n], buf, strlen(buf) + 1);
}

/*用户发送私聊信息*/
void send_private_msg(char *username, char *data, int sfd)
{
    int i, j;
    time_t now;
    char send_man[20];
    char buf[BUFFSIZE], nowtime[20], temp[30];
    now = time(NULL);
    time(&now);
    struct tm *tempTime = localtime(&now);
    strftime(nowtime, 20, "[%H:%M:%S]", tempTime);
    for (j = 0; j < MAXMEM; j++)
    {
        if (sfd == online_users[j].socketfd)
        {
            strcpy(send_man, online_users[j].username);
            break;
        }
    }
    for (i = 0; i < MAXMEM; i++)
    {
        if (strcmp(username, online_users[i].username) == 0)
        {
            strcpy(buf, nowtime);
            strcat(buf, "\t");
            strcat(buf, "from ");
            strcat(buf, send_man);
            strcat(buf, ":\n");
            strcat(buf, data);
            strcat(buf, "\n");
            write(connfd[online_users[i].socketfd], buf, strlen(buf) + 1);
            strcpy(temp, "Sent successfully.\n");
            write(connfd[sfd], temp, strlen(temp) + 1);
            return;
        }
    }
    strcpy(buf, "User is not online or user does not exist.\n");
    write(connfd[sfd], buf, strlen(buf) + 1);
    return;
}

/*用户群发信息给所有用户*/
void send_all_msg(char *msg, int sfd)
{
    int i;
    char buf[BUFFSIZE], nowtime[20], send_man[20], temp[30];
    time_t now;
    time(&now);
    struct tm *tempTime = localtime(&now);
    strftime(nowtime, 20, "[%H:%M:%S]", tempTime);
    for (i = 0; i < MAXMEM; i++)
    {
        if (sfd == online_users[i].socketfd)
        {
            strcpy(send_man, online_users[i].username);
            break;
        }
    }
    strcpy(buf, nowtime);
    strcat(buf, "\t");
    strcat(buf, "from ");
    strcat(buf, send_man);
    strcat(buf, "(goup-sent):\n");
    strcat(buf, msg);
    strcat(buf, "\n");
    for (i = 0; i < MAXMEM; i++)
    {
        if (connfd[i] != -1 && i != sfd)
        {
            write(connfd[i], buf, strlen(buf) + 1);
        }
    }
    strcpy(temp, "Sent successfully\n");
    write(connfd[sfd], temp, strlen(temp) + 1);
}

/*获取所有在线用户信息*/
void get_online_users(int sfd)
{
    int i;
    char buf[BUFFSIZE], nowtime[20];
    time_t now;
    time(&now);
    struct tm *tempTime = localtime(&now);
    strftime(nowtime, 20, "[%H:%M:%S]", tempTime);
    strcpy(buf, nowtime);
    strcat(buf, "\t");
    strcat(buf, "All online user(s):\n");
    for (i = 0; i < MAXMEM; i++)
    {
        if (online_users[i].status == 0)
        {
            strcat(buf, "\t");
            strcat(buf, online_users[i].username);
            strcat(buf, "\n");
        }
    }
    write(connfd[sfd], buf, strlen(buf) + 1);
}

/*向聊天室发送信息*/
void send_chatroom_msg(char *msg, int sfd)
{
    int i, j, k;
    int flag;
    flag = -1;
    char buf[BUFFSIZE], nowtime[20];
    time_t now;
    time(&now);
    struct tm *tempTime = localtime(&now);
    strftime(nowtime, 20, "[%H:%M:%S]", tempTime);
    for (i = 0; i < MAXROOM; i++)
    {
        if (chatrooms[i].status == 0)
        {
            for (j = 0; j < 10; j++)
            {
                if (chatrooms[i].user[j] == sfd)
                {
                    flag = 0;
                    break;
                }
            }
        }
        if (flag == 0)
        {
            break;
        }
    }
    if (flag == -1)
    {
        strcpy(buf, "You have not joined the chat room.\n");
        write(connfd[sfd], buf, strlen(buf) + 1);
    }
    else
    {

        for (k = 0; k < MAXMEM; k++)
        {
            if (online_users[k].status == 0 && online_users[k].socketfd == sfd)
                break;
        }
        strcpy(buf, nowtime);
        strcat(buf, "\tchatroom ");
        strcat(buf, chatrooms[i].name);
        strcat(buf, ":\nfrom ");
        strcat(buf, online_users[k].username);
        strcat(buf, ":\t");
        strcat(buf, msg);
        strcat(buf, "\n");
        for (k = 0; k < 10; k++)
        {
            if (chatrooms[i].user[k] != -1)
            {
                write(connfd[chatrooms[i].user[k]], buf, strlen(buf) + 1);
            }
        }
    }
}

/*创建聊天室*/
void create_chatroom(char *name, char *passwd, int sfd)
{
    int i, j;
    char buf[BUFFSIZE];
    for (i = 0; i < MAXROOM; i++)
    {
        if (chatrooms[i].status == -1)
            break;
    }
    strcpy(chatrooms[i].name, name);
    strcpy(chatrooms[i].passwd, passwd);
    chatrooms[i].status = 0;
    for (j = 0; j < 10; j++)
    {
        if (chatrooms[i].user[j] == -1)
            break;
    }
    chatrooms[i].user[j] = sfd;
    strcpy(buf, "Successfully created chat room.\n");
    write(connfd[sfd], buf, strlen(buf) + 1);
}

/*加入聊天室*/
void join_chatroom(char *name, char *passwd, int sfd)
{
    int i, j;
    int room, flag;
    char buf[BUFFSIZE];
    flag = -1;
    for (i = 0; i < MAXROOM; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (chatrooms[i].user[j] == sfd)
            {
                room = i;
                flag = 0;
            }
        }
    }
    if (flag == 0)
    {
        strcpy(buf, "You have joined the chat room ");
        strcat(buf, chatrooms[room].name);
        strcat(buf, ".\n");
        write(connfd[sfd], buf, strlen(buf) + 1);
    }
    else
    {
        for (i = 0; i < MAXROOM; i++)
        {
            if (chatrooms[i].status != -1)
            {
                if (strcmp(chatrooms[i].name, name) == 0)
                {
                    if (strcmp(chatrooms[i].passwd, passwd) == 0)
                    {
                        for (j = 0; j < 10; j++)
                        {
                            if (chatrooms[i].user[j] == -1)
                            {
                                break;
                            }
                        }
                        chatrooms[i].user[j] = sfd;
                        strcpy(buf, "Successfully joined the chat room.\n");
                        write(connfd[sfd], buf, strlen(buf) + 1);
                        return;
                    }
                    else
                    {
                        strcpy(buf, "Incorrect chat room password.\n");
                        write(connfd[sfd], buf, strlen(buf) + 1);
                        return;
                    }
                }
            }
        }
    }
}

/*获取所有已创建的聊天室的信息*/
void get_online_chatrooms(int sfd)
{
    int i;
    char buf[BUFFSIZE], nowtime[20];
    time_t now;
    time(&now);
    struct tm *tempTime = localtime(&now);
    strftime(nowtime, 20, "[%H:%M:%S]", tempTime);
    strcpy(buf, nowtime);
    strcat(buf, "\tAll online chat room(s):\n");
    for (i = 0; i < MAXROOM; i++)
    {
        if (chatrooms[i].status == 0)
        {
            strcat(buf, "\t");
            strcat(buf, chatrooms[i].name);
            strcat(buf, "\n");
        }
    }
    write(connfd[sfd], buf, strlen(buf) + 1);
}

/*修改密码*/
void change_passwd(int sfd, char *passwd)
{
    int i, j;
    char buf[BUFFSIZE], name[20];
    for (j = 0; j < MAXMEM; j++)
    {
        if (sfd == online_users[j].socketfd)
        {
            strcpy(name, online_users[j].username);
            break;
        }
    }
    for (i = 0; i < MAXMEM; i++)
    {
        if (strcmp(name, users[i].username) == 0)
        {
            strcpy(users[i].password, passwd);
            strcpy(buf, "Password has been updated.\n");
            write(connfd[sfd], buf, strlen(buf) + 1);
            break;
        }
    }
}

/*查询所有加入某聊天室的用户*/
void get_inroom_users(int sfd)
{
    int i, j;
    int room, flag; //room记录查询查询发起人所在的房间，flag标识用户是否加入房间
    flag = -1;
    char buf[BUFFSIZE], nowtime[20];
    time_t now;
    time(&now);
    struct tm *tempTime = localtime(&now);
    strftime(nowtime, 20, "[%H:%M:%S]", tempTime);
    for (i = 0; i < MAXROOM; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (chatrooms[i].user[j] == sfd)
            {
                room = i;
                flag = 0;
            }
        }
    }
    if (flag == -1)
    {
        strcpy(buf, "Sorry, you have not joined the chat room.\n");
        write(connfd[sfd], buf, strlen(buf) + 1);
    }
    else
    {
        strcpy(buf, nowtime);
        strcat(buf, "\tAll users in the ");
        strcat(buf, chatrooms[room].name);
        strcat(buf, ":\n");
        for (i = 0; i < 10; i++)
        {
            if (chatrooms[room].user[i] >= 0)
                for (j = 0; j < MAXMEM; j++)
                {
                    if (online_users[j].status != -1 && (chatrooms[room].user[i] == online_users[j].socketfd))
                    {
                        strcat(buf, "\t");
                        strcat(buf, online_users[j].username);
                        strcat(buf, "\n");
                    }
                }
        }
        write(connfd[sfd], buf, strlen(buf) + 1);
    }
}

/*退出聊天室*/
void exit_chatroom(int sfd)
{
    int i, j;
    int room, flag;
    flag = -1;
    char buf[BUFFSIZE];
    for (i = 0; i < MAXROOM; i++)
    {
        if (chatrooms[i].status == 0)
        {
            for (j = 0; j < 10; j++)
            {
                if (chatrooms[i].user[j] == sfd)
                {
                    chatrooms[i].user[j] = -1;
                    room = i;
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
        strcpy(buf, "You have not joined the chat room.\n");
        write(connfd[sfd], buf, strlen(buf) + 1);
    }
    else
    {
        strcpy(buf, "Successfully quit chat room ");
        strcat(buf, chatrooms[room].name);
        strcat(buf, ".\n");
        write(connfd[sfd], buf, strlen(buf) + 1);
    }
}

/*服务器推出*/
void quit()
{
    int i;
    char msg[10];
    while (1)
    {
        scanf("%s", msg); // scanf 不同于fgets, 它不会读入最后输入的换行符
        if (strcmp(msg, "quit") == 0)
        {
            save_users();
            printf("Byebye... \n");
            close(listenfd);
            exit(0);
        }
    }
}

/*用户输入无效命令*/
void invalid_command(int sfd)
{
    char buf[BUFFSIZE];
    strcpy(buf, "Invalid command.\n");
    write(connfd[sfd], buf, strlen(buf) + 1);
}


