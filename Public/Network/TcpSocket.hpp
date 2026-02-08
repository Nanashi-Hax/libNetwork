#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>
#include <string>

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

        std::optional<int> send(const void*, size_t);
        std::optional<int> recv(void*, size_t);

        bool waitRead(int timeoutMs);
        bool waitWrite(int timeoutMs);

        void shutdown();

    private:
        TcpSocket(int fd);
        int fd;
    };
}