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
    std::cout<<"----------------------------------------"<<std::endl;
    
    int serv_sock;
    int clnt_sock;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;
    std::string message="hello world!";
    int write_len;
    int idx=0;
    int str_len=0;
    if(argc!=2)
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
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
    {
        error_handling("bind() error");
    }

    if(listen(serv_sock,5)==-1)
    {
        error_handling("listen() error");
    }
    clnt_addr_size=sizeof(clnt_addr);
    clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);

    if(clnt_sock==-1)
    {
        error_handling("accept() error");
    }
    uint8_t net_str_len;
    
    if(!recv_all(clnt_sock,&net_str_len,1))
    {
        close(clnt_sock);
        close(serv_sock);
    }
    uint8_t str_len=static_cast<uint8_t>(ntohl(net_str_len));

    std::cout<<"the length of string is:"<<str_len<<std::endl;
    char message[str_len];
    int recv_len=0;
    for (size_t i = 0; i < str_len; i++)
    {
        int recv_char;
        int recv_cnt=recv_all(clnt_sock,&recv_char,sizeof(char));
        if(recv_cnt==-1)
        {
            error_handling("recv_all() error");
        }   
        message[i]=ntohl(recv_char);
    }
    message[recv_len]=0;
    std::cout<<"message from client:"<<message<<std::endl;
    close(clnt_sock);
    close(serv_sock);
    std::cout<<"--------------------------------------------------"<<std::endl;
}
