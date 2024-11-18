#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFSIZE 128
#define HOST_IP "127.0.0.1"
#define PORT 8888

#pragma comment(lib, "Ws2_32.lib")

SOCKET sockfd;
void snd();
void get_help();
