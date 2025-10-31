#include "util.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include<endian.h>
//传参文件路径
static std::string basename_like(const std::string& path) {
    std::size_t p = path.find_last_of("/\\");
    //find_last_of找到最后一个出现在字符集里的字符的位置
    if (p == std::string::npos) return path;
    return path.substr(p + 1);//返回文件名（带扩展名）
}

//static放在全局/命名空间作用域的函数前，表示内部链接，只在本.cpp可见
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "usage: " << argv[0] << " <server_ip> <port>\n";
        return 1;
    }

    int cfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (cfd == -1) error_handling("socket() error");

    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(static_cast<uint16_t>(std::stoi(argv[2])));
    if (::inet_pton(AF_INET, argv[1], &serv.sin_addr) != 1)
        error_handling("inet_pton() error");
    if (::connect(cfd, reinterpret_cast<sockaddr*>(&serv), sizeof(serv)) == -1)
        error_handling("connect() error");

    std::cout << "connected to " << argv[1] << ":" << argv[2] << '\n';


    std::cout << "Enter filename to download: ";
    std::string fname;
    if (!std::getline(std::cin, fname) || fname.empty())
        error_handling("invalid filename");

    if (fname.size() > MAX_NAME) error_handling("filename too long");

    std::uint16_t name_len = static_cast<std::uint16_t>(fname.size());
    std::uint16_t name_len_be = htons(name_len);

    if (!send_all(cfd, &name_len_be, sizeof(name_len_be)) ||!send_all(cfd, fname.data(), fname.size())) 
    {
        error_handling("send filename failed");
    }

    std::uint64_t fsize_be = 0;
    if (!recv_all(cfd, &fsize_be, sizeof(fsize_be))) 
    {
        std::cerr << "server closed: file not found or error.\n";
        ::close(cfd);
        return 1;
    }
    std::uint64_t fsize = be64toh(fsize_be);
    std::cout << "size = " << fsize << " bytes\n";

    std::string out_name = basename_like(fname) + ".download";
    std::ofstream fout(out_name, std::ios::binary | std::ios::trunc);
    //out_name输出文件路径
    //std::ios::binary二进制模式打开
    //std::ios::trunc  截断，如果文件已存在。打开时先清空为0，不存在就创建
    if (!fout) error_handling("open output file failed: " + out_name);

    std::vector<char> buf(IO_BUF);
    std::uint64_t received = 0;
    while (received < fsize) 
    {
        std::size_t want = static_cast<std::size_t>(std::min<std::uint64_t>(buf.size(), fsize - received));
        //每次去缓冲容量和剩余字节数最小的
        if (!recv_all(cfd, buf.data(), want)) 
        {
            error_handling("connection lost while receiving file");
        }
        fout.write(buf.data(), static_cast<std::streamsize>(want));
        received += want;
    }

    fout.close();
    ::close(cfd);

    std::cout << "saved to: " << out_name << '\n';
    return 0;
}
