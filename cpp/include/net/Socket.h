#pragma once

#include "NetworkTypes.h"
#include <cstddef>
#include <cstdint>

class Socket {
public:
    Socket();
    ~Socket();

    bool Open(uint16_t port);
    void Close();
    bool Send(const PlayerID& destination, const uint8_t* data, std::size_t length);
    int Receive(PlayerID& sender, uint8_t* buffer, std::size_t bufferSize);

private:
    int m_handle;
};
