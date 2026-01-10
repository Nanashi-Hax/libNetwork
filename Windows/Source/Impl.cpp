#include <winsock2.h>
#include <system_error>

namespace Network
{
    namespace Impl
    {
        void Initialize()
        {
            WSADATA wsaData;
            int res = WSAStartup(MAKEWORD(2,2), &wsaData);
            if(res != 0)
            {
                throw std::system_error(res, std::system_category(), "WSAStartup()");
            }
        }

        void Shutdown()
        {
            int res = WSACleanup();
            if(res != 0)
            {
                throw std::system_error(res, std::system_category(), "WSACleanup()");
            }
        }
    }
}