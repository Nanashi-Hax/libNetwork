// SocketImpl.cpp
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
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
            fd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if(fd < 0) throw std::system_error(errno, std::generic_category(), "socket()");

            sockaddr_in addr;
            std::memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
            addr.sin_port = htons(port);

            int res = ::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
            if(res < 0)
            {
                ::close(fd);
                throw std::system_error(errno, std::generic_category(), "bind()");
            }
        }

        Socket::~Socket()
        {
            if(fd >= 0) ::close(fd);
        }

        Socket::Socket(Socket && other) noexcept : fd(other.fd) { other.fd = -1; }

        Socket& Socket::operator=(Socket&& other) noexcept
        {
            if(this != &other)
            {
                if(fd >= 0) ::close(fd);
                fd = other.fd;
                other.fd = -1;
            }
            return *this;
        }

        void Socket::setBroadcast(bool enable)
        {
            int v = enable ? 1 : 0;
            int res = setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &v, sizeof(v));
            if(res < 0) throw std::system_error(errno, std::generic_category(), "setsockopt(SO_BROADCAST)");
        }

        size_t Socket::receiveFrom(std::string& outHost, uint16_t& outPort, std::span<std::byte> buffer)
        {
            if(fd < 0) throw std::logic_error("Socket is dead.");

            sockaddr_in addr;
            socklen_t size = sizeof(addr);

            int res = ::recvfrom(fd, buffer.data(), buffer.size(), 0, reinterpret_cast<sockaddr*>(&addr), &size);
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
            if(fd < 0) throw std::logic_error("Socket is dead.");

            sockaddr_in addr;
            std::memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = inet_addr(host.c_str());
            addr.sin_port = htons(port);

            int res = ::sendto(fd, buffer.data(), buffer.size(), 0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
            if(res < 0)
            {
                throw std::system_error(errno, std::generic_category(), "sendto()");
            }
            else
            {
                return res;
            }
        }
    }
}