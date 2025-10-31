//以下创建请求连接的客户端socket
//先编译运行接收请求的服务器端代码
//再运行发送请求的客户端代码
//编译运行的命令
/*
g++ -std=c++11 hello_client.cpp util.cpp -o hello_client
./hello_client 127.0.0.1 9090
*/
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<cstring>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<iostream>
#include "util.h" 
int main(int argc,char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    std::string message(1024,'\0');
    int str_len;

    if(argc!=3)//输入IP地址 端口号
    {
        std::cout<<"Usage: "<<argv[0]<<" <IP> "<<"<port>\n"<<std::endl;
        exit(1);
    }
    sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);//创建socket，如果调用bind,listen函数，成为服务器端socket，
    //PF_INET  IPv4互联网协议族
    //SOCK_STREAM面向连接的socket，不存在数据边界，收发数据的socket内部有缓冲(buffer)，也就是字节数组，通过socket传输的数据将保存在该数组里
    //只要不超过该数组容量，通过一次read()就可以调用读取全部，所以不存在数据边界
    //如果调用connect函数，成为客户端socket
    //socket连接必须一一对应
    //面向连接的套接字只能与另外一个同样特性的套接字连接
    //唯一属于IPv4协议族的并且面向连接的协议是IPPROTO_TCP
    if(sock==-1)
    {
        error_handling("socket() error");
    }


    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);//将字符串形式的IP地址转换成32位整数型数据
    serv_addr.sin_port=htons(atoi(argv[2]));//将主机字节序的端口号转换成网络字节序的端口号

    if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)//连接服务器端的IP地址和端口号
    {
        error_handling("connect() error!");
    }

    str_len=read(sock,&message[0],sizeof(message));
    //ssize_t read(size_t count;int fd,void buf[count],size_t count);
    //read() attempts to read up to [count] bytes from file descriptor [fd] into the buffer starting at [buf] 
    // the number of bytes read is returned
    //必须是原始数组，不能是std::string  对象头

    message.resize(str_len);
    if(str_len==-1)
    {
        error_handling("read() error!");
    }

    std::cout<<"message from server:"<<message<<std::endl;
    close(sock);
    
}