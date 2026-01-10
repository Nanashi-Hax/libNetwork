// ConnectImpl.hpp
#pragma once
#include "Tcp/SocketImpl.hpp"

#include <cstdint>
#include <string>

namespace Network::Tcp::Impl
{
    class Acceptor
    {
    public:
        Acceptor(uint16_t port);
        ~Acceptor();

        Socket accept();

    private:
        int fd;
        uint16_t port;
    };

    class Connector
    {
    public:
        Connector(std::string host, uint16_t port);
        ~Connector();

        Socket connect();

    private:
        int fd;
        std::string host;
        uint16_t port;
    };
}