#pragma once

#include <cstddef>

namespace Library::Network
{
    enum class ResultType
    {
        Data,
        WouldBlock,
        Disconnected,
        Error
    };

    struct Result
    {
        ResultType type;
        size_t bytes;
    };
}