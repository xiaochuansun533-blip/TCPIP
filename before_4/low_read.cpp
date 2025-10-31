#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string>
#include<cstring>
#include "util.h"
#include<iostream>
int main()
{
    int fd;
    std::string buf(BUF_SIZE,'\0');
    
    fd=open("data.txt",O_RDONLY);


    if(fd==-1)
    {
        error_handling("open() error!");
    }
    std::cout<<"file descriptor:"<<fd<<std::endl;

    if(read(fd,&buf[0],sizeof(buf))==-1)
    {
        error_handling("read() error!");
    }
    
    
    
    std::cout<<"file data:"<<buf<<std::endl;

    close(fd);
}