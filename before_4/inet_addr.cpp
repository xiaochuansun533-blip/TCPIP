#include<iostream>
#include<arpa/inet.h>
#include<string>
int main()
{

    std::string addr1="1.2.3.4";
    std::string addr2="1.2.3.256";
    //一个字节是八位，那么最大表示的整数是255，所以addr2是错误的IP地址
    unsigned long conv_addr=inet_addr(&addr1[0]);//inet_addr函数将IP地址字符串转换为32位网络字节序
    if(conv_addr==INADDR_NONE)
    {
        std::cout<<"error occured!"<<std::endl;
    }
    else
    {
        std::cout<<"network ordered integer addr"<<std::hex<<conv_addr<<std::endl;
    }
    conv_addr=inet_addr(&addr2[0]);
    if(conv_addr==INADDR_NONE)
    {
        std::cout<<"error occured!"<<std::endl;
    }
    else
    {
        std::cout<<"network ordered integer addr"<<std::hex<<conv_addr<<std::endl;
    }
}