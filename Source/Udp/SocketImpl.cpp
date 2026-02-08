// SocketImpl.cpp
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <stdexcept>
#include <system_error>
#include <cstring>

#include "Udp/SocketImpl.hpp"

namespace Library::Network
{
    namespace Udp::Impl
    {
        Socket::Socket(uint16_t port)
        {
            fd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if(fd < 0) throw std::system_error(errno, std::generic_category(), "socket()");

            int opt = 1;
            int res = ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
            if(res < 0) throw std::system_error(errno, std::generic_category(), "setsockopt(SO_REUSEADDR)");

            sockaddr_in addr;
            std::memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
            addr.sin_port = htons(port);

            res = ::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
            if(res < 0)
            {
                ::close(fd);
                throw std::system_error(errno, std::generic_category(), "bind()");
            }
        }

        Socket::~Socket()
        {
            if(fd >= 0)
            {
                ::close(fd);
            }
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

        void Socket::setNonBlocking(bool enable)
        {
            if(fd < 0) throw std::logic_error("Socket is dead.");

            int flags = ::fcntl(fd, F_GETFL, 0);
            if (flags < 0) throw std::system_error(errno, std::generic_category(), "fcntl(F_GETFL)");

            if (enable)
            {
                flags |= O_NONBLOCK;
            }
            else
            {
                flags &= ~O_NONBLOCK;
            }

            int res = ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);
            if(res < 0) throw std::system_error(errno, std::generic_category(), "fcntl(F_SETFL, O_NONBLOCK)");
        }

        void Socket::setBroadcast(bool enable)
        {
            if(fd < 0) throw std::logic_error("Socket is dead.");

            int v = enable ? 1 : 0;
            int res = ::setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &v, sizeof(v));
            if(res < 0) throw std::system_error(errno, std::generic_category(), "setsockopt(SO_BROADCAST)");
        }

        void Socket::setReceiveBufferSize(int size)
        {
            if(fd < 0) throw std::logic_error("Socket is dead.");

            int res = ::setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
            if(res < 0) throw std::system_error(errno, std::generic_category(), "setsockopt(SO_RCVBUF)");
        }

        void Socket::setSendBufferSize(int size)
        {
            if(fd < 0) throw std::logic_error("Socket is dead.");

            int res = ::setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
            if(res < 0) throw std::system_error(errno, std::generic_category(), "setsockopt(SO_SNDBUF)");
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

        bool Socket::waitReceive(int timeoutMs)
        {
            if(fd < 0) throw std::logic_error("Socket is dead.");

            pollfd pfd{};
            pfd.fd = fd;
            pfd.events = POLLIN;

            int ret = poll(&pfd, 1, timeoutMs);
            if(ret < 0) throw std::system_error(errno, std::generic_category(), "poll(POLLIN)");
            return (ret > 0 && (pfd.revents & POLLIN));
        }

        bool Socket::waitSend(int timeoutMs)
        {
            if(fd < 0) throw std::logic_error("Socket is dead.");

            pollfd pfd{};
            pfd.fd = fd;
            pfd.events = POLLOUT;

            int ret = poll(&pfd, 1, timeoutMs);
            if(ret < 0) throw std::system_error(errno, std::generic_category(), "poll(POLLOUT)");
            return (ret > 0 && (pfd.revents & POLLOUT));
        }

        void Socket::shutdown()
        {
            if(fd < 0) throw std::logic_error("Socket is dead.");

            ::shutdown(fd, SHUT_RDWR);
        }
    }
}