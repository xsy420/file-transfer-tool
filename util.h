#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>

#if __linux__

#include <sys/socket.h>                         // for socket
#include <netinet/in.h>                         // for sockaddr_in
#include <arpa/inet.h>

#elif __WIN32
#include "winsock2.h"
#include <windows.h>
#include <winsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")//网络库
#endif

#define HELLO_WORLD_SERVER_PORT    6666         //端口号
#define BUFFER_SIZE                   1024
#define FILE_NAME_MAX_SIZE            512
#define LENGTH_OF_LISTEN_QUEUE     20

char *get_host(char *host_str);

int get_port(char *port_str);

void maintain_server(int server_socket);

void receive_file(int client_socket, char *host);
