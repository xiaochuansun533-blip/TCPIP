#include "util.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
int main()
{
    int fd1,fd2,fd3;
    fd1=socket(PF_INET,SOCK_STREAM,0);
    fd2=open("test.dat",O_CREAT|O_WRONLY|O_TRUNC,0644);
    fd3=socket(PF_INET,SOCK_DGRAM,0);

    std::cout<<"file descriptor 1:"<<fd1<<std::endl;
    std::cout<<"file descriptor 2:"<<fd2<<std::endl;
    std::cout<<"file descriptor 3:"<<fd3<<std::endl;

    close(fd1);
    close(fd2);
    close(fd3);
}