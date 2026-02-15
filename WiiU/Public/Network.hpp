#pragma once

#include "Network/Result.hpp"
#include "Network/TcpSocket.hpp"
#include "Network/UdpSocket.hpp"

namespace Library::Network
{
    void Initialize();
    void Shutdown();
}