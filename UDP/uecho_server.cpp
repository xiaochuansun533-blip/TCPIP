#include <iostream>
#include <array>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "util.h"
static void error_handling(const char* msg) { perror(msg); std::exit(1); }

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <port>\n";
        return 1;
    }

    int serv_sock = ::socket(PF_INET, SOCK_DGRAM, 0);
    if (serv_sock == -1) {
        error_handling("UDP socket creation error");
    }

    sockaddr_in serv_adr{};
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(static_cast<uint16_t>(std::stoi(argv[1])));

    if (bind(serv_sock, reinterpret_cast<sockaddr*>(&serv_adr), sizeof(serv_adr)) == -1) {
        error_handling("bind() error");
    }

    std::array<char, BUF_SIZE> message{};
    sockaddr_in clnt_adr{};
    socklen_t clnt_adr_sz = sizeof(clnt_adr);

    while (true) {
        ssize_t str_len = recvfrom(serv_sock, message.data(), message.size(), 0,
                                   reinterpret_cast<sockaddr*>(&clnt_adr), &clnt_adr_sz);
        if (str_len < 0) error_handling("recvfrom() error");

        // 原样回声
        if (sendto(serv_sock, message.data(), str_len, 0,
                   reinterpret_cast<sockaddr*>(&clnt_adr), clnt_adr_sz) < 0) {
            error_handling("sendto() error");
        }
    }

    close(serv_sock);
    return 0;
}
