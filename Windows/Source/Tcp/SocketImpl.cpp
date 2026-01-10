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
            if(s != INVALID_SOCKET) ::closesocket(s);
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

        void Socket::receiveAll(std::span<std::byte> buffer)
        {
            while (!buffer.empty())
            {
                size_t n = receive(buffer);
                if (n == 0) throw std::runtime_error("connection closed while receiving");
                buffer = buffer.subspan(n);
            }
        }

        void Socket::sendAll(std::span<const std::byte> buffer)
        {
            while (!buffer.empty())
            {
                size_t n = send(buffer);
                if (n == 0) throw std::runtime_error("connection closed while sending");
                buffer = buffer.subspan(n);
            }
        }
    }
}