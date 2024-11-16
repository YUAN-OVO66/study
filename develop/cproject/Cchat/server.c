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

    // ����һ���̣߳��Է�����������й�������quit����
    pthread_create(&thread, NULL, (void *)(quit), NULL);

    // ���׽��������������ʼ��Ϊ-1����ʾ����
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
        // accept ��listen���ܵ����Ӷ�����ȡ��һ������
        connfd[i] = accept(listenfd, (struct sockaddr *)&cli_addr, &len);
        if (connfd[i] < 0)
        {
            perror("fail to accept.");
        }
        timenow = time(NULL);
        printf("%.24s\n\tconnect from: %s, port %d\n",
               ctime(&timenow), inet_ntop(AF_INET, &(cli_addr.sin_addr), buff, BUFFSIZE),
               ntohs(cli_addr.sin_port));

        // ��Ե�ǰ�׽��ִ���һ���̣߳��Ե�ǰ�׽��ֵ���Ϣ���д���
        pthread_create(malloc(sizeof(pthread_t)), NULL, (void *)(&rcv_snd), (void *)i);
    }
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

    while (1)
    {
        len = read(connfd[n], buf, BUFFSIZE);
        if (len > 0)
        {
            buf[len - 1] = '\0'; // ȥ�����з�

            if (strcmp(buf, "login") == 0)
            {
                //��¼�ɹ�ʱ�˳���ѭ��
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
            sscanf(temp, "%s %s %[^\n]", command, arg1, arg2); //��������

            /*���ݽ�����������ִ�в�ͬ�ĺ���*/
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

/*��ʼ��*/
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
    //���ļ��ж�ȡ�û�
    while (fscanf(fp, "%s", buf) != EOF)
    {
        strcpy(users[user_count].username, buf);
        fscanf(fp, "%s", buf);
        strcpy(users[user_count].password, buf);
        user_count++;
    }
    fclose(fp);
}

/*���û����浽�ļ�*/
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

/*�����������û��˳�*/
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

/*�û���¼*/
int user_login(int n)
{
    int len, i, j;
    char buf[BUFFSIZE], username[20], password[20];
    sprintf(buf, "your username: ");
    write(connfd[n], buf, strlen(buf) + 1);
    len = read(connfd[n], username, 20);
    if (len > 0)
    {
        username[len - 1] = '\0'; // ȥ�����з�
    }
    sprintf(buf, "your password: ");
    write(connfd[n], buf, strlen(buf) + 1);
    len = read(connfd[n], password, 20);
    if (len > 0)
    {
        password[len - 1] = '\0'; // ȥ�����з�
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

/*�û�ע��*/
void register_user(int n)
{
    int len, i;
    char buf[BUFFSIZE], username[20], password[20];
    sprintf(buf, "your username: ");
    write(connfd[n], buf, strlen(buf) + 1);
    len = read(connfd[n], username, 20);
    if (len > 0)
    {
        username[len - 1] = '\0'; // ȥ�����з�
    }
    sprintf(buf, "your password: ");
    write(connfd[n], buf, strlen(buf) + 1);
    len = read(connfd[n], password, 20);
    if (len > 0)
    {
        password[len - 1] = '\0'; // ȥ�����з�
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

/*�û�����˽����Ϣ*/
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

/*�û�Ⱥ����Ϣ�������û�*/
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

/*��ȡ���������û���Ϣ*/
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

/*�������ҷ�����Ϣ*/
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

/*����������*/
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

/*����������*/
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

/*��ȡ�����Ѵ����������ҵ���Ϣ*/
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

/*�޸�����*/
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

/*��ѯ���м���ĳ�����ҵ��û�*/
void get_inroom_users(int sfd)
{
    int i, j;
    int room, flag; //room��¼��ѯ��ѯ���������ڵķ��䣬flag��ʶ�û��Ƿ���뷿��
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

/*�˳�������*/
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

/*�������Ƴ�*/
void quit()
{
    int i;
    char msg[10];
    while (1)
    {
        scanf("%s", msg); // scanf ��ͬ��fgets, ����������������Ļ��з�
        if (strcmp(msg, "quit") == 0)
        {
            save_users();
            printf("Byebye... \n");
            close(listenfd);
            exit(0);
        }
    }
}

/*�û�������Ч����*/
void invalid_command(int sfd)
{
    char buf[BUFFSIZE];
    strcpy(buf, "Invalid command.\n");
    write(connfd[sfd], buf, strlen(buf) + 1);
}


