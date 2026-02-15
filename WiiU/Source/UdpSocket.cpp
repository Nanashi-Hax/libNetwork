#include "Network/UdpSocket.hpp"
#include "Network/Result.hpp"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <cstring>
#include <system_error>

namespace Library::Network
{
    UdpSocket::UdpSocket()
    {
        fd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(fd < 0) throw std::system_error(errno, std::generic_category(), "socket()");

        int opt = 1;
        int res = ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
        if(res < 0) throw std::system_error(errno, std::generic_category(), "setsockopt()");

        opt = 1;
        res = ::setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));
        if(res < 0) throw std::system_error(errno, std::generic_category(), "setsockopt()");

        int flags = ::fcntl(fd, F_GETFL, 0);
        if (flags < 0) return;
        flags |= O_NONBLOCK;
        res = ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        if(res < 0) throw std::system_error(errno, std::generic_category(), "fcntl()");
    }

    UdpSocket::~UdpSocket()
    {
        if(fd < 0) return;

        ::close(fd);
    }

    bool UdpSocket::bind(uint16_t port)
    {
        if(fd < 0) throw std::logic_error("sockfd is invalid");

        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);
        
        int res = ::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if(res < 0) throw std::system_error(errno, std::generic_category(), "bind()");

        return true;
    }

    Result UdpSocket::sendTo(std::string const & host, uint16_t port, void const * data, size_t size) noexcept
    {
        if(fd < 0) return {ResultType::Error, 0};

        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(host.c_str());
        addr.sin_port = htons(port);

        int res = ::sendto(fd, data, size, 0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if(res < 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK) return {ResultType::WouldBlock, 0};
            return {ResultType::Error, 0};
        }
        
        return {ResultType::Data, static_cast<size_t>(res)};
    }

    Result UdpSocket::recvFrom(std::string & host, uint16_t & port, void * data, size_t size) noexcept
    {
        if(fd < 0) return {ResultType::Error, 0};

        sockaddr_in addr;
        socklen_t addrSize = sizeof(addr);

        int res = ::recvfrom(fd, data, size, 0, reinterpret_cast<sockaddr*>(&addr), &addrSize);
        if(res < 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK) return {ResultType::WouldBlock, 0};
            return {ResultType::Error, 0};
        }

        host = inet_ntoa(addr.sin_addr);
        port = ntohs(addr.sin_port);
        return {ResultType::Data, static_cast<size_t>(res)};
    }

    bool UdpSocket::waitRead(int timeoutMs) noexcept
    {
        if(fd < 0) return false;

        pollfd pfd{};
        pfd.fd = fd;
        pfd.events = POLLIN;

        int ret = poll(&pfd, 1, timeoutMs);
        if(ret < 0) return false;
        return (ret > 0 && (pfd.revents & POLLIN));
    }

    bool UdpSocket::waitWrite(int timeoutMs) noexcept
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