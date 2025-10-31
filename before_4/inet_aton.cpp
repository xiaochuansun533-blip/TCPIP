#include<iostream>
#include<stdlib.h>
#include<arpa/inet.h>
#include<string>
#include "util.h"
int main()
{
    std::string addr="127.232.124.79";
    struct  sockaddr_in addr_inet;

    if(!inet_aton(&addr[0],&addr_inet.sin_addr))//将IP地址字符串转换成
    {
        error_handling("conversion error");
    }
    else
    {
        std::cout<<"network ordered integer addr:"<<std::hex<<addr_inet.sin_addr.s_addr<<std::endl;
    }
}