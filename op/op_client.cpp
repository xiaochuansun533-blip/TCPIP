#include "util.h"

#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>

// 字节序/套接字（放在 .cpp）
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cout << "usage: " << argv[0] << " <IP> <port>\n";
        return 1;
    }

    int sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) error_handling("socket() error");

    sockaddr_in serv_adr{};
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port   = htons(static_cast<uint16_t>(std::stoi(argv[2])));
    if (::inet_pton(AF_INET, argv[1], &serv_adr.sin_addr) != 1)
        error_handling("inet_pton() error");

    if (::connect(sock, (sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error");

    std::cout << "connected...............\n";

    int opnd_cnt = 0;
    std::cout << "operand count: ";
    if (!(std::cin >> opnd_cnt) || opnd_cnt <= 0)
        error_handling("invalid operand count");

    const std::size_t total = 1 + static_cast<std::size_t>(opnd_cnt) * OPSZ + 1;
    if (total > BUF_SIZE) error_handling("payload too large for BUF_SIZE");

    std::vector<uint8_t> msg(total);
    msg[0] = static_cast<uint8_t>(opnd_cnt);

    for (int i = 0; i < opnd_cnt; ++i) 
    {
        long long v = 0;
        std::cout << "operand " << (i + 1) << ": ";
        if (!(std::cin >> v)) error_handling("invalid operand");
        uint32_t v_net = htonl(static_cast<uint32_t>(v));
        std::memcpy(&msg[1 + i * OPSZ], &v_net, 4);
    }

    std::cout << "operator (+ - * /): ";
    char op = 0;
    if (!(std::cin >> op)) error_handling("invalid operator");
    msg[1 + opnd_cnt * OPSZ] = static_cast<uint8_t>(op);

    if (!send_all(sock, msg.data(), msg.size()))
        error_handling("send payload error");

    uint32_t net_res = 0;
    if (!recv_all(sock, &net_res, sizeof(net_res)))
        error_handling("recv result error");

    int32_t result = static_cast<int32_t>(ntohl(net_res));
    std::cout << "operation result: " << result << '\n';

    ::close(sock);
    return 0;
}
