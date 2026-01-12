// Network.cpp
#include "Network.hpp"
#include "Tcp/ConnectImpl.hpp"
#include "Tcp/SocketImpl.hpp"
#include "Udp/SocketImpl.hpp"
#include "Impl.hpp"
#include <memory>

namespace Network
{
    void Initialize()
    {
        Impl::Initialize();
    }

    void Shutdown()
    {
        Impl::Shutdown();
    }

    namespace Tcp
    {
        Socket::Socket(std::unique_ptr<Impl::Socket> impl) : impl(std::move(impl)) {}

        Socket::~Socket() {}

        Socket::Socket(Socket&&) noexcept = default;
        Socket& Socket::operator=(Socket&&) noexcept = default;

        void Socket::setNonBlocking(bool enable)
        {
            impl->setNonBlocking(enable);
        }

        void Socket::setNoDelay(bool enable)
        {
            impl->setNoDelay(enable);
        }

        void Socket::setReceiveBufferSize(int size)
        {
            impl->setSendBufferSize(size);
        }

        void Socket::setSendBufferSize(int size)
        {
            impl->setSendBufferSize(size);
        }

        size_t Socket::receive(std::span<std::byte> buffer)
        {
            return impl->receive(buffer);
        }

        size_t Socket::send(std::span<const std::byte> buffer)
        {
            return impl->send(buffer);
        }

        bool Socket::waitReceive(int timeoutMs)
        {
            return impl->waitReceive(timeoutMs);
        }
        
        bool Socket::waitSend(int timeoutMs)
        {
            return impl->waitSend(timeoutMs);
        }

        Acceptor::Acceptor(uint16_t port)
        {
            impl = std::make_unique<Impl::Acceptor>(port);
        }

        Acceptor::~Acceptor() {}

        Socket Acceptor::accept()
        {
            std::unique_ptr<Impl::Socket> socketImpl = std::make_unique<Impl::Socket>(impl->accept());
            return Socket(std::move(socketImpl));
        }

        Connector::Connector(std::string host, uint16_t port)
        {
            impl = std::make_unique<Impl::Connector>(host, port);
        }

        Connector::~Connector() {}

        Socket Connector::connect()
        {
            std::unique_ptr<Impl::Socket> socketImpl = std::make_unique<Impl::Socket>(impl->connect());
            return Socket(std::move(socketImpl));
        }

        void Socket::shutdown()
        {
            impl->shutdown();
        }
    }

    namespace Udp
    {
        Socket::Socket(uint16_t port)
        {
            impl = std::make_unique<Impl::Socket>(port);
        }

        Socket::~Socket() {}
        
        Socket::Socket(Socket&&) noexcept = default;

        Socket& Socket::operator=(Socket&&) noexcept = default;

        void Socket::setNonBlocking(bool enable)
        {
            impl->setNonBlocking(enable);
        }

        void Socket::setBroadcast(bool enable)
        {
            impl->setBroadcast(enable);
        }

        void Socket::setReceiveBufferSize(int size)
        {
            impl->setSendBufferSize(size);
        }

        void Socket::setSendBufferSize(int size)
        {
            impl->setSendBufferSize(size);
        }

        size_t Socket::receiveFrom(std::string& host, uint16_t& port, std::span<std::byte> buffer)
        {
            return impl->receiveFrom(host, port, buffer);
        }

        size_t Socket::sendTo(const std::string host, const uint16_t port, std::span<const std::byte> buffer)
        {
            return impl->sendTo(host, port, buffer);
        }

        bool Socket::waitReceive(int timeoutMs)
        {
            return impl->waitReceive(timeoutMs);
        }
        
        bool Socket::waitSend(int timeoutMs)
        {
            return impl->waitSend(timeoutMs);
        }

        void Socket::shutdown()
        {
            impl->shutdown();
        }
    }
}