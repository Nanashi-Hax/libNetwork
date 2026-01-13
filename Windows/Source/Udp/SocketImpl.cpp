// SocketImpl.cpp
#include <winsock2.h>
#include <stdexcept>
#include <system_error>
#include <cstring>

#include "Udp/SocketImpl.hpp"

namespace Network
{
    namespace Udp::Impl
    {
        Socket::Socket(uint16_t port)
        {
            s = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if(s == INVALID_SOCKET) throw std::system_error(errno, std::generic_category(), "socket()");

            sockaddr_in addr;
            std::memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
            addr.sin_port = htons(port);

            int res = ::bind(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
            if(res < 0)
            {
                ::closesocket(s);
                throw std::system_error(errno, std::generic_category(), "bind()");
            }

            int opt = 1;
            res = ::setsockopt(s, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
            if(res == SOCKET_ERROR) throw std::system_error(WSAGetLastError(), std::system_category(), "setsockopt(SO_REUSEADDR)");
        }

        Socket::~Socket()
        {
            if(s >= 0)
            {
                ::closesocket(s);
            }
        }

        Socket::Socket(Socket && other) noexcept : s(other.s) { other.s = -1; }

        Socket& Socket::operator=(Socket&& other) noexcept
        {
            if(this != &other)
            {
                if(s >= 0) ::closesocket(s);
                s = other.s;
                other.s = -1;
            }
            return *this;
        }

        void Socket::setNonBlocking(bool enable)
        {
            if(s == INVALID_SOCKET) throw std::logic_error("Socket is dead.");

            u_long mode = enable ? 1 : 0;
            int res = ::ioctlsocket(s, FIONBIO, &mode);
            if(res == SOCKET_ERROR) throw std::system_error(WSAGetLastError(), std::system_category(), "ioctlsocket(FIONBIO)");
        }

        void Socket::setBroadcast(bool enable)
        {
            int v = enable ? 1 : 0;
            int res = ::setsockopt(s, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(&v), sizeof(v));
            if(res == SOCKET_ERROR) throw std::system_error(WSAGetLastError(), std::system_category(), "setsockopt(SO_BROADCAST)");
        }

        void Socket::setReceiveBufferSize(int size)
        {
            int res = ::setsockopt(s, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&size), sizeof(size));
            if(res < 0) throw std::system_error(errno, std::generic_category(), "setsockopt(SO_RCVBUF)");
        }

        void Socket::setSendBufferSize(int size)
        {
            int res = setsockopt(s, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char*>(&size), sizeof(size));
            if(res == SOCKET_ERROR) throw std::system_error(WSAGetLastError(), std::system_category(), "setsockopt(SO_SNDBUF)");
        }

        size_t Socket::receiveFrom(std::string& outHost, uint16_t& outPort, std::span<std::byte> buffer)
        {
            if(s == INVALID_SOCKET) throw std::logic_error("Socket is dead.");

            sockaddr_in addr;
            int size = sizeof(addr);

            int res = ::recvfrom(s, reinterpret_cast<char*>(buffer.data()), buffer.size(), 0, reinterpret_cast<sockaddr*>(&addr), &size);
            if(res < 0)
            {
                throw std::system_error(errno, std::generic_category(), "recvfrom()");
            }
            else
            {
                outHost = inet_ntoa(addr.sin_addr);
                outPort = ntohs(addr.sin_port);
                return res;
            }
        }

        size_t Socket::sendTo(const std::string host, const uint16_t port, std::span<const std::byte> buffer)
        {
            if(s == INVALID_SOCKET) throw std::logic_error("Socket is dead.");

            sockaddr_in addr;
            std::memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = inet_addr(host.c_str());
            addr.sin_port = htons(port);

            int res = ::sendto(s, reinterpret_cast<const char*>(buffer.data()), buffer.size(), 0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
            if(res < 0)
            {
                throw std::system_error(errno, std::generic_category(), "sendto()");
            }
            else
            {
                return res;
            }
        }

        bool Socket::waitReceive(int timeoutMs)
        {
            WSAPOLLFD pfd{};
            pfd.fd = s;
            pfd.events = POLLIN;

            int res = WSAPoll(&pfd, 1, timeoutMs);
            if(res < 0) throw std::system_error(WSAGetLastError(), std::system_category(), "WSAPoll(POLLIN)");
            return (res > 0 && (pfd.revents & POLLIN));
        }

        bool Socket::waitSend(int timeoutMs)
        {
            WSAPOLLFD pfd{};
            pfd.fd = s;
            pfd.events = POLLOUT;

            int res = WSAPoll(&pfd, 1, timeoutMs);
            if(res < 0) throw std::system_error(WSAGetLastError(), std::system_category(), "WSAPoll(POLLOUT)");
            return (res > 0 && (pfd.revents & POLLOUT));
        }

        void Socket::shutdown()
        {
            ::shutdown(s, SD_BOTH);
        }
    }
}