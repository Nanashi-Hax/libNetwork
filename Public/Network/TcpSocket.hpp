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
        ~TcpSocket() noexcept;

        TcpSocket(const TcpSocket&) = delete;
        TcpSocket& operator=(const TcpSocket&) = delete;

        TcpSocket(TcpSocket&& other) noexcept;
        TcpSocket& operator=(TcpSocket&& other) noexcept;

        bool listen(uint16_t port) noexcept;
        std::optional<TcpSocket> accept() noexcept;
        bool connect(std::string host, uint16_t port) noexcept;

        Result send(const void*, size_t) noexcept;
        Result recv(void*, size_t) noexcept;

        bool waitRead(int timeoutMs) noexcept;
        bool waitWrite(int timeoutMs) noexcept;

        void shutdown() noexcept;

    private:
        TcpSocket(int fd) noexcept;
        int fd;
    };
}