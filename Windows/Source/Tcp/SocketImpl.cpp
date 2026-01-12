// SocketImpl.cpp
#include <winsock2.h>
#include <stdexcept>
#include <system_error>

#include "Tcp/SocketImpl.hpp"

namespace Network
{
    namespace Tcp::Impl
    {
        Socket::Socket(SOCKET s) : s(s) {}

        Socket::~Socket()
        {
            if(s != INVALID_SOCKET)
            {
                ::closesocket(s);
            }
        }

        Socket::Socket(Socket && other) noexcept : s(other.s) { other.s = INVALID_SOCKET; }

        Socket& Socket::operator=(Socket&& other) noexcept
        {
            if(this != &other)
            {
                if(s != INVALID_SOCKET) ::closesocket(s);
                s = other.s;
                other.s = INVALID_SOCKET;
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

        void Socket::setNoDelay(bool enable)
        {
            int v = enable ? 1 : 0;
            int res = ::setsockopt(s, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&v), sizeof(v));
            if(res == SOCKET_ERROR) throw std::system_error(WSAGetLastError(), std::system_category(), "setsockopt(TCP_NODELAY)");
        }

        void Socket::setReceiveBufferSize(int size)
        {
            int res = ::setsockopt(s, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&size), sizeof(size));
            if(res < 0) throw std::system_error(WSAGetLastError(), std::system_category(), "setsockopt(SO_RCVBUF)");
        }

        void Socket::setSendBufferSize(int size)
        {
            int res = ::setsockopt(s, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char*>(&size), sizeof(size));
            if(res == SOCKET_ERROR) throw std::system_error(WSAGetLastError(), std::system_category(), "setsockopt(SO_SNDBUF)");
        }

        size_t Socket::receive(std::span<std::byte> buffer)
        {
            if(s == INVALID_SOCKET) throw std::logic_error("Socket is dead.");

            int res = ::recv(s, reinterpret_cast<char*>(buffer.data()), buffer.size(), 0);
            if(res == SOCKET_ERROR)
            {
                throw std::system_error(WSAGetLastError(), std::system_category(), "recv()");
            }
            else if(res == 0)
            {
                return 0;
            }
            else
            {
                return res;
            }
        }

        size_t Socket::send(std::span<const std::byte> buffer)
        {
            if(s == INVALID_SOCKET) throw std::logic_error("Socket is dead.");

            int res = ::send(s, reinterpret_cast<const char*>(buffer.data()), buffer.size(), 0);
            if(res < 0)
            {
                throw std::system_error(WSAGetLastError(), std::system_category(), "send()");
            }
            else if(res == 0)
            {
                return 0;
            }
            else
            {
                return res;
            }
        }

        void Socket::shutdown()
        {
            ::shutdown(s, SD_BOTH);
        }
    }
}