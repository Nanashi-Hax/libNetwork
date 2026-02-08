#include "Network/TcpSocket.hpp"

#include <optional>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <cstring>

namespace Library::Network
{
    TcpSocket::TcpSocket()
    {
        fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(fd < 0) return;

        int opt = 1;
        ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
    }

    TcpSocket::~TcpSocket()
    {
        if(fd < 0) return;

        ::close(fd);
    }

    bool TcpSocket::listen(uint16_t port)
    {
        if(fd < 0) return false;

        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);

        int res = ::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if(res < 0) return false;

        res = ::listen(fd, 64);
        if(res < 0) return false;
        
        return true;
    }

    std::optional<TcpSocket> TcpSocket::accept()
    {
        if(fd < 0) return std::nullopt;

        sockaddr addr;
        socklen_t size = sizeof(addr);
        int accepted = ::accept(fd, &addr, &size);
        if(accepted < 0) return std::nullopt;

        int flags = ::fcntl(accepted, F_GETFL, 0);
        if (flags < 0)  return std::nullopt;
        flags |= O_NONBLOCK;
        int res = ::fcntl(accepted, F_SETFL, flags | O_NONBLOCK);
        if(res < 0)  return std::nullopt;

        int v = 1;
        res = ::setsockopt(accepted, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v));
        if(res < 0) return std::nullopt;

        return TcpSocket(accepted);
    }

    bool TcpSocket::connect(std::string host, uint16_t port)
    {
        if(fd < 0) return false;

        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(host.c_str());
        addr.sin_port = htons(port);

        int res = ::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if(res < 0)
        {
            if(errno != EINPROGRESS) return false;
        }

        int flags = ::fcntl(fd, F_GETFL, 0);
        if (flags < 0)  return false;
        flags |= O_NONBLOCK;
        res = ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        if(res < 0)  return false;

        int v = 1;
        res = ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v));
        if(res < 0) return false;

        return true;
    }


    std::optional<int> TcpSocket::send(const void* data, size_t size)
    {
        if(fd < 0) return std::nullopt;

        int res = ::send(fd, data, size, 0);
        if(res < 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK) return 0;
            return std::nullopt;
        }

        return res;
    }

    std::optional<int> TcpSocket::recv(void* data, size_t size)
    {
        if(fd < 0) return std::nullopt;

        int res = ::recv(fd, data, size, 0);
        if(res < 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK) return 0;
            return std::nullopt;
        }

        return res;
    }


    bool TcpSocket::waitRead(int timeoutMs)
    {
        if(fd < 0) return false;

        pollfd pfd{};
        pfd.fd = fd;
        pfd.events = POLLIN;

        int res = ::poll(&pfd, 1, timeoutMs);
        if(res < 0) return false;
        return (res > 0 && (pfd.revents & POLLIN));
    }

    bool TcpSocket::waitWrite(int timeoutMs)
    {
        if(fd < 0) return false;

        pollfd pfd{};
        pfd.fd = fd;
        pfd.events = POLLOUT;

        int res = ::poll(&pfd, 1, timeoutMs);
        if(res < 0) return false;
        return (res > 0 && (pfd.revents & POLLOUT));
    }


    void TcpSocket::shutdown()
    {
        if(fd < 0) return;
        
        ::shutdown(fd, SHUT_RDWR);
        ::close(fd);
        fd = -1;
    }

    TcpSocket::TcpSocket(int fd) : fd(fd) {}
}