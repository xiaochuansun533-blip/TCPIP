#pragma once
#include <cstddef>
#include <cstdint>
#include <string>

// 公共常量
constexpr std::size_t BUF_SIZE = 1024;
constexpr std::size_t OPSZ     = 4;   // 32-bit 整数 = 4字节
constexpr std::size_t RLT_SIZE = 4;   // 结果固定4字节（网络序）
constexpr std::size_t IO_BUF = 64 * 1024;   // 64 KiB
constexpr std::size_t MAX_NAME = 4096;      // 最大文件名长度（字节）
// 错误处理（打印到 std::cerr 并退出）
[[noreturn]] void error_handling(const std::string& message);

// 文件拷贝（C++ 文件流）
void copy(const std::string& path_source, const std::string& path_target);

// 演示用忙等
void busy_waiting();

// 计算器：对 opnds[0..opnum-1] 按 op 累积（+ - * /）
int calculate(int opnum, int opnds[], char op);

// —— 网络 I/O 的 C++ 风格包装（返回 bool，成功/失败）——
// 说明：这些是“高层 C++ 接口”，真正的系统调用隐藏在 util.cpp 里。
bool recv_all(int fd, void* buf, std::size_t n);
bool send_all(int fd, const void* buf, std::size_t n);
std::uint64_t get_file_size(std::ifstream &fin);
