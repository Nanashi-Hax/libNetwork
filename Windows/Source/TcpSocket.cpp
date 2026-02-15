#include "Network/TcpSocket.hpp"
#include "Network/Define.hpp"
#include "Network/Result.hpp"

#include <optional>
#include <psdk_inc/_socket_types.h>
#include <stdexcept>
#include <system_error>

#include <winsock2.h>

#include <cstring>

namespace Library::Network
{
    TcpSocket::TcpSocket()
    {
        fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(fd == INVALID_SOCKET) throw std::system_error(WSAGetLastError(), std::system_category(), "socket()");

        int opt = TRUE;
        int res = ::setsockopt(fd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, reinterpret_cast<const char*>(&opt), sizeof(opt));
        if(res == SOCKET_ERROR) throw std::system_error(WSAGetLastError(), std::system_category(), "setsockopt()");
    }

    TcpSocket::~TcpSocket() noexcept
    {
        if(fd == INVALID_SOCKET) return;

        ::closesocket(fd);
    }

    TcpSocket::TcpSocket(TcpSocket&& other) noexcept : fd(other.fd)
    {
        other.fd = INVALID_SOCKET;
    }

    TcpSocket& TcpSocket::operator=(TcpSocket&& other) noexcept
    {
        if (this != &other)
        {
            if (fd != INVALID_SOCKET) ::closesocket(fd);
            fd = other.fd;
            other.fd = INVALID_SOCKET;
        }
        return *this;
    }

    bool TcpSocket::listen(uint16_t port)
    {
        if(fd == INVALID_SOCKET) throw std::logic_error("sockfd is invalid");

        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);

        int res = ::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if(res == SOCKET_ERROR) throw std::system_error(WSAGetLastError(), std::system_category(), "bind()");

        res = ::listen(fd, 64);
        if(res == SOCKET_ERROR) throw std::system_error(WSAGetLastError(), std::system_category(), "listen()");
        
        return true;
    }

    std::optional<TcpSocket> TcpSocket::accept() noexcept
    {
        if(fd == INVALID_SOCKET) return std::nullopt;

        sockaddr_in addr;
        int size = sizeof(addr);
        SocketFD accepted = ::accept(fd, reinterpret_cast<sockaddr*>(&addr), &size);
        if(accepted == INVALID_SOCKET) return std::nullopt;

        u_long mode = 1;
        int res = ::ioctlsocket(accepted, FIONBIO, &mode);
        if (res == SOCKET_ERROR) return std::nullopt;

        int v = 1;
        res = ::setsockopt(accepted, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&v), sizeof(v));
        if(res == SOCKET_ERROR) return std::nullopt;

        return TcpSocket(accepted);
    }

    bool TcpSocket::connect(std::string host, uint16_t port) noexcept
    {
        if(fd == INVALID_SOCKET) return false;

        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(host.c_str());
        addr.sin_port = htons(port);

        int res = ::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if(res == SOCKET_ERROR)
        {
            if(WSAGetLastError() != WSAEWOULDBLOCK) return false;
        }

        u_long mode = 1;
        res = ::ioctlsocket(fd, FIONBIO, &mode);
        if (res == SOCKET_ERROR) return false;

        int v = 1;
        res = ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&v), sizeof(v));
        if(res == SOCKET_ERROR) return false;

        return true;
    }

    Result TcpSocket::send(const void* data, size_t size) noexcept
    {
        if(fd == INVALID_SOCKET) return {ResultType::Error, 0};

        int res = ::send(fd, reinterpret_cast<const char*>(data), size, 0);
        if(res == SOCKET_ERROR)
        {
            if(WSAGetLastError() == WSAEWOULDBLOCK) return {ResultType::WouldBlock, 0};
            return {ResultType::Error, 0};
        }

        if(res == 0) return {ResultType::Disconnected, 0};

        return {ResultType::Data, static_cast<size_t>(res)};
    }

    Result TcpSocket::recv(void* data, size_t size) noexcept
    {
        if(fd == INVALID_SOCKET) return {ResultType::Error, 0};

        int res = ::recv(fd, reinterpret_cast<char*>(data), size, 0);
        if(res == SOCKET_ERROR)
        {
            if(WSAGetLastError() == WSAEWOULDBLOCK) return {ResultType::WouldBlock, 0};
            return {ResultType::Error, 0};
        }

        if(res == 0) return {ResultType::Disconnected, 0};

        return {ResultType::Data, static_cast<size_t>(res)};
    }


    bool TcpSocket::waitRead(int timeoutMs) noexcept
    {
        if(fd == INVALID_SOCKET) return false;

        WSAPOLLFD pfd{};
        pfd.fd = fd;
        pfd.events = POLLIN;

        int res = ::WSAPoll(&pfd, 1, timeoutMs);
        if(res == SOCKET_ERROR) return false;
        return (res > 0 && (pfd.revents & POLLIN));
    }

    bool TcpSocket::waitWrite(int timeoutMs) noexcept
    {
        if(fd == INVALID_SOCKET) return false;

        WSAPOLLFD pfd{};
        pfd.fd = fd;
        pfd.events = POLLOUT;

        int res = ::WSAPoll(&pfd, 1, timeoutMs);
        if(res == SOCKET_ERROR) return false;
        return (res > 0 && (pfd.revents & POLLOUT));
    }


    void TcpSocket::shutdown() noexcept
    {
        if(fd == INVALID_SOCKET) return;
        
        ::shutdown(fd, SD_BOTH);
        ::closesocket(fd);
        fd = INVALID_SOCKET;
    }

    TcpSocket::TcpSocket(SocketFD fd) noexcept : fd(fd) {}
}