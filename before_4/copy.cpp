#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string>
#include<cstring>
#include "util.h"
#include<iostream>

void my_copy(const std::string &path_source,const std::string &path_target)
{
    int fd_source;
    int fd_target;
    std::string message(BUF_SIZE,'\n');
    fd_source=open(path_source.data(),O_RDWR,0644);//打开文件

    if(fd_source==-1)
    {
        error_handling("open source file error");
    }

    std::cout<<"file descriptor:"<<fd_source<<std::endl;

    if(read(fd_source,&message[0],sizeof(message))==-1)
    {
        error_handling("read() error!");
    }
    
    fd_target=open(path_target.data(),O_CREAT|O_WRONLY|O_TRUNC,0644);//打开文件

    if(fd_target==-1)
    {
        error_handling("open target file error");
    }

    std::cout<<"file descriptor:"<<fd_target<<std::endl;

    if(write(fd_target,&message[0],sizeof(message))==-1)
    {
        error_handling("write error!");
    }

    close(fd_source);//关闭文件
    close(fd_target);
}
int main()
{
    std::string path_source="data.txt";
    std::string path_target="target.txt";
    my_copy(path_source,path_target);
}