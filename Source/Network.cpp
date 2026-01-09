// Network.cpp
#include "Network.hpp"
#include "ConnectImpl.hpp"
#include "SocketImpl.hpp"
#include <memory>

namespace Network::Tcp
{
    Socket::Socket(std::unique_ptr<Impl::Socket> impl) : impl(std::move(impl)) {}

    Socket::~Socket() {}

    size_t Socket::receive(std::span<std::byte> buffer)
    {
        return impl->receive(buffer);
    }

    size_t Socket::send(std::span<const std::byte> buffer)
    {
        return impl->send(buffer);
    }

    void Socket::receiveAll(std::span<std::byte> buffer)
    {
        impl->receiveAll(buffer);
    }

    void Socket::sendAll(std::span<const std::byte> buffer)
    {
        impl->sendAll(buffer);
    }

    Acceptor::Acceptor(uint16_t port)
    {
        impl = std::make_unique<Impl::Acceptor>(Impl::Acceptor(port));
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
}