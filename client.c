#include "util.h"

#if __linux__

int client(int argc, char **argv) {
    int port = HELLO_WORLD_SERVER_PORT;
    if (argc > 3) {
        printf("Usage: ./%s ServerIPAddress [Port]\n", argv[0]);
        exit(1);
    }
    if (argc == 3) {
        port = get_port(argv[2]);
    }

    char *host = get_host(argv[1]);
    // 设置一个socket地址结构client_addr, 代表客户机的internet地址和端口
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET; // internet协议族
    client_addr.sin_addr.s_addr = htons(INADDR_ANY); // INADDR_ANY表示自动获取本机地址
    client_addr.sin_port = htons(0); // auto allocated, 让系统自动分配一个空闲端口

    // 创建用于internet的流协议(TCP)类型socket，用client_socket代表客户端socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        printf("Create Socket Failed!\n");
        exit(1);
    }

    // 把客户端的socket和客户端的socket地址结构绑定
    if (bind(client_socket, (struct sockaddr *) &client_addr, sizeof(client_addr))) {
        printf("Client Bind Port Failed!\n");
        exit(1);
    }

    // 设置一个socket地址结构server_addr,代表服务器的internet地址和端口
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;

    // 服务器的IP地址来自程序的参数
    if (inet_aton(host, &server_addr.sin_addr) == 0) {
        printf("Server IP Address Error!\n");
        exit(1);
    }

    server_addr.sin_port = htons(port);
    socklen_t server_addr_length = sizeof(server_addr);

    // 向服务器发起连接请求，连接成功后client_socket代表客户端和服务器端的一个socket连接
    if (connect(client_socket, (struct sockaddr *) &server_addr, server_addr_length) < 0) {
        printf("Can Not Connect To %s:%d!\n", host, port);
        exit(1);
    }

    receive_file(client_socket, host);

    return 0;
}

#elif __WIN32

int client(int argc, char **argv) {
    if (argc > 3) {
        printf("Usage: ./%s ServerIPAddress [Port]\n", argv[0]);
        exit(1);
    }
    char *host = get_host(argv[1]);
    int port = get_port(argv[2]);
    WORD wVersionRequested = MAKEWORD(2, 2);//关于内存 需要的版本  wdversion转换为二进制高字节装副版本号 低字节装主版本号
    WSADATA data;     //可以用指针 要申请堆空间 并且释放 在这里栈空间已经足够
    int wsaStartup = WSAStartup(wVersionRequested, &data);//打开网络库
    if (wsaStartup != 0) {
        return wsaStartup;
    }

    if (HIBYTE(data.wVersion) != 2 || LOBYTE(data.wVersion) != 2)//校验版本
    {
        //版本不对
        WSACleanup();//清理关闭网络库
        return 0;
    }

    SOCKET socketclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //操作接口 调用协议通信 第三个参数可以为0默认参数值 此处默认为tcp
    //第一个参数ip地址类型 第二个套接字类型 第三个传输协议

    if (socketclient == INVALID_SOCKET) {
        printf("socketclient error = %d !\n", WSAGetLastError());//连接失败出错了 找出错误码进行改正
        WSACleanup();   //清理网络库
        system("pause");
        return 0;
    }

    struct sockaddr_in client;
    client.sin_family = AF_INET;//ipv4
    client.sin_port = htons(port);
    client.sin_addr.S_un.S_addr = inet_addr(host);

    int con = connect(socketclient, (const struct sockaddr *) &client, sizeof(client));
    printf("connect server = %d !\n", con);
    if (con == SOCKET_ERROR) {
        printf("connecting server error = %d !\n", WSAGetLastError());//连接失败出错了 找出错误码进行改正
        closesocket(socketclient); //关闭网络连接(服务端)
        WSACleanup();//关闭网络库
        system("pause");
        return 0;
    }

    receive_file((int)socketclient, host);

    return 0;
}

#endif

int main(int argc, char **argv) {
    return client(argc, argv);
}
