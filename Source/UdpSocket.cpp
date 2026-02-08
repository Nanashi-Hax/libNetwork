#include "Network/UdpSocket.hpp"

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
    UdpSocket::UdpSocket()
    {
        fd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(fd < 0) return;

        int opt = 1;
        int res = ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
        if(res < 0) return;

        opt = 1;
        res = ::setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));
        if(res < 0) return;

        int flags = ::fcntl(fd, F_GETFL, 0);
        if (flags < 0) return;
        flags |= O_NONBLOCK;
        ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }

    UdpSocket::~UdpSocket()
    {
        if(fd < 0) return;

        ::close(fd);
    }

    bool UdpSocket::bind(uint16_t port)
    {
        if(fd < 0) return false;

        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);
        
        int res = ::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if(res < 0) return false;

        return true;
    }

    std::optional<int> UdpSocket::sendTo(std::string const & host, uint16_t port, void const * data, size_t size)
    {
        if(fd < 0) return std::nullopt;

        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(host.c_str());
        addr.sin_port = htons(port);

        int res = ::sendto(fd, data, size, 0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if(res < 0) return std::nullopt;
        return res;
    }

    std::optional<int> UdpSocket::recvFrom(std::string & host, uint16_t & port, void * data, size_t size)
    {
        if(fd < 0) return std::nullopt;

        sockaddr_in addr;
        socklen_t addrSize = sizeof(addr);

        int res = ::recvfrom(fd, data, size, 0, reinterpret_cast<sockaddr*>(&addr), &addrSize);
        if(res < 0) return std::nullopt;

        host = inet_ntoa(addr.sin_addr);
        port = ntohs(addr.sin_port);
        return res;
    }

    bool UdpSocket::waitRead(int timeoutMs)
    {
        if(fd < 0) return false;

        pollfd pfd{};
        pfd.fd = fd;
        pfd.events = POLLIN;

        int ret = poll(&pfd, 1, timeoutMs);
        if(ret < 0) return false;
        return (ret > 0 && (pfd.revents & POLLIN));
    }

    bool UdpSocket::waitWrite(int timeoutMs)
    {
        if(fd < 0) return false;

        pollfd pfd{};
        pfd.fd = fd;
        pfd.events = POLLOUT;

        int ret = poll(&pfd, 1, timeoutMs);
        if(ret < 0) return false;
        return (ret > 0 && (pfd.revents & POLLOUT));
    }

    void UdpSocket::shutdown()
    {
        if(fd < 0) return;

        ::shutdown(fd, SHUT_RDWR);
        ::close(fd);
        fd = -1;
    }
}