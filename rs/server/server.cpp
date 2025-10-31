#include "util.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdint>
#include <cstring>
#include<endian.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " <port>\n";
        return 1;
    }

    int lfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) error_handling("socket() error");

    int yes = 1;
    ::setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(static_cast<uint16_t>(std::stoi(argv[1])));

    if (::bind(lfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1)
        error_handling("bind() error");
    if (::listen(lfd, 16) == -1)
        error_handling("listen() error");

    std::cout << "server listening on port " << std::stoi(argv[1]) << " ...\n";

    while (true) {
        sockaddr_in cli{};
        socklen_t cl = sizeof(cli);
        int cfd = ::accept(lfd, reinterpret_cast<sockaddr*>(&cli), &cl);
        if (cfd < 0) { if (errno == EINTR) continue; std::cerr << "accept error\n"; continue; }
        //errno == EINTR  阻塞
        std::uint16_t name_len_be = 0;
        if (!recv_all(cfd, &name_len_be, sizeof(name_len_be))) {
            ::close(cfd); continue;
        }
        std::uint16_t name_len = ntohs(name_len_be);
        if (name_len == 0 || name_len > MAX_NAME) {
            ::close(cfd); continue;
        }

        std::string fname(name_len, '\0');
        if (!recv_all(cfd, &fname[0], fname.size())) {
            ::close(cfd); continue;
        }

        std::ifstream fin(fname, std::ios::binary);
        if (!fin) {
            ::close(cfd);
            continue;
        }
        
        std::uint64_t fsize =get_file_size(fin);
        std::uint64_t fsize_be = htobe64(fsize);
        if (!send_all(cfd, &fsize_be, sizeof(fsize_be))) {
            ::close(cfd); continue;
        }


        std::vector<char> buf(IO_BUF);
        //constexpr std::size_t IO_BUF = 64 * 1024;   // 64 KiB
        
        while (fin) {
            fin.read(buf.data(), static_cast<std::streamsize>(buf.size()));
            //fin.read(ptr,count)
            //从输入文件流fin里最多读取count个字节到内存地址ptr
            std::streamsize n = fin.gcount();
            //n代表真正读取的字节
            if (n > 0) {
                if (!send_all(cfd, buf.data(), static_cast<std::size_t>(n))) {
                    break;
                }
            }
        }

        ::close(cfd);
        std::cout << "sent file: " << fname << " (" << fsize << " bytes)\n";
    }
    ::close(lfd);
    return 0;
}
