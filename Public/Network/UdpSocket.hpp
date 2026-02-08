#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>
#include <string>

namespace Library::Network
{
    class UdpSocket
    {
    public:
        UdpSocket();
        ~UdpSocket();

        bool bind(uint16_t port);

        std::optional<int> sendTo(std::string const & host, uint16_t port, void const * data, size_t size);
        std::optional<int> recvFrom(std::string & host, uint16_t & port, void * data, size_t size);

        bool waitRead(int timeoutMs);
        bool waitWrite(int timeoutMs);

        void shutdown();

    private:
        int fd;
    };
}