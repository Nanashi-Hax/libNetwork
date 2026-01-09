// Network.hpp
#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <span>

namespace Network
{
    void Initialize();
    void Shutdown();

    namespace Tcp
    {
        namespace Impl
        {
            class Socket;
            class Acceptor;
            class Connector;
        }

        class Socket;
        class Acceptor;
        class Connector;

        class Socket
        {
            friend Acceptor;
            friend Connector;

        public:
            ~Socket();

            size_t receive(std::span<std::byte> buffer);
            size_t send(std::span<const std::byte> buffer);

            void receiveAll(std::span<std::byte> buffer);
            void sendAll(std::span<const std::byte> buffer);

        private:
            explicit Socket(std::unique_ptr<Impl::Socket> impl);

            std::unique_ptr<Impl::Socket> impl;
        };

        class Acceptor
        {
        public:
            explicit Acceptor(uint16_t port);
            ~Acceptor();

            Socket accept();

        private:
            std::unique_ptr<Impl::Acceptor> impl;
        };

        class Connector
        {
        public:
            Connector(std::string host, uint16_t port);
            ~Connector();

            Socket connect();

        private:
            std::unique_ptr<Impl::Connector> impl;
        };
    }
}