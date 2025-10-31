#include "util.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cerrno>
#include <cstring>
#include <cstdlib>

// POSIX（仅在此实现文件中依赖）
#include <unistd.h>
#include <sys/socket.h>

[[noreturn]] void error_handling(const std::string& message) {
    std::cerr << message << '\n';
    std::exit(1);
}

// C++ 文件流版本的文件拷贝（二进制安全，大文件 OK）
void copy(const std::string& path_source, const std::string& path_target)
{
    std::ifstream in(path_source, std::ios::binary);
    if (!in) error_handling("open source file error: " + path_source);

    std::ofstream out(path_target, std::ios::binary | std::ios::trunc);
    if (!out) error_handling("open target file error: " + path_target);

    std::vector<char> buf(BUF_SIZE);
    while (in) {
        in.read(buf.data(), static_cast<std::streamsize>(buf.size()));
        std::streamsize n = in.gcount();
        if (n > 0) out.write(buf.data(), n);
    }
    if (!out) error_handling("write error while copying to: " + path_target);
}

void busy_waiting(void)
{
    for (std::size_t i = 0; i < 1000; ++i)
        std::cout << "wait time " << i << '\n';
}

int calculate(int opnum, int opnds[], char op)
{
    if (opnum <= 0) return 0;
    long long acc = opnds[0];
    switch (op) {
        case '+': for (int i = 1; i < opnum; ++i) acc += opnds[i]; break;
        case '-': for (int i = 1; i < opnum; ++i) acc -= opnds[i]; break;
        case '*': for (int i = 1; i < opnum; ++i) acc *= opnds[i]; break;
        case '/':
            for (int i = 1; i < opnum; ++i) {
                if (opnds[i] == 0) error_handling("divide by zero");
                acc /= opnds[i];
            }
            break;
        default: error_handling("unknown operator");
    }
    return static_cast<int>(acc);
}

bool recv_all(int fd, void* vbuf, std::size_t n)
{//fd写入buf
    std::size_t left = n;
    char* p = static_cast<char*>(vbuf);
    while (left > 0) {
        ssize_t r = ::recv(fd, p, left, 0);

        if (r == 0) return false;             
        if (r < 0) {
            if (errno == EINTR) continue;        
            return false;
        }
        left -= static_cast<std::size_t>(r);
        p    += r;//移动p指针指向的地址
    }
    return true;
}

bool send_all(int fd, const void* vbuf, std::size_t n)
{//buf写入fd
    std::size_t left = n;
    const char* p = static_cast<const char*>(vbuf);
    while (left > 0) {
        ssize_t r = ::send(fd, p, left, 0);
        if (r <= 0) {
            if (r < 0 && errno == EINTR) continue;
            return false;
        }
        left -= static_cast<std::size_t>(r);
        p    += r;
    }
    return true;
}


 std::uint64_t get_file_size(std::ifstream &fin)
{
        //std::ifstream  输入文件流，专门从文件读取字节/文本
        //fin是创建的文件流对象的变量名
        //fname要打开的文件路径
        //std::ios::binary 以二进制模式打开，不做任何换行/编码转换
    fin.seekg(0,std::ios::end);
        //seekg(off,dif) 将读指针移到dir相对的off偏移处
        //那么距离文件末尾是0，那么读指针处于文件末尾
        //将读指针移到文件末尾
    std::uint64_t fsize=static_cast<std::uint64_t>(fin.tellg());
    //tellog 返回当前读指针距离文件开头的距离
    
    fin.seekg(0,std::ios::beg);
    //回到文件开头

    return fsize;
}