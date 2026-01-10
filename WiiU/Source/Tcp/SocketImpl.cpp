// SocketImpl.cpp
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include <system_error>

#include "Tcp/SocketImpl.hpp"

namespace Network
{
    namespace Tcp::Impl
    {
        Socket::Socket(int fd) : fd(fd) {}

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

        void Socket::setNoDelay(bool enable)
        {
            int v = enable ? 1 : 0;
            int res = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v));
            if(res < 0) throw std::system_error(errno, std::generic_category(), "setsockopt(TCP_NODELAY)");
        }

        size_t Socket::receive(std::span<std::byte> buffer)
        {
            if(fd < 0) throw std::logic_error("Socket is dead.");

            int res = ::recv(fd, buffer.data(), buffer.size(), 0);
            if(res < 0)
            {
                throw std::system_error(errno, std::generic_category(), "recv()");
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
            if(fd < 0) throw std::logic_error("Socket is dead.");

            int res = ::send(fd, buffer.data(), buffer.size(), 0);
            if(res < 0)
            {
                throw std::system_error(errno, std::generic_category(), "send()");
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