// SocketImpl.cpp
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <stdexcept>
#include <system_error>

#include "Tcp/SocketImpl.hpp"

#include <whb/log.h>

namespace Network
{
    namespace Tcp::Impl
    {
        Socket::Socket(int fd) : fd(fd) {}

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

        void Socket::setNoDelay(bool enable)
        {
            if(fd < 0) throw std::logic_error("Socket is dead.");

            int v = enable ? 1 : 0;
            int res = ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v));
            if(res < 0) throw std::system_error(errno, std::generic_category(), "setsockopt(TCP_NODELAY)");
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

        size_t Socket::receive(std::span<std::byte> buffer)
        {
            if(fd < 0) throw std::logic_error("Socket is dead.");

            ssize_t res = ::recv(fd, buffer.data(), buffer.size(), 0);
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

            ssize_t res = ::send(fd, buffer.data(), buffer.size(), 0);
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

        bool Socket::waitReceive(int timeoutMs)
        {
            if(fd < 0) throw std::logic_error("Socket is dead.");

            pollfd pfd{};
            pfd.fd = fd;
            pfd.events = POLLIN;

            int res = poll(&pfd, 1, timeoutMs);
            if(res < 0) throw std::system_error(errno, std::generic_category(), "poll(POLLIN)");
            return (res > 0 && (pfd.revents & POLLIN));
        }

        bool Socket::waitSend(int timeoutMs)
        {
            if(fd < 0) throw std::logic_error("Socket is dead.");

            pollfd pfd{};
            pfd.fd = fd;
            pfd.events = POLLOUT;

            int res = poll(&pfd, 1, timeoutMs);
            if(res < 0) throw std::system_error(errno, std::generic_category(), "poll(POLLOUT)");
            return (res > 0 && (pfd.revents & POLLOUT));
        }

        void Socket::shutdown()
        {
            if(fd < 0) throw std::logic_error("Socket is dead.");

            ::shutdown(fd, SHUT_RDWR);
        }
    }
}