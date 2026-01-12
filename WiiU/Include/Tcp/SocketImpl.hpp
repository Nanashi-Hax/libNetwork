// SocketImpl.hpp
#pragma once

#include <span>

namespace Network
{
    namespace Tcp::Impl
    {
        class Socket
        {
            friend class Acceptor;
            friend class Connector;

        public:
            Socket(int fd);
            ~Socket();

            Socket(Socket const &) = delete;
            Socket& operator=(Socket const &) = delete;

            Socket(Socket && other) noexcept;
            Socket& operator=(Socket&& other) noexcept;

            void setNonBlocking(bool enable);
            void setNoDelay(bool enable);
            void setReceiveBufferSize(int size);
            void setSendBufferSize(int size);

            size_t receive(std::span<std::byte> buffer);
            size_t send(std::span<const std::byte> buffer);

            void receiveAll(std::span<std::byte> buffer);
            void sendAll(std::span<const std::byte> buffer);
            
            void shutdown();

        private:
            int fd;
        };
    }
}