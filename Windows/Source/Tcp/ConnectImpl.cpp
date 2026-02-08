// ConnectImpl.cpp
#include <winsock2.h>

#include <cstring>
#include <system_error>

#include "Tcp/ConnectImpl.hpp"

namespace Library::Network::Tcp::Impl
{
    Acceptor::Acceptor(uint16_t port) : port(port)
    {
        s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(s == INVALID_SOCKET) throw std::system_error(WSAGetLastError(), std::system_category(), "socket()");

        int opt = 1;
        int res = ::setsockopt(s, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
        if(res == SOCKET_ERROR) throw std::system_error(WSAGetLastError(), std::system_category(), "setsockopt(SO_REUSEADDR)");

        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);

        res = ::bind(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if(res == SOCKET_ERROR)
        {
            ::closesocket(s);
            throw std::system_error(WSAGetLastError(), std::system_category(), "bind()");
        }

        res = ::listen(s, 64);
        if(res == SOCKET_ERROR)
        {
            ::closesocket(s);
            throw std::system_error(WSAGetLastError(), std::system_category(), "listen()");
        }
    }

    Acceptor::~Acceptor()
    {
        if(s != INVALID_SOCKET) ::closesocket(s);
    }

    Socket Acceptor::accept()
    {
        if(s == INVALID_SOCKET) throw std::logic_error("Accepter is dead.");
        sockaddr addr;
        int size = sizeof(addr);
        SOCKET accepted = ::accept(s, &addr, &size);
        if(accepted == INVALID_SOCKET)
        {
            throw std::system_error(WSAGetLastError(), std::system_category(), "accept()");
        }
        return Socket(accepted);
    }

    void Acceptor::shutdown()
    {
        if(s == INVALID_SOCKET) throw std::logic_error("Acceptor is dead.");

        ::shutdown(s, SD_BOTH);
    }

    Connector::Connector(std::string host, uint16_t port) : host(host), port(port)
    {
        s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(s == INVALID_SOCKET) throw std::system_error(WSAGetLastError(), std::system_category(), "socket()");
    }

    Connector::~Connector()
    {
        if(s != INVALID_SOCKET) ::closesocket(s);
    }

    Socket Connector::connect()
    {
        if(s == INVALID_SOCKET) throw std::logic_error("Connector is dead.");
        
        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(host.c_str());
        addr.sin_port = htons(port);

        int res = ::connect(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if(res < 0)
        {
            throw std::system_error(WSAGetLastError(), std::system_category(), "connect()");
        }
        Socket socket(s);
        s = INVALID_SOCKET;
        return socket;
    }

    void Connector::shutdown()
    {
        if(s == INVALID_SOCKET) throw std::logic_error("Connector is dead.");

        ::shutdown(s, SD_BOTH);
    }
}