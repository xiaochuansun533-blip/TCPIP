#include "util.h"
#include<iostream>
#include<string>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<cstring>

int main(int argc,char*argv[])
{
    int serv_sock;
    int clnt_sock;
    int str_len,i;

    struct sockaddr_in serv_adr,clnt_adr;
    socklen_t clnt_adr_sz;
    std::string  message(BUF_SIZE,'\0');
    if(argc!=2)
    {
        std::cout<<"usage:"<<argv[0]<<std::endl;
        exit(1);
    }

    serv_sock=socket(PF_INET,SOCK_STREAM,0);
    if(serv_sock==-1)
    {
        error_handling("socket() error!");
    }

    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port=htons(atoi(argv[1]));

    if(bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1)
    {
        error_handling("bind() error");
    }

    if(listen(serv_sock,5)==-1)
    {
        error_handling("listen() error");
    }

    clnt_adr_sz=sizeof(clnt_adr);

    for (size_t i = 0; i < 5; i++)
    {
        clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);
        if(clnt_sock==-1)
        {
            error_handling("accept() error");
        }
        else
        {
            std::cout<<"connected client"<<i+1<<std::endl;
        }

        while ((str_len=read(clnt_sock,&message[0],BUF_SIZE))!=0)
        {
            write(clnt_sock,&message[0],str_len);
        }

        close(clnt_sock);
    }
    close(serv_sock);
}