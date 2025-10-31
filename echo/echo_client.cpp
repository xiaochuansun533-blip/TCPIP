#include<iostream>
#include "util.h"
#include<string>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<cstring>
#include<string_view>
int main(int argc,char*argv[])
{
    int sock;
    std::string message(BUF_SIZE,'\0');
    int str_len;
    struct sockaddr_in serv_adr;

    if(argc!=3)
    {
        std::cout<<"usage:"<<argv[0]<<" <IP> <port>"<<std::endl;
        exit(1);
    }

    sock=socket(PF_INET,SOCK_STREAM,0);
    if(sock==-1)
    {
        error_handling("socket() error");
    }

    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_adr.sin_port=htons(atoi(argv[2]));

    if(connect(sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1)
    {
        error_handling("connect() error");
    }
    else
    {
        puts("connected...");
        //  int puts(const char * str);
        //writes every character from the null-terminated string [str] and one additional character '\n' to the output stream stdout
    }

    while (true)
    {
        std::cout<<"input message(Q to quit):"<<std::flush;
        //int fputs(const char* restrict str,FILE* restrict stream);
        //writes every character from the null-terminated string [str] to output stream [stream]
        if(!std::getline(std::cin,message))
        {
            break;
        }
        //char* fgets(char* restrict str,int count,FILE* restrict stream);
        //reads at most [count-1] characters from the given file [stream] and stores them in the character array pointed to by [str].
        //parsing stops if a newline character is found (in which case [str] will contain that newline character) or if end-of-file occurs.  
        //if bytes are read and no errors occur, writes a null character at the position immediately after the last character written to [str] 
        
        if(message=="q"|| message=="Q")
        {
            break;
        }

        if(write(sock,&message[0],message.size())==-1)
        {
            error_handling("write() error");
        }
        
        str_len=recv(sock,&message[0],BUF_SIZE,0);
        //is used to receive messages from a socket
        //may be used to receive data on both connectionless and connection-oriented sockets

        if(str_len<0)
        {
            error_handling("recv() error");
        }
        if(str_len==0)
        {
            std::cout<<"server closed\n";
            break;
        }
        message.resize((size_t)str_len);
        std::cout<<"message from server:"<<message<<std::endl;
    }
    close(sock);
}