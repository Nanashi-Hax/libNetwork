// ConnectImpl.hpp
#pragma once
#include "SocketImpl.hpp"

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
        SOCKET s;
        uint16_t port;
    };

    class Connector
    {
    public:
        Connector(std::string host, uint16_t port);
        ~Connector();

        Socket connect();

    private:
        SOCKET s;
        std::string host;
        uint16_t port;
    };
}