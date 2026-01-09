// ConnectImpl.cpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <system_error>

#include "ConnectImpl.hpp"

namespace Network::Tcp::Impl
{
    Acceptor::Acceptor(uint16_t port) : port(port)
    {
        fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(fd < 0) throw std::system_error(errno, std::generic_category(), "socket()");

        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);

        int res = ::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if(res < 0)
        {
            ::close(fd);
            throw std::system_error(errno, std::generic_category(), "bind()");
        }

        res = ::listen(fd, 64);
        if(res < 0)
        {
            ::close(fd);
            throw std::system_error(errno, std::generic_category(), "listen()");
        }
    }

    Acceptor::~Acceptor()
    {
        if(fd >= 0) ::close(fd);
    }

    Socket Acceptor::accept()
    {
        if(fd < 0) throw std::logic_error("Accepter is dead.");
        sockaddr addr;
        socklen_t size = sizeof(addr);
        int res = ::accept(fd, &addr, &size);
        if(res < 0)
        {
            ::close(fd);
            throw std::system_error(errno, std::generic_category(), "accept()");
        }
        return Socket(res);
    }

    Connector::Connector(std::string host, uint16_t port) : host(host), port(port)
    {
        fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(fd < 0) throw std::system_error(errno, std::generic_category(), "socket()");
    }

    Connector::~Connector()
    {
        if(fd >= 0) ::close(fd);
    }

    Socket Connector::connect()
    {
        if(fd < 0) throw std::logic_error("Connector is dead.");
        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(host.c_str());
        addr.sin_port = htons(port);

        int res = ::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if(res < 0)
        {
            ::close(fd);
            throw std::system_error(errno, std::generic_category(), "connect()");
        }
        Socket socket(fd);
        fd = -1;
        return socket;
    }
}