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
            if(s < 0) throw std::system_error(errno, std::generic_category(), "socket()");

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

        void Socket::setBroadcast(bool enable)
        {
            int v = enable ? 1 : 0;
            int res = ::setsockopt(s, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(&v), sizeof(v));
            if(res == SOCKET_ERROR) throw std::system_error(WSAGetLastError(), std::system_category(), "setsockopt(SO_BROADCAST)");
        }

        void Socket::setSendBufferSize(int size)
        {
            int res = setsockopt(s, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char*>(&size), sizeof(size));
            if(res < 0) throw std::system_error(errno, std::generic_category(), "setsockopt(SO_SNDBUF)");
        }

        size_t Socket::receiveFrom(std::string& outHost, uint16_t& outPort, std::span<std::byte> buffer)
        {
            if(s < 0) throw std::logic_error("Socket is dead.");

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
            if(s < 0) throw std::logic_error("Socket is dead.");

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

        void Socket::shutdown()
        {
            ::shutdown(s, SD_BOTH);
        }
    }
}