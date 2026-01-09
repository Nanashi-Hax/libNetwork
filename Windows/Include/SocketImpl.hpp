// SocketImpl.hpp
#pragma once

#include <span>
#include <winsock2.h>

namespace Network
{
    namespace Impl
    {
        void Initialize();
        void Shutdown();
    }

    namespace Tcp::Impl
    {
        class Socket
        {
            friend class Acceptor;
            friend class Connector;

        public:
            Socket(SOCKET s);
            ~Socket();

            Socket(Socket const &) = delete;
            Socket& operator=(Socket const &) = delete;

            Socket(Socket && other) noexcept;
            Socket& operator=(Socket&& other) noexcept;

            size_t receive(std::span<std::byte> buffer);
            size_t send(std::span<const std::byte> buffer);

            void receiveAll(std::span<std::byte> buffer);
            void sendAll(std::span<const std::byte> buffer);

        private:
            SOCKET s;
        };
    }
}