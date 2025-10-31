#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string>
#include<cstring>
#include "util.h"
#include<iostream>
int main(void)
{
    int fd;
    std::string buf="Let't go!\n";
    fd=open("data.txt",O_CREAT|O_WRONLY|O_TRUNC,0644);
    //int open(const char*path,int flags,/*mode_t mode*/)
    //the [mode] argument specifies the file mode bits to be applied when a new file is created.
    //the [mode] argument must be supplied if O_CREAT or O_TMPFILE is specified in [flags]
    //file mode bits 文件权限位 保存在incode的st_mode字段里、用来控制“谁能对这个文件/目录做什么”的一组二进制位
    if(fd==-1)
    {
        error_handling("open() error");
    }
    std::cout<<"file descriptor:"<<fd<<std::endl;

    if(write(fd,&buf[0],sizeof(buf))==-1)
    {
        error_handling("write() error!");
    }
    
    close(fd);
    std::cout<<"--------------"<<std::endl;
    return 0;
}