// SocketImpl.hpp
#pragma once

#include <span>
#include <string>
#include <cstdint>

namespace Network
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

            void setBroadcast(bool enable);
            void setSendBufferSize(int size);

            size_t receiveFrom(std::string& outHost, uint16_t& outPort, std::span<std::byte> buffer);
            size_t sendTo(const std::string host, const uint16_t port, std::span<const std::byte> buffer);

            void shutdown();

        private:
            int fd;
        };
    }
}