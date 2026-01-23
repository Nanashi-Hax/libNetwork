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

            Socket(Socket&&) noexcept;
            Socket& operator=(Socket&&) noexcept;

            void setNonBlocking(bool enable);
            void setNoDelay(bool enable);
            void setReceiveBufferSize(int size);
            void setSendBufferSize(int size);

            size_t receive(std::span<std::byte> buffer);
            size_t send(std::span<const std::byte> buffer);

            bool waitReceive(int timeoutMs);
            bool waitSend(int timeoutMs);

            void shutdown();

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
            void shutdown();

        private:
            std::unique_ptr<Impl::Acceptor> impl;
        };

        class Connector
        {
        public:
            Connector(std::string host, uint16_t port);
            ~Connector();

            Socket connect();
            void shutdown();

        private:
            std::unique_ptr<Impl::Connector> impl;
        };
    }

    namespace Udp
    {
        namespace Impl
        {
            class Socket;
        }

        class Socket
        {
        public:
            Socket(uint16_t port = 0);
            ~Socket();

            Socket(Socket&&) noexcept;
            Socket& operator=(Socket&&) noexcept;

            void setNonBlocking(bool enable);
            void setBroadcast(bool enable);
            void setReceiveBufferSize(int size);
            void setSendBufferSize(int size);

            size_t receiveFrom(std::string& host, uint16_t& port, std::span<std::byte> buffer);
            size_t sendTo(const std::string host, const uint16_t port, std::span<const std::byte> buffer);

            bool waitReceive(int timeoutMs);
            bool waitSend(int timeoutMs);

            void shutdown();

        private:
            std::unique_ptr<Impl::Socket> impl;
        };
    }
}