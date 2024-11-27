#include "server.h"

int main()
{
    // Winsock ��ʼ��
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

    // ��շ�������ַ�ṹ
    memset(&ljlServAddr, 0, sizeof(struct sockaddr_in));
    ljlServAddr.sin_family = AF_INET;
    ljlServAddr.sin_port = htons(ljlPort);
    ljlServAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // ���������׽���
    ljlListenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (ljlListenfd == INVALID_SOCKET)
    {
        perror("fail to socket");
        WSACleanup();
        exit(-1);
    }

    // ���׽���
    if (bind(ljlListenfd, (struct sockaddr *)&ljlServAddr, sizeof(ljlServAddr)) == SOCKET_ERROR)
    {
        perror("fail to bind");
        closesocket(ljlListenfd);
        WSACleanup();
        exit(-2);
    }

    // ��ʼ����
    if (listen(ljlListenfd, ljlMaxMem) == SOCKET_ERROR)
    {
        perror("fail to listen");
        closesocket(ljlListenfd);
        WSACleanup();
        exit(-3);
    }

    // ����һ���̣߳����ڹ�������������� quit ����
    pthread_create(&ljlThread, NULL, (void *)(ljlQuit), NULL);

    // ��ʼ�������׽�������
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
        ljlConnfd[ljlI] = INVALID_SOCKET;

    // ���տͻ�������
    while (1)
    {
        int ljlLen;
        for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
        {
            if (ljlConnfd[ljlI] == INVALID_SOCKET)
                break;
        }

        // ��������
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

        // ��Ե�ǰ�׽��ִ���һ���߳���������Ϣ
        pthread_t ljlRecvThread;
        pthread_create(&ljlRecvThread, NULL, (void *)(&ljlRcvSnd), (void *)ljlI);
    }

}



/*���������պͷ��ͺ���*/
void ljlRcvSnd(int ljlN)
{
    ssize_t ljlLen;
    int ljlI;
    char ljlMyTime[32], ljlBuff[ljlBuffSize];
    char ljlTemp[ljlBuffSize];
    char ljlCommand[20], ljlArg1[20], ljlArg2[ljlBuffSize];
    time_t ljlTimeNow;

    // ��һ��ѭ�������ڽ��պʹ����¼��ע������
    while (1)
    {
        ljlLen = recv(ljlConnfd[ljlN], ljlBuff, ljlBuffSize, 0);
        if (ljlLen > 0)
        {
            ljlBuff[ljlLen - 1] = '\0'; // ȥ�����з�

            // �����¼����
            if (strcmp(ljlBuff, "login") == 0)
            {
                // ��¼�ɹ�ʱ�˳���ѭ��
                if (ljlUserLogin(ljlN) == 0)
                {
                    break;
                }
            }
                // ����ע������
            else if (strcmp(ljlBuff, "register") == 0)
            {
                ljlRegisterUser(ljlN);
            }
                // �����˳�����
            else if (strcmp(ljlBuff, "quit") == 0)
            {
                ljlQuitClient(ljlN);
            }
        }
    }

    // �ڶ���ѭ�������ڴ����¼�������
    while (1)
    {
        if ((ljlLen = recv(ljlConnfd[ljlN], ljlTemp, ljlBuffSize, 0)) > 0)
        {
            ljlTemp[ljlLen - 1] = '\0'; // ȥ�����з�
            sscanf(ljlTemp, "%s %s %[^\n]", ljlCommand, ljlArg1, ljlArg2); // ��������

            // ��������Ϣ������
            if (strcmp(ljlCommand, "send") == 0 && strcmp(ljlArg1, "-all") == 0)
            {
                ljlSendAllMsg(ljlArg2, ljlN);  // �����������û�������Ϣ
            }
            else if (strcmp(ljlCommand, "send") == 0 && strcmp(ljlArg1, "-chatroom") == 0)
            {
                ljlSendChatroomMsg(ljlArg2, ljlN);  // ���������е������û�������Ϣ
            }
            else if (strcmp(ljlCommand, "send") == 0)
            {
                ljlSendPrivateMsg(ljlArg1, ljlArg2, ljlN);  // ��ָ���û�����˽����Ϣ
            }
                // �����˳�����
            else if (strcmp(ljlCommand, "quit") == 0)
            {
                ljlQuitClient(ljlN);
            }
                // �����޸���������
            else if (strcmp(ljlCommand, "chgpsw") == 0)
            {
                ljlChangePasswd(ljlN, ljlArg1);
            }
                // ����������������
            else if (strcmp(ljlCommand, "create") == 0)
            {
                ljlCreateChatroom(ljlArg1, ljlArg2, ljlN);
            }
                // �����������������
            else if (strcmp(ljlCommand, "join") == 0)
            {
                ljlJoinChatroom(ljlArg1, ljlArg2, ljlN);
            }
                // �����г�����������
            else if (strcmp(ljlCommand, "ls") == 0 && strcmp(ljlArg1, "-chatrooms") == 0)
            {
                ljlGetOnlineChatrooms(ljlN);
            }
                // �����г������û�����
            else if (strcmp(ljlCommand, "ls") == 0 && strcmp(ljlArg1, "-users") == 0)
            {
                ljlGetOnlineUsers(ljlN);
            }
                // �����г����������û�����
            else if (strcmp(ljlCommand, "ls") == 0 && strcmp(ljlArg1, "-inrmusr") == 0)
            {
                ljlGetInroomUsers(ljlN);
            }
                // �����˳�����������
            else if (strcmp(ljlCommand, "exit") == 0)
            {
                ljlExitChatroom(ljlN);
            }
                // ������Ч����
            else
            {
                ljlInvalidCommand(ljlN);
            }
        }
    }
}


//��ʼ��
void ljlInit()
{
    int ljlI, ljlJ;
    ljlUserCount = 0;
    ljlChatroomCount = 0;

    // ��ʼ�������û�״̬
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
    {
        ljlOnlineUsers[ljlI].ljlStatus = -1; // -1 ��ʾ����
    }

    // ��ʼ��������״̬
    for (ljlI = 0; ljlI < ljlMaxRoom; ljlI++)
    {
        ljlChatrooms[ljlI].ljlStatus = -1; // -1 ��ʾ�����Ҳ�����
        for (ljlJ = 0; ljlJ < 10; ljlJ++)
        {
            ljlChatrooms[ljlI].ljlUser[ljlJ] = -1; // ��ʼ���������е��û�Ϊ -1�����û���
        }
    }

    // ���ļ������ļ���ȡ�û�����
    char ljlBuf[20];
    FILE *ljlFp = NULL;
    ljlFp = fopen("users.txt", "r");
    if (ljlFp == NULL)
    {
        printf("Error opening users.txt file.\n");
        return;
    }

    // ���ļ��ж�ȡ�û��������벢�洢
    while (fscanf(ljlFp, "%s", ljlBuf) != EOF)
    {
        strcpy(ljlUsers[ljlUserCount].ljlUsername, ljlBuf);  // ��ȡ�û���
        fscanf(ljlFp, "%s", ljlBuf);
        strcpy(ljlUsers[ljlUserCount].ljlPassword, ljlBuf);  // ��ȡ����
        ljlUserCount++;  // �����û�����
    }

    // �ر��ļ�
    fclose(ljlFp);
}


/*���û����浽�ļ�*/
void ljlSaveUsers()
{
    int ljlI;
    FILE *ljlFp = NULL;

    // ���ļ��Խ���д����������ļ��������򴴽�
    ljlFp = fopen("users.txt", "w+");
    if (ljlFp == NULL)  // ����ļ��Ƿ�ɹ���
    {
        printf("Error opening users.txt file.\n");
        return;
    }

    // �����û����飬д��ÿ���û����û���������
    for (ljlI = 0; ljlI < ljlUserCount; ljlI++)
    {
        // ʹ�� fprintf ֱ�ӽ��û���������д���ļ�
        fprintf(ljlFp, "%s\n", ljlUsers[ljlI].ljlUsername);
        fprintf(ljlFp, "%s\n", ljlUsers[ljlI].ljlPassword);
    }

    // �ر��ļ�
    fclose(ljlFp);
}


//�˳��ͻ���
void ljlQuitClient(int ljlN)
{
    int ljlI;

    // ȷ���׽���������Ч
    if (ljlConnfd[ljlN] != -1)
    {
        closesocket(ljlConnfd[ljlN]);  // �ر��׽�������
        ljlConnfd[ljlN] = -1;    // ��������״̬����Ǹ�λ��Ϊ��
    }

    // ���������û�״̬
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
    {
        if (ljlOnlineUsers[ljlI].ljlSocketfd == ljlConnfd[ljlN])
        {
            ljlOnlineUsers[ljlI].ljlStatus = -1;  // ����û�����
            break;  // �ҵ��û������º�����ѭ��
        }
    }

    // �˳��̣߳�������Ҫ�����˳�״̬��ֱ�ӵ���pthread_exit()
    pthread_exit(NULL);
}

//�û���¼
int ljlUserLogin(int ljlN)
{
    int ljlLen, ljlI, ljlJ;
    char ljlBuf[ljlBuffSize], ljlUsername[20], ljlPassword[20];

    // ��ʾ�û������û���
    sprintf(ljlBuf, "Your username: ");
    send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);

    // ��ȡ�û���
    ljlLen = recv(ljlConnfd[ljlN], ljlUsername, 20, 0);
    if (ljlLen > 0)
    {
        ljlUsername[ljlLen - 1] = '\0'; // ȥ�����з�
    }

    // ��ʾ�û���������
    sprintf(ljlBuf, "Your password: ");
    send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);

    // ��ȡ����
    ljlLen = recv(ljlConnfd[ljlN], ljlPassword, 20, 0);
    if (ljlLen > 0)
    {
        ljlPassword[ljlLen - 1] = '\0'; // ȥ�����з�
    }

    // �����û�������֤����
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
    {
        if (strcmp(ljlUsername, ljlUsers[ljlI].ljlUsername) == 0)
        {
            // �û���ƥ�䣬��֤����
            if (strcmp(ljlPassword, ljlUsers[ljlI].ljlPassword) == 0)
            {
                sprintf(ljlBuf, "Login successfully.\n\n");
                send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);

                // ���ҵ�һ�����е�λ�ò����û����Ϊ����
                for (ljlJ = 0; ljlJ < ljlMaxMem; ljlJ++)
                {
                    if (ljlOnlineUsers[ljlJ].ljlStatus == -1)
                    {
                        strcpy(ljlOnlineUsers[ljlJ].ljlUsername, ljlUsername);
                        ljlOnlineUsers[ljlJ].ljlSocketfd = ljlN;
                        ljlOnlineUsers[ljlJ].ljlStatus = 0;
                        break;  // �ҵ�����λ�ú��˳�
                    }
                }
                return 0;  // ��¼�ɹ�
            }
            else
            {
                // �������
                sprintf(ljlBuf, "Wrong password.\n\n");
                send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);
                return -1;  // �������
            }
        }
    }

    // �û���������
    sprintf(ljlBuf, "Account does not exist.\n\n");
    send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);
    return -1;  // �û���������
}

//�û�ע��
void ljlRegisterUser(int ljlN)
{
    int ljlLen, ljlI;
    char ljlBuf[ljlBuffSize], ljlUsername[20], ljlPassword[20];

    // ��ʾ�û������û���
    sprintf(ljlBuf, "Your username: ");
    send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);

    // ��ȡ�û���
    ljlLen = recv(ljlConnfd[ljlN], ljlUsername, 20, 0);
    if (ljlLen > 0)
    {
        ljlUsername[ljlLen - 1] = '\0'; // ȥ�����з�
    }

    // ��ʾ�û���������
    sprintf(ljlBuf, "Your password: ");
    send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);

    // ��ȡ����
    ljlLen = recv(ljlConnfd[ljlN], ljlPassword, 20, 0);
    if (ljlLen > 0)
    {
        ljlPassword[ljlLen - 1] = '\0'; // ȥ�����з�
    }

    // ����û����Ƿ��Ѿ�����
    for (ljlI = 0; ljlI < ljlUserCount; ljlI++)
    {
        if (strcmp(ljlUsers[ljlI].ljlUsername, ljlUsername) == 0)
        {
            sprintf(ljlBuf, "The username already exists.\n\n");
            send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);
            return;
        }
    }

    // ����û������������޷�ע��
    if (ljlUserCount >= ljlMaxMem)
    {
        sprintf(ljlBuf, "User registration limit reached.\n\n");
        send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);
        return;
    }

    // �������û���Ϣ
    strcpy(ljlUsers[ljlUserCount].ljlUsername, ljlUsername);
    strcpy(ljlUsers[ljlUserCount].ljlPassword, ljlPassword);
    ljlUserCount++;

    // ����ע��ɹ���Ϣ
    sprintf(ljlBuf, "Account created successfully.\n\n");
    send(ljlConnfd[ljlN], ljlBuf, strlen(ljlBuf), 0);
}

/*�û�����˽����Ϣ*/
void ljlSendPrivateMsg(char *ljlUsername, char *ljlData, SOCKET ljlSfd)
{
    int ljlI, ljlJ;
    time_t ljlNow;
    char ljlSendMan[20];
    char ljlBuf[ljlBuffSize], ljlNowtime[20], ljlTemp[30];
    ljlNow = time(NULL);  // ��ȡ��ǰʱ��
    struct tm *ljlTempTime = localtime(&ljlNow);
    strftime(ljlNowtime, sizeof(ljlNowtime), "[%H:%M:%S]", ljlTempTime);  // ��ʽ��ʱ��

    // ���ҷ����ߵ��û���
    for (ljlJ = 0; ljlJ < ljlMaxMem; ljlJ++)
    {
        if (ljlSfd == ljlOnlineUsers[ljlJ].ljlSocketfd)
        {
            strcpy(ljlSendMan, ljlOnlineUsers[ljlJ].ljlUsername);
            break;
        }
    }

    // ����Ŀ���û���������Ϣ
    int ljlFound = 0;  // ����Ƿ��ҵ�Ŀ���û�
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
    {
        if (strcmp(ljlUsername, ljlOnlineUsers[ljlI].ljlUsername) == 0)
        {
            snprintf(ljlBuf, sizeof(ljlBuf), "%s\tfrom %s:\n%s\n", ljlNowtime, ljlSendMan, ljlData);
            send(ljlConnfd[ljlOnlineUsers[ljlI].ljlSocketfd], ljlBuf, strlen(ljlBuf) + 1, 0);  // ����˽����Ϣ
            strcpy(ljlTemp, "Sent successfully.\n");
            send(ljlConnfd[ljlSfd], ljlTemp, strlen(ljlTemp) + 1, 0);  // ���ͳɹ���ִ
            ljlFound = 1;
            break;
        }
    }

    // ���δ�ҵ�Ŀ���û�
    if (!ljlFound)
    {
        strcpy(ljlBuf, "User is not online or user does not exist.\n");
        send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);  // ���ش�����Ϣ��������
    }
}


//��ϢȺ��
void ljlSendAllMsg(char *ljlMsg, SOCKET ljlSfd)
{
    int ljlI;
    char ljlBuf[ljlBuffSize], ljlNowtime[20], ljlSendMan[20], ljlTemp[30];
    time_t ljlNow;
    ljlNow = time(NULL);  // ��ȡ��ǰʱ��
    struct tm *ljlTempTime = localtime(&ljlNow);
    strftime(ljlNowtime, sizeof(ljlNowtime), "[%H:%M:%S]", ljlTempTime);  // ��ʽ��ʱ��

    // ���ҷ����ߵ��û���
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
    {
        if (ljlSfd == ljlOnlineUsers[ljlI].ljlSocketfd)
        {
            strcpy(ljlSendMan, ljlOnlineUsers[ljlI].ljlUsername);
            break;
        }
    }

    // ������Ϣ����
    snprintf(ljlBuf, sizeof(ljlBuf), "%s\tfrom %s(goup-sent):\n%s\n", ljlNowtime, ljlSendMan, ljlMsg);

    // Ⱥ����Ϣ�����������û�
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
    {
        if (ljlConnfd[ljlI] != -1 && ljlI != ljlSfd)  // ȷ��������Ч�Ҳ��Ƿ������Լ�
        {
            send(ljlConnfd[ljlI], ljlBuf, strlen(ljlBuf) + 1, 0);  // ������Ϣ
        }
    }

    // �����߷��ط��ͳɹ��Ļ�ִ
    snprintf(ljlTemp, sizeof(ljlTemp), "Sent successfully\n");
    send(ljlConnfd[ljlSfd], ljlTemp, strlen(ljlTemp) + 1, 0);
}

//��ȡ�����û�
void ljlGetOnlineUsers(SOCKET ljlSfd)
{
    int ljlI;
    char ljlBuf[ljlBuffSize], ljlNowtime[20];
    time_t ljlNow;
    ljlNow = time(NULL);  // ��ȡ��ǰʱ��
    struct tm *ljlTempTime = localtime(&ljlNow);
    strftime(ljlNowtime, sizeof(ljlNowtime), "[%H:%M:%S]", ljlTempTime);  // ��ʽ��ʱ��

    // �������ص���Ϣ����
    snprintf(ljlBuf, sizeof(ljlBuf), "%s\tAll online user(s):\n", ljlNowtime);

    // ���ÿ�������û����û���
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
    {
        if (ljlOnlineUsers[ljlI].ljlStatus == 0)  // ֻ�г������û�
        {
            snprintf(ljlBuf + strlen(ljlBuf), sizeof(ljlBuf) - strlen(ljlBuf), "\t%s\n", ljlOnlineUsers[ljlI].ljlUsername);
        }
    }

    // ���͸�����Ŀͻ���
    send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
}

//������Ϣ��������
void ljlSendChatroomMsg(char *ljlMsg, SOCKET ljlSfd)
{
    int ljlI, ljlJ, ljlK;
    int ljlFlag = -1;
    char ljlBuf[ljlBuffSize], ljlNowtime[20];
    time_t ljlNow;
    ljlNow = time(NULL);
    struct tm *ljlTempTime = localtime(&ljlNow);
    strftime(ljlNowtime, sizeof(ljlNowtime), "[%H:%M:%S]", ljlTempTime);  // ��ʽ����ǰʱ��

    // ����û��Ƿ������������
    for (ljlI = 0; ljlI < ljlMaxRoom; ljlI++)
    {
        if (ljlChatrooms[ljlI].ljlStatus == 0)  // �������Ч��������
        {
            for (ljlJ = 0; ljlJ < 10; ljlJ++)
            {
                if (ljlChatrooms[ljlI].ljlUser[ljlJ] == ljlSfd)  // ����Ƿ��������ҵĳ�Ա
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
        // �û�δ����������
        strcpy(ljlBuf, "You have not joined the chat room.\n");
        send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
    }
    else
    {
        // ��ȡ������Ϣ���û���
        for (ljlK = 0; ljlK < ljlMaxMem; ljlK++)
        {
            if (ljlOnlineUsers[ljlK].ljlSocketfd == ljlSfd)  // �ҵ�������Ϣ���û�
                break;
        }

        // ������Ϣ����
        snprintf(ljlBuf, sizeof(ljlBuf), "%s\tchatroom %s:\nfrom %s:\t%s\n",
                 ljlNowtime, ljlChatrooms[ljlI].ljlName, ljlOnlineUsers[ljlK].ljlUsername, ljlMsg);

        // ���������ڵ����г�Ա������Ϣ
        for (ljlK = 0; ljlK < 10; ljlK++)
        {
            if (ljlChatrooms[ljlI].ljlUser[ljlK] != -1)
            {
                send(ljlConnfd[ljlChatrooms[ljlI].ljlUser[ljlK]], ljlBuf, strlen(ljlBuf) + 1, 0);
            }
        }
    }
}


//����������
void ljlCreateChatroom(char *ljlName, char *ljlPasswd, SOCKET ljlSfd)
{
    int ljlI, ljlJ;
    char ljlBuf[ljlBuffSize];

    // ���ҵ�һ�����е�������λ��
    for (ljlI = 0; ljlI < ljlMaxRoom; ljlI++)
    {
        if (ljlChatrooms[ljlI].ljlStatus == -1)  // �ҵ�δʹ�õ�������
            break;
    }

    if (ljlI == ljlMaxRoom)
    {
        // ���û�п��е������ң����ش�����Ϣ
        strcpy(ljlBuf, "No available rooms to create.\n");
        send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
        return;
    }

    // ��ȫ�ؿ������������ƺ����룬�������
    snprintf(ljlChatrooms[ljlI].ljlName, sizeof(ljlChatrooms[ljlI].ljlName), "%s", ljlName);
    snprintf(ljlChatrooms[ljlI].ljlPasswd, sizeof(ljlChatrooms[ljlI].ljlPasswd), "%s", ljlPasswd);
    ljlChatrooms[ljlI].ljlStatus = 0;  // ��������״̬����Ϊ���Ѵ�����

    // ���û�����������
    for (ljlJ = 0; ljlJ < 10; ljlJ++)
    {
        if (ljlChatrooms[ljlI].ljlUser[ljlJ] == -1)  // �ҵ���λ
        {
            ljlChatrooms[ljlI].ljlUser[ljlJ] = ljlSfd;  // ���û�����������
            break;
        }
    }

    // ֪ͨ�û������Ҵ����ɹ�
    snprintf(ljlBuf, sizeof(ljlBuf), "Successfully created chat room '%s'.\n", ljlChatrooms[ljlI].ljlName);
    send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
}

//����������
void ljlJoinChatroom(char *ljlName, char *ljlPasswd, SOCKET ljlSfd)
{
    int ljlI, ljlJ;
    char ljlBuf[ljlBuffSize];
    int ljlRoomFound = -1;

    // ����û��Ƿ��Ѿ���ĳ��������
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
            break;  // �û��Ѿ���ĳ����������
    }

    if (ljlRoomFound != -1)
    {
        snprintf(ljlBuf, sizeof(ljlBuf), "You have already joined the chat room '%s'.\n", ljlChatrooms[ljlRoomFound].ljlName);
        send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
        return;  // �û��Ѿ�����������ң�ֱ�ӷ���
    }

    // ����Ŀ�������Ҳ�����
    for (ljlI = 0; ljlI < ljlMaxRoom; ljlI++)
    {
        if (ljlChatrooms[ljlI].ljlStatus != -1 && strcmp(ljlChatrooms[ljlI].ljlName, ljlName) == 0)  // �ҵ�������
        {
            if (strcmp(ljlChatrooms[ljlI].ljlPasswd, ljlPasswd) == 0)  // ������ȷ
            {
                // ���ҿ�λ�ü���������
                for (ljlJ = 0; ljlJ < 10; ljlJ++)
                {
                    if (ljlChatrooms[ljlI].ljlUser[ljlJ] == -1)  // �ҵ���λ
                    {
                        ljlChatrooms[ljlI].ljlUser[ljlJ] = ljlSfd;
                        snprintf(ljlBuf, sizeof(ljlBuf), "Successfully joined the chat room '%s'.\n", ljlChatrooms[ljlI].ljlName);
                        send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
                        return;  // �ɹ�����������
                    }
                }
                // �������������
                snprintf(ljlBuf, sizeof(ljlBuf), "Chat room '%s' is full.\n", ljlChatrooms[ljlI].ljlName);
                send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
                return;
            }
            else
            {
                // �������
                snprintf(ljlBuf, sizeof(ljlBuf), "Incorrect password for chat room '%s'.\n", ljlName);
                send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
                return;
            }
        }
    }

    // ���û���ҵ���������
    snprintf(ljlBuf, sizeof(ljlBuf), "Chat room '%s' does not exist.\n", ljlName);
    send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
}

//��ȡ��������Ϣ
void ljlGetOnlineChatrooms(SOCKET ljlSfd)
{
    int ljlI;
    char ljlBuf[ljlBuffSize];
    time_t ljlNow = time(NULL);
    struct tm *ljlTempTime = localtime(&ljlNow);

    // ��ȡ��ǰʱ��
    snprintf(ljlBuf, sizeof(ljlBuf), "[%02d:%02d:%02d]\tAll online chat room(s):\n",
             ljlTempTime->tm_hour, ljlTempTime->tm_min, ljlTempTime->tm_sec);

    // ��ȡ��������������
    for (ljlI = 0; ljlI < ljlMaxRoom; ljlI++)
    {
        if (ljlChatrooms[ljlI].ljlStatus == 0)  // �ж��������Ƿ�����
        {
            snprintf(ljlBuf + strlen(ljlBuf), sizeof(ljlBuf) - strlen(ljlBuf), "\t%s\n", ljlChatrooms[ljlI].ljlName);
        }
    }

    send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
}


// �޸�����
void ljlChangePasswd(SOCKET ljlSfd, char *ljlPasswd)
{
    int ljlI;
    char ljlBuf[ljlBuffSize], ljlName[20];

    // ��ȡ��ǰ�û����û���
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
    {
        if (ljlSfd == ljlOnlineUsers[ljlI].ljlSocketfd)
        {
            strncpy(ljlName, ljlOnlineUsers[ljlI].ljlUsername, sizeof(ljlName) - 1);
            ljlName[sizeof(ljlName) - 1] = '\0';  // ȷ���ַ�����ֹ
            break;
        }
    }

    // ���Ҳ���������
    for (ljlI = 0; ljlI < ljlMaxMem; ljlI++)
    {
        if (strcmp(ljlName, ljlUsers[ljlI].ljlUsername) == 0)
        {
            strncpy(ljlUsers[ljlI].ljlPassword, ljlPasswd, sizeof(ljlUsers[ljlI].ljlPassword) - 1);
            ljlUsers[ljlI].ljlPassword[sizeof(ljlUsers[ljlI].ljlPassword) - 1] = '\0';  // ȷ���ַ�����ֹ
            snprintf(ljlBuf, sizeof(ljlBuf), "Password has been updated for user: %s\n", ljlName);
            send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
            break;
        }
    }
}

/* ��ѯ���м���ĳ�����ҵ��û� */
void ljlGetInroomUsers(SOCKET ljlSfd)
{
    int ljlI, ljlJ;
    int ljlRoom = -1;
    char ljlBuf[ljlBuffSize], ljlNowtime[20];
    time_t ljlNow;
    time(&ljlNow);
    struct tm *ljlTempTime = localtime(&ljlNow);
    strftime(ljlNowtime, 20, "[%H:%M:%S]", ljlTempTime);

    // �����û����ڵ�������
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
            break;  // �û��Ѽ���ĳ�����ң�����ѭ��
    }

    // ����û�û�м����κ�������
    if (ljlRoom == -1)
    {
        snprintf(ljlBuf, sizeof(ljlBuf), "Sorry, you have not joined any chat room.\n");
        send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
    }
    else
    {
        snprintf(ljlBuf, sizeof(ljlBuf), "%s\tAll users in the %s chat room:\n", ljlNowtime, ljlChatrooms[ljlRoom].ljlName);

        // ���Ҳ��г��������е����������û�
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

// �˳�������
void ljlExitChatroom(SOCKET ljlSfd)
{
    int ljlI, ljlJ;
    int ljlRoom = -1;
    char ljlBuf[ljlBuffSize];

    // �����û����ڵ�������
    for (ljlI = 0; ljlI < ljlMaxRoom; ljlI++)
    {
        if (ljlChatrooms[ljlI].ljlStatus == 0)
        {
            for (ljlJ = 0; ljlJ < 10; ljlJ++)
            {
                if (ljlChatrooms[ljlI].ljlUser[ljlJ] == ljlSfd)
                {
                    ljlChatrooms[ljlI].ljlUser[ljlJ] = -1;  // ���û��Ƴ�������
                    ljlRoom = ljlI;
                    break;
                }
            }
            if (ljlRoom != -1)
                break;  // �ҵ������Һ������˳�ѭ��
        }
    }

    // ����û�û�м����κ�������
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

// �˳�������
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
            continue;  // �����ȴ�����
        }

        // ȥ�������ַ����Ļ��з�
        ljlMsg[strcspn(ljlMsg, "\n")] = '\0';

        if (strcmp(ljlMsg, "quit") == 0)
        {
            ljlSaveUsers();
            printf("Byebye... \n");

            // �رռ����׽���
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

    // ��ʾ��Ч����ṩ������Ϣ
    strcpy(ljlBuf, "Invalid command. Please use one of the following valid commands:\n");
    strcat(ljlBuf, "/join <room_name> <password> - Join a chat room\n");
    strcat(ljlBuf, "/create <room_name> <password> - Create a new chat room\n");
    strcat(ljlBuf, "/exit - Exit the chat room\n");
    strcat(ljlBuf, "/quit - Quit the server\n");

    send(ljlConnfd[ljlSfd], ljlBuf, strlen(ljlBuf) + 1, 0);
}
