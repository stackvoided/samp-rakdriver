#pragma once

#include "BitStream.h"
#include "NetworkTypes.h"
#include "PacketEnumerations.h"
#include "Socket.h"
#include <memory>

class RakClient {
public:
    RakClient();
    ~RakClient();

    bool Connect(const char* host, uint16_t port, uint16_t localPort = 0);
    void Disconnect();
    bool Send(BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel);
    Packet* Receive();
    void DeallocatePacket(Packet* packet);
    bool IsConnected() const;

private:
    Socket m_socket;
    PlayerID m_serverAddress;
    bool m_isConnected;
};
