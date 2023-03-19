#include "util.h"

#if __linux__

int server(char **argv) {
    int port = get_port(argv[1]);

    // set socket's address information
    // 设置一个socket地址结构server_addr,代表服务器internet的地址和端口
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // create a stream socket
    // 创建用于internet的流协议(TCP)socket，用server_socket代表服务器向客户端提供服务的接口
    int server_socket = (int) socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        printf("Create Socket Failed!\n");
        exit(1);
    }

    // 把socket和socket地址结构绑定
    if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
        printf("Server Bind Port: %d Failed!\n", port);
        exit(1);
    }

    // server_socket用于监听
    if (listen(server_socket, LENGTH_OF_LISTEN_QUEUE)) {
        printf("Server Listen Failed!\n");
        exit(1);
    }

    maintain_server(server_socket);

    return 0;
}

#elif __WIN32

int server(char **argv) {

    int port = get_port(argv[1]);

    WORD wVersionRequested = MAKEWORD(2, 2);//关于内存 需要的版本  wdversion转换为二进制高字节装副版本号 低字节装主版本号
    WSADATA wordSockMessage;     //可以用指针 要申请堆空间 并且释放 在这里栈空间已经足够
    int wsaStartup = WSAStartup(wVersionRequested, &wordSockMessage);//打开网络库

    if (wsaStartup != 0) {
        int error = WSAGetLastError();  //错误码
        WSACleanup();   //清理网络库
        return error;
    }

    SOCKET socketsever = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //操作接口 调用协议通信 第三个参数可以为0默认参数值 此处默认为tcp
    //第一个参数ip地址类型 第二个套接字类型 第三个传输协议

    if (socketsever == INVALID_SOCKET) {
        int error = WSAGetLastError();  //错误码
        WSACleanup();   //清理网络库
        return error;
    }

    struct sockaddr_in sever;
    sever.sin_family = AF_INET;//ipv4
    sever.sin_port = htons(port);
    sever.sin_addr.S_un.S_addr = htons(INADDR_ANY);

    int bind_value = bind(socketsever, (const struct sockaddr *) &sever,
                          sizeof(sever));//服务器端要用 bind() 函数将套接字与特定的 IP 地址和端口绑定起来
    /*bind第一个参数是socket变量 第二个参数为结构体变量addr里面有端口号 ip地址 第三个变量为addr变量的大小*//*（返回值正确为0 错误返回错误码）*/
    if (bind_value != 0) {
        int error2 = WSAGetLastError();//获得错误码
        closesocket(socketsever); //关闭网络连接
        WSACleanup();//关闭网络库
        return error2;
    }

    int listen_value = listen(socketsever, SOMAXCONN);//开始监听

    if (listen_value == SOCKET_ERROR) {
        int error2 = WSAGetLastError();//获得错误码
        closesocket(socketsever); //关闭网络连接
        WSACleanup();//关闭网络库
        return error2;
    }

    maintain_server((int)socketsever);

    WSACleanup();//关闭网络库

    return 0;

}

#endif

int main(__attribute__((unused)) int argc, char **argv) {
    return server(argv);
}
