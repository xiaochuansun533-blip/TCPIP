#include "util.h"
#include <iostream>
#include <string>
#include <array>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "usage : " << argv[0] << " <IP> <port>\n";
        return 1;
    }

    int sock = ::socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        error_handling("socket() error");
    }

    sockaddr_in serv_adr{};
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port   = htons(static_cast<uint16_t>(std::stoi(argv[2])));
    // 更稳妥的地址转换：inet_pton 而不是 inet_addr
    if (inet_pton(AF_INET, argv[1], &serv_adr.sin_addr) != 1) {
        error_handling("inet_pton() error");
    }

    std::array<char, BUF_SIZE> message{};
    sockaddr_in from_adr{};
    socklen_t adr_sz = sizeof(from_adr);

    std::string line;
    while (true) {
        std::cout << "insert message(q to quit):";
        if (!std::getline(std::cin, line)) break;
        if (line == "q" || line == "Q") break;

        ssize_t sent = sendto(sock, line.data(), line.size(), 0,
                              reinterpret_cast<sockaddr*>(&serv_adr),
                              static_cast<socklen_t>(sizeof(serv_adr)));
        if (sent < 0) error_handling("sendto() error");

        adr_sz = sizeof(from_adr); 
        ssize_t n = recvfrom(sock, message.data(), message.size() - 1, 0,
                             reinterpret_cast<sockaddr*>(&from_adr), &adr_sz);
        if (n < 0) error_handling("recvfrom() error");

        message[n] = '\0';
        std::cout << "message from server: " << message.data();
        if (n == 0 || message[n - 1] != '\n') std::cout << '\n';
    }

    close(sock);
    return 0;
}
