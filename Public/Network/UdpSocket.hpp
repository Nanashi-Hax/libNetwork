#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>
#include <string>
#include "Network/Result.hpp"

namespace Library::Network
{
    class UdpSocket
    {
    public:
        UdpSocket();
        ~UdpSocket();

        bool bind(uint16_t port);

        Result sendTo(std::string const & host, uint16_t port, void const * data, size_t size);
        Result recvFrom(std::string & host, uint16_t & port, void * data, size_t size);

        bool waitRead(int timeoutMs);
        bool waitWrite(int timeoutMs);

        void shutdown();

    private:
        int fd;
    };
}