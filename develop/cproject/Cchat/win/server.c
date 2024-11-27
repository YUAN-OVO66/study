#include "server.h"

int main()
{
    // Winsock 初始化
    WSADATA ljlWsaData;
    if (WSAStartup(MAKEWORD(2, 2), &ljlWsaData) != 0)
    {
        perror("WSAStartup failed");
        exit(-1);
    }

    ljlInit();

    struct sockaddr_in ljlServAddr, ljlCliAddr;
    int ljlI;
    time_t ljlTimeNow;
    pthread_t ljlThread;
    char ljlBuff[ljlBuffSize];

    printf("Running...\nEnter command \"quit\" to exit server.\n\n");

    // 清空服务器地址结构
    memset(&ljlServAddr, 0, sizeof(struct sockaddr_in));
    ljlServAddr.sin_family = AF_INET;
    ljlServAddr.sin_port = htons(ljlPort);
    ljlServAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 创建监听套接字
    ljlListenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (ljlListenfd == INVALID_SOCKET)
    {
        perror("fail to socket");
        WSACleanup();
        exit(-1);
    }

    // 绑定套接字
    if (bind(ljlListenfd, (struct sockaddr *)&ljlServAddr, sizeof(ljlServAddr)) == SOCKET_ERROR)
    {
        perror("fail to bind");
        closesocket(ljlListenfd);
        WSACleanup();
        exit(-2);
    }

    // 开始监听
    if (listen(ljlListenfd, ljlMaxMem) == SOCKET_ERROR)
    {
        perror("fail to listen");
        closesocket(ljlListenfd);
        WSACleanup();
        exit(-3);
    }

    // 创建一个线程，用于管理服务器，调用 quit 函数
    pthread_create(&ljlThread, NULL, (void *)(ljlQuit), NULL);

    // 初始化连接套接字数组
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
        ljlConnfd[ljlI] = INVALID_SOCKET;

    // 接收客户端连接
    while (1)
    {
        int ljlLen;
        for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
        {
            if (ljlConnfd[ljlI] == INVALID_SOCKET)
                break;
        }

        // 接受连接
        ljlConnfd[ljlI] = accept(ljlListenfd, (struct sockaddr *)&ljlCliAddr, &ljlLen);
        if (ljlConnfd[ljlI] == INVALID_SOCKET)
        {
            perror("fail to accept.");
            continue;
        }

        ljlTimeNow = time(NULL);
        printf("%.24s\n\tconnect from: %s, port %d\n",
               ctime(&ljlTimeNow), inet_ntoa(ljlCliAddr.sin_addr),
               ntohs(ljlCliAddr.sin_port));

        // 针对当前套接字创建一个线程来处理消息
        pthread_t ljlRecvThread;
        pthread_create(&ljlRecvThread, NULL, (void *)(&ljlRcvSnd), (void *)ljlI);
    }

}



/*服务器接收和发送函数*/
void ljlRcvSnd(int ljlN)
{
    ssize_t ljlLen;
    int ljlI;
    char ljlMyTime[32], ljlBuff[ljlBuffSize];
    char ljlTemp[ljlBuffSize];
    char ljlCommand[20], ljlArg1[20], ljlArg2[ljlBuffSize];
    time_t ljlTimeNow;

    // 第一个循环，用于接收和处理登录或注册命令
    while (1)
    {
        ljlLen = recv(ljlConnfd[ljlN], ljlBuff, ljlBuffSize, 0);
        if (ljlLen > 0)
        {
            ljlBuff[ljlLen - 1] = '\0'; // 去除换行符

            // 处理登录请求
            if (strcmp(ljlBuff, "login") == 0)
            {
                // 登录成功时退出该循环
                if (ljlUserLogin(ljlN) == 0)
                {
                    break;
                }
            }
                // 处理注册请求
            else if (strcmp(ljlBuff, "register") == 0)
            {
                ljlRegisterUser(ljlN);
            }
                // 处理退出请求
            else if (strcmp(ljlBuff, "quit") == 0)
            {
                ljlQuitClient(ljlN);
            }
        }
    }

    // 第二个循环，用于处理登录后的命令
    while (1)
    {
        if ((ljlLen = recv(ljlConnfd[ljlN], ljlTemp, ljlBuffSize, 0)) > 0)
        {
            ljlTemp[ljlLen - 1] = '\0'; // 去除换行符
            sscanf(ljlTemp, "%s %s %[^\n]", ljlCommand, ljlArg1, ljlArg2); // 解析命令

            // 处理发送消息的命令
            if (strcmp(ljlCommand, "send") == 0 && strcmp(ljlArg1, "-all") == 0)
            {
                ljlSendAllMsg(ljlArg2, ljlN);  // 向所有在线用户发送消息
            }
            else if (strcmp(ljlCommand, "send") == 0 && strcmp(ljlArg1, "-chatroom") == 0)
            {
                ljlSendChatroomMsg(ljlArg2, ljlN);  // 向聊天室中的所有用户发送消息
            }
            else if (strcmp(ljlCommand, "send") == 0)
            {
                ljlSendPrivateMsg(ljlArg1, ljlArg2, ljlN);  // 向指定用户发送私聊消息
            }
                // 处理退出命令
            else if (strcmp(ljlCommand, "quit") == 0)
            {
                ljlQuitClient(ljlN);
            }
                // 处理修改密码命令
            else if (strcmp(ljlCommand, "chgpsw") == 0)
            {
                ljlChangePasswd(ljlN, ljlArg1);
            }
                // 处理创建聊天室命令
            else if (strcmp(ljlCommand, "create") == 0)
            {
                ljlCreateChatroom(ljlArg1, ljlArg2, ljlN);
            }
                // 处理加入聊天室命令
            else if (strcmp(ljlCommand, "join") == 0)
            {
                ljlJoinChatroom(ljlArg1, ljlArg2, ljlN);
            }
                // 处理列出聊天室命令
            else if (strcmp(ljlCommand, "ls") == 0 && strcmp(ljlArg1, "-chatrooms") == 0)
            {
                ljlGetOnlineChatrooms(ljlN);
            }
                // 处理列出在线用户命令
            else if (strcmp(ljlCommand, "ls") == 0 && strcmp(ljlArg1, "-users") == 0)
            {
                ljlGetOnlineUsers(ljlN);
            }
                // 处理列出聊天室内用户命令
            else if (strcmp(ljlCommand, "ls") == 0 && strcmp(ljlArg1, "-inrmusr") == 0)
            {
                ljlGetInroomUsers(ljlN);
            }
                // 处理退出聊天室命令
            else if (strcmp(ljlCommand, "exit") == 0)
            {
                ljlExitChatroom(ljlN);
            }
                // 处理无效命令
            else
            {
                ljlInvalidCommand(ljlN);
            }
        }
    }
}


//初始化
void ljlInit()
{
    int ljlI, ljlJ;
    ljlUserCount = 0;
    ljlChatroomCount = 0;

    // 初始化在线用户状态
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
    {
        ljlOnlineUsers[ljlI].ljlStatus = -1; // -1 表示离线
    }

    // 初始化聊天室状态
    for (ljlI = 0; ljlI < ljlMaxRoom; ljlI++)
    {
        ljlChatrooms[ljlI].ljlStatus = -1; // -1 表示聊天室不存在
        for (ljlJ = 0; ljlJ < 10; ljlJ++)
        {
            ljlChatrooms[ljlI].ljlUser[ljlJ] = -1; // 初始化聊天室中的用户为 -1（无用户）
        }
    }

    // 打开文件并从文件读取用户数据
    char ljlBuf[20];
    FILE *ljlFp = NULL;
    ljlFp = fopen("users.txt", "r");
    if (ljlFp == NULL)
    {
        printf("Error opening users.txt file.\n");
        return;
    }

    // 从文件中读取用户名和密码并存储
    while (fscanf(ljlFp, "%s", ljlBuf) != EOF)
    {
        strcpy(ljlUsers[ljlUserCount].ljlUsername, ljlBuf);  // 读取用户名
        fscanf(ljlFp, "%s", ljlBuf);
        strcpy(ljlUsers[ljlUserCount].ljlPassword, ljlBuf);  // 读取密码
        ljlUserCount++;  // 增加用户数量
    }

    // 关闭文件
    fclose(ljlFp);
}


/*将用户保存到文件*/
void ljlSaveUsers()
{
    int ljlI;
    FILE *ljlFp = NULL;

    // 打开文件以进行写操作，如果文件不存在则创建
    ljlFp = fopen("users.txt", "w+");
    if (ljlFp == NULL)  // 检查文件是否成功打开
    {
        printf("Error opening users.txt file.\n");
        return;
    }

    // 遍历用户数组，写入每个用户的用户名和密码
    for (ljlI = 0; ljlI < ljlUserCount; ljlI++)
    {
        // 使用 fprintf 直接将用户名和密码写入文件
        fprintf(ljlFp, "%s\n", ljlUsers[ljlI].ljlUsername);
        fprintf(ljlFp, "%s\n", ljlUsers[ljlI].ljlPassword);
    }

    // 关闭文件
    fclose(ljlFp);
}


//退出客户端
void ljlQuitClient(int ljlN)
{
    int ljlI;

    // 确保套接字连接有效
    if (ljlConnfd[ljlN] != -1)
    {
        closesocket(ljlConnfd[ljlN]);  // 关闭套接字连接
        ljlConnfd[ljlN] = -1;    // 更新连接状态，标记该位置为空
    }

    // 更新在线用户状态
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
    {
        if (ljlOnlineUsers[ljlI].ljlSocketfd == ljlConnfd[ljlN])
        {
            ljlOnlineUsers[ljlI].ljlStatus = -1;  // 标记用户下线
            break;  // 找到用户并更新后跳出循环
        }
    }

    // 退出线程，若不需要返回退出状态，直接调用pthread_exit()
    pthread_exit(NULL);
}

//用户登录
int ljlUserLogin(int ljlN)
{
    int ljlLen, ljlI, ljlJ;
    char ljlBuf[ljlBuffSize], ljlUsername[20], ljlPassword[20];

    // 提示用户输入用户名
    sprintf(ljlBuf, "Your username: ");
    send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);

    // 读取用户名
    ljlLen = recv(ljlConnfd[ljlN], ljlUsername, 20, 0);
    if (ljlLen > 0)
    {
        ljlUsername[ljlLen - 1] = '\0'; // 去除换行符
    }

    // 提示用户输入密码
    sprintf(ljlBuf, "Your password: ");
    send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);

    // 读取密码
    ljlLen = recv(ljlConnfd[ljlN], ljlPassword, 20, 0);
    if (ljlLen > 0)
    {
        ljlPassword[ljlLen - 1] = '\0'; // 去除换行符
    }

    // 查找用户名并验证密码
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
    {
        if (strcmp(ljlUsername, ljlUsers[ljlI].ljlUsername) == 0)
        {
            // 用户名匹配，验证密码
            if (strcmp(ljlPassword, ljlUsers[ljlI].ljlPassword) == 0)
            {
                sprintf(ljlBuf, "Login successfully.\n\n");
                send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);

                // 查找第一个空闲的位置并将用户标记为在线
                for (ljlJ = 0; ljlJ < ljlMaxMem; ljlJ++)
                {
                    if (ljlOnlineUsers[ljlJ].ljlStatus == -1)
                    {
                        strcpy(ljlOnlineUsers[ljlJ].ljlUsername, ljlUsername);
                        ljlOnlineUsers[ljlJ].ljlSocketfd = ljlN;
                        ljlOnlineUsers[ljlJ].ljlStatus = 0;
                        break;  // 找到空闲位置后退出
                    }
                }
                return 0;  // 登录成功
            }
            else
            {
                // 密码错误
                sprintf(ljlBuf, "Wrong password.\n\n");
                send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);
                return -1;  // 密码错误
            }
        }
    }

    // 用户名不存在
    sprintf(ljlBuf, "Account does not exist.\n\n");
    send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);
    return -1;  // 用户名不存在
}

//用户注册
void ljlRegisterUser(int ljlN)
{
    int ljlLen, ljlI;
    char ljlBuf[ljlBuffSize], ljlUsername[20], ljlPassword[20];

    // 提示用户输入用户名
    sprintf(ljlBuf, "Your username: ");
    send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);

    // 读取用户名
    ljlLen = recv(ljlConnfd[ljlN], ljlUsername, 20, 0);
    if (ljlLen > 0)
    {
        ljlUsername[ljlLen - 1] = '\0'; // 去除换行符
    }

    // 提示用户输入密码
    sprintf(ljlBuf, "Your password: ");
    send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);

    // 读取密码
    ljlLen = recv(ljlConnfd[ljlN], ljlPassword, 20, 0);
    if (ljlLen > 0)
    {
        ljlPassword[ljlLen - 1] = '\0'; // 去除换行符
    }

    // 检查用户名是否已经存在
    for (ljlI = 0; ljlI < ljlUserCount; ljlI++)
    {
        if (strcmp(ljlUsers[ljlI].ljlUsername, ljlUsername) == 0)
        {
            sprintf(ljlBuf, "The username already exists.\n\n");
            send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);
            return;
        }
    }

    // 如果用户数量已满，无法注册
    if (ljlUserCount >= ljlMaxMem)
    {
        sprintf(ljlBuf, "User registration limit reached.\n\n");
        send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);
        return;
    }

    // 保存新用户信息
    strcpy(ljlUsers[ljlUserCount].ljlUsername, ljlUsername);
    strcpy(ljlUsers[ljlUserCount].ljlPassword, ljlPassword);
    ljlUserCount++;

    // 返回注册成功信息
    sprintf(ljlBuf, "Account created successfully.\n\n");
    send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);
}

/*用户发送私聊信息*/
void ljlSendPrivateMsg(char *ljlUsername, char *ljlData, SOCKET ljlSfd)
{
    int ljlI, ljlJ;
    time_t ljlNow;
    char ljlSendMan[20];
    char ljlBuf[ljlBuffSize], ljlNowtime[20], ljlTemp[30];
    ljlNow = time(NULL);  // 获取当前时间
    struct tm *ljlTempTime = localtime(&ljlNow);
    strftime(ljlNowtime, sizeof(ljlNowtime), "[%H:%M:%S]", ljlTempTime);  // 格式化时间

    // 查找发送者的用户名
    for (ljlJ = 0; ljlJ < ljlMaxMem; ljlJ++)
    {
        if (ljlSfd == ljlOnlineUsers[ljlJ].ljlSocketfd)
        {
            strcpy(ljlSendMan, ljlOnlineUsers[ljlJ].ljlUsername);
            break;
        }
    }

    // 查找目标用户并发送消息
    int ljlFound = 0;  // 标记是否找到目标用户
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
    {
        if (strcmp(ljlUsername, ljlOnlineUsers[ljlI].ljlUsername) == 0)
        {
            snprintf(ljlBuf, sizeof(ljlBuf), "%s\tfrom %s:\n%s\n", ljlNowtime, ljlSendMan, ljlData);
            send(ljlConnfd[ljlOnlineUsers[ljlI].ljlSocketfd], ljlBuf, strlen(ljlBuf) + 1, 0);  // 发送私聊消息
            strcpy(ljlTemp, "Sent successfully.\n");
            send(ljlConnfd[ljlSfd], ljlTemp, strlen(ljlTemp) + 1, 0);  // 发送成功回执
            ljlFound = 1;
            break;
        }
    }

    // 如果未找到目标用户
    if (!ljlFound)
    {
        strcpy(ljlBuf, "User is not online or user does not exist.\n");
        send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);  // 返回错误消息给发送者
    }
}


//消息群发
void ljlSendAllMsg(char *ljlMsg, SOCKET ljlSfd)
{
    int ljlI;
    char ljlBuf[ljlBuffSize], ljlNowtime[20], ljlSendMan[20], ljlTemp[30];
    time_t ljlNow;
    ljlNow = time(NULL);  // 获取当前时间
    struct tm *ljlTempTime = localtime(&ljlNow);
    strftime(ljlNowtime, sizeof(ljlNowtime), "[%H:%M:%S]", ljlTempTime);  // 格式化时间

    // 查找发送者的用户名
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
    {
        if (ljlSfd == ljlOnlineUsers[ljlI].ljlSocketfd)
        {
            strcpy(ljlSendMan, ljlOnlineUsers[ljlI].ljlUsername);
            break;
        }
    }

    // 构建消息内容
    snprintf(ljlBuf, sizeof(ljlBuf), "%s\tfrom %s(goup-sent):\n%s\n", ljlNowtime, ljlSendMan, ljlMsg);

    // 群发消息给所有在线用户
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
    {
        if (ljlConnfd[ljlI] != -1 && ljlI != ljlSfd)  // 确保连接有效且不是发送者自己
        {
            send(ljlConnfd[ljlI], ljlBuf, strlen(ljlBuf) + 1, 0);  // 发送消息
        }
    }

    // 向发送者返回发送成功的回执
    snprintf(ljlTemp, sizeof(ljlTemp), "Sent successfully\n");
    send(ljlConnfd[ljlSfd], ljlTemp, strlen(ljlTemp) + 1, 0);
}

//获取在线用户
void ljlGetOnlineUsers(SOCKET ljlSfd)
{
    int ljlI;
    char ljlBuf[ljlBuffSize], ljlNowtime[20];
    time_t ljlNow;
    ljlNow = time(NULL);  // 获取当前时间
    struct tm *ljlTempTime = localtime(&ljlNow);
    strftime(ljlNowtime, sizeof(ljlNowtime), "[%H:%M:%S]", ljlTempTime);  // 格式化时间

    // 构建返回的消息内容
    snprintf(ljlBuf, sizeof(ljlBuf), "%s\tAll online user(s):\n", ljlNowtime);

    // 添加每个在线用户的用户名
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
    {
        if (ljlOnlineUsers[ljlI].ljlStatus == 0)  // 只列出在线用户
        {
            snprintf(ljlBuf + strlen(ljlBuf), sizeof(ljlBuf) - strlen(ljlBuf), "\t%s\n", ljlOnlineUsers[ljlI].ljlUsername);
        }
    }

    // 发送给请求的客户端
    send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
}

//发送消息到聊天室
void ljlSendChatroomMsg(char *ljlMsg, SOCKET ljlSfd)
{
    int ljlI, ljlJ, ljlK;
    int ljlFlag = -1;
    char ljlBuf[ljlBuffSize], ljlNowtime[20];
    time_t ljlNow;
    ljlNow = time(NULL);
    struct tm *ljlTempTime = localtime(&ljlNow);
    strftime(ljlNowtime, sizeof(ljlNowtime), "[%H:%M:%S]", ljlTempTime);  // 格式化当前时间

    // 检查用户是否加入了聊天室
    for (ljlI = 0; ljlI < ljlMaxRoom; ljlI++)
    {
        if (ljlChatrooms[ljlI].ljlStatus == 0)  // 仅检查有效的聊天室
        {
            for (ljlJ = 0; ljlJ < 10; ljlJ++)
            {
                if (ljlChatrooms[ljlI].ljlUser[ljlJ] == ljlSfd)  // 检查是否是聊天室的成员
                {
                    ljlFlag = 0;
                    break;
                }
            }
        }
        if (ljlFlag == 0)
            break;
    }

    if (ljlFlag == -1)
    {
        // 用户未加入聊天室
        strcpy(ljlBuf, "You have not joined the chat room.\n");
        send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
    }
    else
    {
        // 获取发送消息的用户名
        for (ljlK = 0; ljlK < ljlMaxMem; ljlK++)
        {
            if (ljlOnlineUsers[ljlK].ljlSocketfd == ljlSfd)  // 找到发送消息的用户
                break;
        }

        // 构建消息内容
        snprintf(ljlBuf, sizeof(ljlBuf), "%s\tchatroom %s:\nfrom %s:\t%s\n",
                 ljlNowtime, ljlChatrooms[ljlI].ljlName, ljlOnlineUsers[ljlK].ljlUsername, ljlMsg);

        // 向聊天室内的所有成员发送消息
        for (ljlK = 0; ljlK < 10; ljlK++)
        {
            if (ljlChatrooms[ljlI].ljlUser[ljlK] != -1)
            {
                send(ljlConnfd[ljlChatrooms[ljlI].ljlUser[ljlK]], ljlBuf, strlen(ljlBuf) + 1, 0);
            }
        }
    }
}


//创建聊天室
void ljlCreateChatroom(char *ljlName, char *ljlPasswd, SOCKET ljlSfd)
{
    int ljlI, ljlJ;
    char ljlBuf[ljlBuffSize];

    // 查找第一个空闲的聊天室位置
    for (ljlI = 0; ljlI < ljlMaxRoom; ljlI++)
    {
        if (ljlChatrooms[ljlI].ljlStatus == -1)  // 找到未使用的聊天室
            break;
    }

    if (ljlI == ljlMaxRoom)
    {
        // 如果没有空闲的聊天室，返回错误信息
        strcpy(ljlBuf, "No available rooms to create.\n");
        send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
        return;
    }

    // 安全地拷贝聊天室名称和密码，避免溢出
    snprintf(ljlChatrooms[ljlI].ljlName, sizeof(ljlChatrooms[ljlI].ljlName), "%s", ljlName);
    snprintf(ljlChatrooms[ljlI].ljlPasswd, sizeof(ljlChatrooms[ljlI].ljlPasswd), "%s", ljlPasswd);
    ljlChatrooms[ljlI].ljlStatus = 0;  // 将聊天室状态设置为“已创建”

    // 将用户加入聊天室
    for (ljlJ = 0; ljlJ < 10; ljlJ++)
    {
        if (ljlChatrooms[ljlI].ljlUser[ljlJ] == -1)  // 找到空位
        {
            ljlChatrooms[ljlI].ljlUser[ljlJ] = ljlSfd;  // 将用户加入聊天室
            break;
        }
    }

    // 通知用户聊天室创建成功
    snprintf(ljlBuf, sizeof(ljlBuf), "Successfully created chat room '%s'.\n", ljlChatrooms[ljlI].ljlName);
    send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
}

//加入聊天室
void ljlJoinChatroom(char *ljlName, char *ljlPasswd, SOCKET ljlSfd)
{
    int ljlI, ljlJ;
    char ljlBuf[ljlBuffSize];
    int ljlRoomFound = -1;

    // 检查用户是否已经在某个聊天室
    for (ljlI = 0; ljlI < ljlMaxRoom; ljlI++)
    {
        for (ljlJ = 0; ljlJ < 10; ljlJ++)
        {
            if (ljlChatrooms[ljlI].ljlUser[ljlJ] == ljlSfd)
            {
                ljlRoomFound = ljlI;
                break;
            }
        }
        if (ljlRoomFound != -1)
            break;  // 用户已经在某个聊天室中
    }

    if (ljlRoomFound != -1)
    {
        snprintf(ljlBuf, sizeof(ljlBuf), "You have already joined the chat room '%s'.\n", ljlChatrooms[ljlRoomFound].ljlName);
        send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
        return;  // 用户已经加入过聊天室，直接返回
    }

    // 查找目标聊天室并加入
    for (ljlI = 0; ljlI < ljlMaxRoom; ljlI++)
    {
        if (ljlChatrooms[ljlI].ljlStatus != -1 && strcmp(ljlChatrooms[ljlI].ljlName, ljlName) == 0)  // 找到聊天室
        {
            if (strcmp(ljlChatrooms[ljlI].ljlPasswd, ljlPasswd) == 0)  // 密码正确
            {
                // 查找空位置加入聊天室
                for (ljlJ = 0; ljlJ < 10; ljlJ++)
                {
                    if (ljlChatrooms[ljlI].ljlUser[ljlJ] == -1)  // 找到空位
                    {
                        ljlChatrooms[ljlI].ljlUser[ljlJ] = ljlSfd;
                        snprintf(ljlBuf, sizeof(ljlBuf), "Successfully joined the chat room '%s'.\n", ljlChatrooms[ljlI].ljlName);
                        send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
                        return;  // 成功加入聊天室
                    }
                }
                // 如果聊天室已满
                snprintf(ljlBuf, sizeof(ljlBuf), "Chat room '%s' is full.\n", ljlChatrooms[ljlI].ljlName);
                send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
                return;
            }
            else
            {
                // 密码错误
                snprintf(ljlBuf, sizeof(ljlBuf), "Incorrect password for chat room '%s'.\n", ljlName);
                send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
                return;
            }
        }
    }

    // 如果没有找到该聊天室
    snprintf(ljlBuf, sizeof(ljlBuf), "Chat room '%s' does not exist.\n", ljlName);
    send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
}

//获取聊天室信息
void ljlGetOnlineChatrooms(SOCKET ljlSfd)
{
    int ljlI;
    char ljlBuf[ljlBuffSize];
    time_t ljlNow = time(NULL);
    struct tm *ljlTempTime = localtime(&ljlNow);

    // 获取当前时间
    snprintf(ljlBuf, sizeof(ljlBuf), "[%02d:%02d:%02d]\tAll online chat room(s):\n",
             ljlTempTime->tm_hour, ljlTempTime->tm_min, ljlTempTime->tm_sec);

    // 获取所有在线聊天室
    for (ljlI = 0; ljlI < ljlMaxRoom; ljlI++)
    {
        if (ljlChatrooms[ljlI].ljlStatus == 0)  // 判断聊天室是否在线
        {
            snprintf(ljlBuf + strlen(ljlBuf), sizeof(ljlBuf) - strlen(ljlBuf), "\t%s\n", ljlChatrooms[ljlI].ljlName);
        }
    }

    send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
}


// 修改密码
void ljlChangePasswd(SOCKET ljlSfd, char *ljlPasswd)
{
    int ljlI;
    char ljlBuf[ljlBuffSize], ljlName[20];

    // 获取当前用户的用户名
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
    {
        if (ljlSfd == ljlOnlineUsers[ljlI].ljlSocketfd)
        {
            strncpy(ljlName, ljlOnlineUsers[ljlI].ljlUsername, sizeof(ljlName) - 1);
            ljlName[sizeof(ljlName) - 1] = '\0';  // 确保字符串终止
            break;
        }
    }

    // 查找并更新密码
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
    {
        if (strcmp(ljlName, ljlUsers[ljlI].ljlUsername) == 0)
        {
            strncpy(ljlUsers[ljlI].ljlPassword, ljlPasswd, sizeof(ljlUsers[ljlI].ljlPassword) - 1);
            ljlUsers[ljlI].ljlPassword[sizeof(ljlUsers[ljlI].ljlPassword) - 1] = '\0';  // 确保字符串终止
            snprintf(ljlBuf, sizeof(ljlBuf), "Password has been updated for user: %s\n", ljlName);
            send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
            break;
        }
    }
}

/* 查询所有加入某聊天室的用户 */
void ljlGetInroomUsers(SOCKET ljlSfd)
{
    int ljlI, ljlJ;
    int ljlRoom = -1;
    char ljlBuf[ljlBuffSize], ljlNowtime[20];
    time_t ljlNow;
    time(&ljlNow);
    struct tm *ljlTempTime = localtime(&ljlNow);
    strftime(ljlNowtime, 20, "[%H:%M:%S]", ljlTempTime);

    // 查找用户所在的聊天室
    for (ljlI = 0; ljlI < ljlMaxRoom; ljlI++)
    {
        for (ljlJ = 0; ljlJ < 10; ljlJ++)
        {
            if (ljlChatrooms[ljlI].ljlUser[ljlJ] == ljlSfd)
            {
                ljlRoom = ljlI;
                break;
            }
        }
        if (ljlRoom != -1)
            break;  // 用户已加入某聊天室，跳出循环
    }

    // 如果用户没有加入任何聊天室
    if (ljlRoom == -1)
    {
        snprintf(ljlBuf, sizeof(ljlBuf), "Sorry, you have not joined any chat room.\n");
        send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
    }
    else
    {
        snprintf(ljlBuf, sizeof(ljlBuf), "%s\tAll users in the %s chat room:\n", ljlNowtime, ljlChatrooms[ljlRoom].ljlName);

        // 查找并列出聊天室中的所有在线用户
        for (ljlI = 0; ljlI < 10; ljlI++)
        {
            if (ljlChatrooms[ljlRoom].ljlUser[ljlI] >= 0)
            {
                for (ljlJ = 0; ljlJ < ljlMaxMem; ljlJ++)
                {
                    if (ljlOnlineUsers[ljlJ].ljlStatus != -1 && (ljlChatrooms[ljlRoom].ljlUser[ljlI] == ljlOnlineUsers[ljlJ].ljlSocketfd))
                    {
                        strncat(ljlBuf, "\t", sizeof(ljlBuf) - strlen(ljlBuf) - 1);
                        strncat(ljlBuf, ljlOnlineUsers[ljlJ].ljlUsername, sizeof(ljlBuf) - strlen(ljlBuf) - 1);
                        strncat(ljlBuf, "\n", sizeof(ljlBuf) - strlen(ljlBuf) - 1);
                    }
                }
            }
        }
        send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
    }
}

// 退出聊天室
void ljlExitChatroom(SOCKET ljlSfd)
{
    int ljlI, ljlJ;
    int ljlRoom = -1;
    char ljlBuf[ljlBuffSize];

    // 查找用户所在的聊天室
    for (ljlI = 0; ljlI < ljlMaxRoom; ljlI++)
    {
        if (ljlChatrooms[ljlI].ljlStatus == 0)
        {
            for (ljlJ = 0; ljlJ < 10; ljlJ++)
            {
                if (ljlChatrooms[ljlI].ljlUser[ljlJ] == ljlSfd)
                {
                    ljlChatrooms[ljlI].ljlUser[ljlJ] = -1;  // 将用户移除聊天室
                    ljlRoom = ljlI;
                    break;
                }
            }
            if (ljlRoom != -1)
                break;  // 找到聊天室后立即退出循环
        }
    }

    // 如果用户没有加入任何聊天室
    if (ljlRoom == -1)
    {
        snprintf(ljlBuf, sizeof(ljlBuf), "You have not joined any chat room.\n");
        send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
    }
    else
    {
        snprintf(ljlBuf, sizeof(ljlBuf), "Successfully quit chat room %s.\n", ljlChatrooms[ljlRoom].ljlName);
        send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
    }
}

// 退出服务器
void ljlQuit()
{
    int ljlI;
    char ljlMsg[10];

    while (1)
    {
        printf("Enter 'quit' to shut down the server: ");
        if (fgets(ljlMsg, sizeof(ljlMsg), stdin) == NULL)
        {
            perror("Error reading input");
            continue;  // 继续等待输入
        }

        // 去掉输入字符串的换行符
        ljlMsg[strcspn(ljlMsg, "\n")] = '\0';

        if (strcmp(ljlMsg, "quit") == 0)
        {
            ljlSaveUsers();
            printf("Byebye... \n");

            // 关闭监听套接字
            if (closesocket(ljlListenfd) == -1)
            {
                perror("Error closing listenfd");
                exit(1);
            }

            exit(0);
        }
    }
}


void ljlInvalidCommand(SOCKET ljlSfd)
{
    char ljlBuf[ljlBuffSize];

    // 提示无效命令并提供帮助信息
    strcpy(ljlBuf, "Invalid command. Please use one of the following valid commands:\n");
    strcat(ljlBuf, "/join <room_name> <password> - Join a chat room\n");
    strcat(ljlBuf, "/create <room_name> <password> - Create a new chat room\n");
    strcat(ljlBuf, "/exit - Exit the chat room\n");
    strcat(ljlBuf, "/quit - Quit the server\n");

    send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
}
