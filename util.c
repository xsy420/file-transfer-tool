#include "util.h"

char *get_host(char *host_str) {
    if (host_str != NULL && strlen(host_str) != 0) {
        return host_str;
    }
    return "127.0.0.1";
}

int get_port(char *port_str) {
    int port = HELLO_WORLD_SERVER_PORT;
    char *ptr;
    if (port_str != NULL && strlen(port_str) != 0) {
        port = (int) strtol(port_str, &ptr, 10);
    }
    printf("Using port = %d\n", port);
    return port;
}

void maintain_server(int server_socket) {
    // 服务器端一直运行用以持续为客户端提供服务
    while (1) {
        // 定义客户端的socket地址结构client_addr，当收到来自客户端的请求后，调用accept
        // 接受此请求，同时将client端的地址和端口等信息写入client_addr中
        struct sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);

        printf("Server created successfully and waiting client connection\n");
        // 接受一个从client端到达server端的连接请求,将客户端的信息保存在client_addr中
        // 如果没有连接请求，则一直等待直到有连接请求为止，这是accept函数的特性，可以
        // 用select()来实现超时检测
        // accpet返回一个新的socket,这个socket用来与此次连接到server的client进行通信
        // 这里的new_server_socket代表了这个通信通道
        int new_server_socket = (int) accept(server_socket, (struct sockaddr *) &client_addr, &length);
        if (new_server_socket < 0) {
            printf("Server Accept Failed!\n");
            break;
        }

        char buffer[BUFFER_SIZE];
        bzero(buffer, sizeof(buffer));
        length = recv(new_server_socket, buffer, BUFFER_SIZE, 0);
        if (length < 0) {
            printf("Server Recieve Data Failed!\n");
            break;
        }

        char file_name[FILE_NAME_MAX_SIZE + 1];
        bzero(file_name, sizeof(file_name));
        strncpy(file_name, buffer,
                strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));

        FILE *fp = fopen(file_name, "rb");
        if (fp == NULL) {
            printf("File:\t%s Not Found!\n", file_name);
        } else {
            bzero(buffer, BUFFER_SIZE);
            int file_block_length;
            while ((file_block_length = (int) fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0) {
                printf("file_block_length = %d\n", file_block_length);

                // 发送buffer中的字符串到new_server_socket,实际上就是发送给客户端
                if (send(new_server_socket, buffer, file_block_length, 0) < 0) {
                    printf("Send File:\t%s Failed!\n", file_name);
                    break;
                }

                bzero(buffer, sizeof(buffer));
            }
            fclose(fp);
            printf("File:\t%s Transfer Finished!\n", file_name);
        }

#if __linux
        close(new_server_socket);
#elif __WIN32
        closesocket(new_server_socket);
#endif
    }

#if __linux
    close(server_socket);
#elif __WIN32
    closesocket(server_socket); //关闭网络连接(服务端)
#endif
}

void receive_file(int client_socket, char *host) {

    char file_name[FILE_NAME_MAX_SIZE + 1];
    bzero(file_name, sizeof(file_name));
    printf("Please Input File Name On Server.\t");
    scanf("%s", file_name);

    char buffer[BUFFER_SIZE];
    bzero(buffer, sizeof(buffer));
    strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));

    // 向服务器发送buffer中的数据，此时buffer中存放的是客户端需要接收的文件的名字
    send(client_socket, buffer, BUFFER_SIZE, 0);

    strcpy(file_name, basename(file_name));

    printf("filename = %s\n", file_name);

    FILE *fp = fopen(file_name, "wb");
    if (fp == NULL) {
        printf("File:\t%s Can Not Open To Write!\n", file_name);
        exit(1);
    }

    // 从服务器端接收数据到buffer中
    bzero(buffer, sizeof(buffer));
    int length = (int) recv(client_socket, buffer, BUFFER_SIZE, 0);
    while (length) {
        if (length < 0) {
            printf("Recieve Data From Server %s Failed!\n", host);
            break;
        }

        int write_length = (int) fwrite(buffer, sizeof(char), length, fp);
        if (write_length < length) {
            printf("File:\t%s Write Failed!\n", file_name);
            break;
        }
        bzero(buffer, BUFFER_SIZE);
        length = (int) recv(client_socket, buffer, BUFFER_SIZE, 0);
    }

    printf("Recieve File:\t %s From Server[%s] Finished!\n", file_name, host);

    // 传输完毕，关闭socket
    fclose(fp);
    close(client_socket);
}