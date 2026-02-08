#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>
#include <string>
#include "Network/Result.hpp"

namespace Library::Network
{
    class TcpSocket
    {
    public:
        TcpSocket();
        ~TcpSocket();

        bool listen(uint16_t port);
        std::optional<TcpSocket> accept();
        bool connect(std::string host, uint16_t port);

        Result send(const void*, size_t);
        Result recv(void*, size_t);

        bool waitRead(int timeoutMs);
        bool waitWrite(int timeoutMs);

        void shutdown();

    private:
        TcpSocket(int fd);
        int fd;
    };
}