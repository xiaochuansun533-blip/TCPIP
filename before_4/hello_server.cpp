//服务器端是能够受理连接请求的程序
//以下创建等待连接请求的服务器端
//服务器端创建的socket称为服务器端socket或者监听socket
#include<stdio.h>//C标准库的标准输入输出
#include<stdlib.h>//通用工具：内存与进程退出
#include<string.h>//字符串/内存块处理
#include<unistd.h>//UNIX系统调用
#include<arpa/inet.h>//套接字核心API
#include<sys/socket.h>//网络地址/字节序工具
#include<iostream>
#include<string>
#include<cstring>
#include "util.h"
//先编译运行接收请求的服务器端代码
//再运行发送请求的客户端代码
//编译运行的命令
//# 编译生成服务器可执行文件
//g++ -std=c++11 hello_server.cpp util.cpp -o hello_server

//# 运行（给端口参数，选 >1024）
//./hello_server 9090

int main(int argc,char *argv[])//argc表示命令行参数总数,argv存储命令行参数的字符串数组
{
    // argv存储的是  "./hello_server"  +端口号
     
    std::cout<<"----------------------------------------"<<std::endl;
    
    int serv_sock;
    int clnt_sock;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;
    std::string message="hello world!";

    if(argc!=2)//如果只是输入./hello_server，没有加端口号
    {
        std::cout<<"usage:"<<argv[0]<<"<port>"<<std::endl;
        exit(1);//exit()  cause normal program termination to occur
    }
    serv_sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(serv_sock==-1)
    {
        error_handling("socket() error");
    }
    memset(&serv_addr,0,sizeof(serv_addr));
    //void* memset( void* dest, int ch, std::size_t count );
    //copies the value static_cast<unsigned char>(ch) into each of the first [count] characters of the object pointed to by [dest]
    //return [dest]
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);//host to net 这是将主机字节序转换成网络字节序  通过INADDR_ANY指定IP地址
    serv_addr.sin_port=htons(atoi(argv[1]));//将字符串转换成int  指定端口号
    
    if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
    {//如果函数调用成功，则将第二个参数指定的地址信息分配给第一个参数中的相应socket
        error_handling("bind() error");
    }

    if(listen(serv_sock,5)==-1)
    {
        error_handling("listen() error");
    }
    clnt_addr_size=sizeof(clnt_addr);
    clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
    //创建并返回一个用于数据I/O的socket  clnt_sock
    //如果在调用accept函数时进入阻塞状态，直到客户端调用connet函数为止
    
    if(clnt_sock==-1)
    {
        error_handling("accept() error");
    }

    write(clnt_sock,message.data(),message.size());//std::string.data() returns a pointer to the first character of a string
    //ssize_t write(size_t count;int fd,const void buf[count],size_t count);
    //writes up to [count] bytes from the buffer starting at [buf] to the file reffered to by the file descriptor [fd]
    close(clnt_sock);
    close(serv_sock);
    std::cout<<"--------------------------------------------------"<<std::endl;
}
