#include "client.h"

int main()
{
    pthread_t thread;             // pthread_t 线程，gcc编译时需加上-lpthread
    struct sockaddr_in serv_addr; // struct sockaddr_in
    char buf[BUFFSIZE], temp[BUFFSIZE];

    // 初始化服务端地址结构
    bzero(&serv_addr, sizeof(struct sockaddr_in)); // bzero 清零
    serv_addr.sin_family = AF_INET;                // sin_family   AF_INET
    serv_addr.sin_port = htons(PORT);              // sin_port     htons(PORT)
    inet_pton(HOST_IP, &serv_addr.sin_addr);       // inet_pton
    // 创建客户端套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // socket 创建套接字
    if (sockfd < 0)
    {
        perror("fail to socket");
        exit(-1);
    }
    // 与服务器建立连接
    printf("connecting... \n");
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("fail to connect");
        exit(-2);
    }

    printf("Enter \"login\" to login\n");
    printf("Enter \"register\" to create an account\n");
    printf("Enter \"quit\" to quit\n");
    printf("Enter \"help\" to get more help\n\n");

    /* === 从此处开始 程序分做两个线程 === */
    // 创建发送消息的线程，调用发送消息的函数snd
    pthread_create(&thread, NULL, (void *)(&snd), NULL); // pthread_create
    // 接收消息的线程
    while (1)
    {
        int len;
        if ((len = read(sockfd, buf, BUFFSIZE)) > 0) // read 读取通信套接字
        {
            write(1, buf, len); //1：标准输出
            printf("\n");
        }
    }
    return 0;
}

/*发送消息的函数*/
void snd()
{
    char buf[BUFFSIZE];
    while (1)
    {
        fgets(buf, BUFFSIZE, stdin);
        if (strcmp(buf, "help\n") == 0)
        {
            get_help();
            continue;
        }
        if (strcmp(buf, "\n") != 0)
            write(sockfd, buf, strlen(buf));
        if (strcmp(buf, "quit\n") == 0) // 注意此处的\n
            exit(0);
    }
}

/*获取帮助信息*/
void get_help()
{
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

