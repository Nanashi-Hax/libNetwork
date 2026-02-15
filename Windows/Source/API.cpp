#include <winsock2.h>

namespace Library::Network
{
    void Initialize()
    {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2,2), &wsaData);
    }

    void Shutdown()
    {
        WSACleanup();
    }
}