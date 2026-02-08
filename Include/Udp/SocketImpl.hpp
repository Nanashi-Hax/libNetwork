// SocketImpl.hpp
#pragma once

#include <span>
#include <string>
#include <cstdint>

namespace Library::Network
{
    namespace Udp::Impl
    {
        class Socket
        {
        public:
            Socket(uint16_t port);
            ~Socket();

            Socket(Socket const &) = delete;
            Socket& operator=(Socket const &) = delete;

            Socket(Socket && other) noexcept;
            Socket& operator=(Socket&& other) noexcept;

            void setNonBlocking(bool enable);
            void setBroadcast(bool enable);
            void setReceiveBufferSize(int size);
            void setSendBufferSize(int size);

            size_t receiveFrom(std::string& outHost, uint16_t& outPort, std::span<std::byte> buffer);
            size_t sendTo(const std::string host, const uint16_t port, std::span<const std::byte> buffer);

            bool waitReceive(int timeoutMs);
            bool waitSend(int timeoutMs);

            void shutdown();

        private:
            int fd;
        };
    }
}