#include "util.h"

#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>

// 字节序/套接字（放在 .cpp，避免污染头文件）
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " <port>\n";
        return 1;
    }

    // 1) 创建监听 socket
    int serv_sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) error_handling("socket() error");

    int yes = 1;
    ::setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    // 2) 绑定地址
    sockaddr_in serv_adr{};
    serv_adr.sin_family      = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port        = htons(static_cast<uint16_t>(std::stoi(argv[1])));

    if (::bind(serv_sock, (sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");

    // 3) 监听
    if (::listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    std::cout << "server listening on port " << std::stoi(argv[1]) << " ...\n";

    // 简单：处理 5 个客户端后退出
    for (int i = 0; i < 5; ++i) {
        //创建客户端socket
        sockaddr_in clnt_adr{};
        socklen_t clnt_len = sizeof(clnt_adr);
        int clnt_sock = ::accept(serv_sock, (sockaddr*)&clnt_adr, &clnt_len);
        if (clnt_sock == -1) {
            std::cerr << "accept error\n";
            --i; // 不计数，继续
            continue;
        }

        // 读 1 字节 cnt
        uint8_t cnt = 0;
        //uint8_t无符号8位1字节
        if (!recv_all(clnt_sock, &cnt, 1)) {
            ::close(clnt_sock);
            continue;
        }

        // 长度校验
        const std::size_t need = static_cast<std::size_t>(cnt) * OPSZ + 1;
        if (need > BUF_SIZE) {
            std::cerr << "too many operands: " << int(cnt) << '\n';
            ::close(clnt_sock);
            continue;
        }

        // 读满剩余负载
        std::vector<uint8_t> buf(need);
        if (!recv_all(clnt_sock, buf.data(), buf.size())) {
            ::close(clnt_sock);
            continue;
        }

        std::vector<int> operands(cnt);
        for (uint8_t k = 0; k < cnt; ++k) {
            uint32_t netv;
            //uint32_t  无符号精确32位  4字节
            std::memcpy(&netv, &buf[k * 4], 4);
            //按字节拷贝内存块的函数
            //ntohl  网络序转换成主机序
            operands[k] = static_cast<int>(ntohl(netv));
        }
        char op = static_cast<char>(buf.back());

        // 计算
        int result = calculate(cnt, operands.data(), op);

        // 回写 4 字节网络序结果
        //uint32_t
        uint32_t net_res = htonl(static_cast<uint32_t>(result));
        if (!send_all(clnt_sock, &net_res, sizeof(net_res))) {
            std::cerr << "send result failed\n";
        }
        
        ::close(clnt_sock);
    }

    ::close(serv_sock);
    return 0;
}
