# File-Transfer-Tool 文件传输工具

基于Socket网络编程，通过TCP协议实现的文件传输工具

## Description

- server

在本地开启一个服务器，可指定端口，默认6666

- client

连接服务器，可指定ip地址和端口，默认127.0.0.1和6666。
且第一个参数为ip地址，第二个参数为端口号。

连接成功后，输入服务器上的文件路径（支持绝对路径和相对路径），
文件支持二进制文件和非二进制文件，
即可下载到客户端

## Build from source

- **Linux**

```shell
cmake -B build -DCMAKE_BUILD_TYPE=release
cd build
make
```

- **Windows**

```shell
cmake -B build -DCMAKE_BUILD_TYPE=release -G "MinGW Makefiles"
cd build
mingw32-make
```

## Platform

- Windows

- Linux
