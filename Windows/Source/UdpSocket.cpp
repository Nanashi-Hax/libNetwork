#include "Network/UdpSocket.hpp"
#include "Network/Result.hpp"

#include <winsock2.h>

#include <cstring>
#include <system_error>

namespace Library::Network
{
    UdpSocket::UdpSocket()
    {
        fd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(fd == INVALID_SOCKET) throw std::system_error(WSAGetLastError(), std::system_category(), "socket()");

        int opt = 1;
        int res = ::setsockopt(fd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, reinterpret_cast<const char*>(&opt), sizeof(opt));
        if(res == SOCKET_ERROR) throw std::system_error(WSAGetLastError(), std::system_category(), "setsockopt()");

        opt = 1;
        res = ::setsockopt(fd, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(&opt), sizeof(opt));
        if(res == SOCKET_ERROR) throw std::system_error(WSAGetLastError(), std::system_category(), "setsockopt()");

        u_long mode = 1;
        res = ::ioctlsocket(fd, FIONBIO, &mode);
        if (res == SOCKET_ERROR) throw std::system_error(WSAGetLastError(), std::system_category(), "fcntl()");
    }

    UdpSocket::~UdpSocket()
    {
        if(fd == INVALID_SOCKET) return;

        ::closesocket(fd);
    }

    bool UdpSocket::bind(uint16_t port)
    {
        if(fd == INVALID_SOCKET) throw std::logic_error("sockfd is invalid");

        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);
        
        int res = ::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if(res == SOCKET_ERROR) throw std::system_error(WSAGetLastError(), std::system_category(), "bind()");

        return true;
    }

    Result UdpSocket::sendTo(std::string const & host, uint16_t port, void const * data, size_t size) noexcept
    {
        if(fd == INVALID_SOCKET) return {ResultType::Error, 0};

        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(host.c_str());
        addr.sin_port = htons(port);

        int res = ::sendto(fd, reinterpret_cast<const char*>(data), size, 0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if(res == SOCKET_ERROR)
        {
            if(WSAGetLastError() == EAGAIN || WSAGetLastError() == EWOULDBLOCK) return {ResultType::WouldBlock, 0};
            return {ResultType::Error, 0};
        }
        
        return {ResultType::Data, static_cast<size_t>(res)};
    }

    Result UdpSocket::recvFrom(std::string & host, uint16_t & port, void * data, size_t size) noexcept
    {
        if(fd == INVALID_SOCKET) return {ResultType::Error, 0};

        sockaddr_in addr;
        int addrSize = sizeof(addr);

        int res = ::recvfrom(fd, reinterpret_cast<char*>(data), size, 0, reinterpret_cast<sockaddr*>(&addr), &addrSize);
        if(res == SOCKET_ERROR)
        {
            if(WSAGetLastError() == WSAEWOULDBLOCK) return {ResultType::WouldBlock, 0};
            return {ResultType::Error, 0};
        }

        host = inet_ntoa(addr.sin_addr);
        port = ntohs(addr.sin_port);
        return {ResultType::Data, static_cast<size_t>(res)};
    }

    bool UdpSocket::waitRead(int timeoutMs) noexcept
    {
        if(fd == INVALID_SOCKET) return false;

        WSAPOLLFD pfd{};
        pfd.fd = fd;
        pfd.events = POLLIN;

        int ret = WSAPoll(&pfd, 1, timeoutMs);
        if(ret < 0) return false;
        return (ret > 0 && (pfd.revents & POLLIN));
    }

    bool UdpSocket::waitWrite(int timeoutMs) noexcept
    {
        if(fd == INVALID_SOCKET) return false;

        WSAPOLLFD pfd{};
        pfd.fd = fd;
        pfd.events = POLLOUT;

        int ret = WSAPoll(&pfd, 1, timeoutMs);
        if(ret < 0) return false;
        return (ret > 0 && (pfd.revents & POLLOUT));
    }

    void UdpSocket::shutdown()
    {
        if(fd == INVALID_SOCKET) return;

        ::shutdown(fd, SD_BOTH);
        ::closesocket(fd);
        fd = -1;
    }
}