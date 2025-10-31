#include<iostream>
#include<string>
#include<arpa/inet.h>

int main()
{
    struct  sockaddr_in addr1,addr2;
    char *str_ptr;
    std::string str_arr(20,'\0');
    addr1.sin_addr.s_addr=htonl(0x1020304);
    addr2.sin_addr.s_addr=htonl(0x1010101);
}