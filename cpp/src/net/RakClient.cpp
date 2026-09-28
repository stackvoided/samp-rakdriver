#include "../../include/net/RakClient.h"
#if defined(_WIN32)
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
#endif

RakClient::RakClient() : m_isConnected(false) {}

RakClient::~RakClient() {
    Disconnect();
}

bool RakClient::Connect(const char* host, uint16_t port, uint16_t localPort) {
    if (!m_socket.Open(localPort)) return false;

    inet_pton(AF_INET, host, &m_serverAddress.binaryAddress);
    m_serverAddress.port = htons(port);

    BitStream bs;
    bs.Write<uint8_t>(ID_CONNECTION_REQUEST);
    bs.Write<uint32_t>(4057);
    bs.Write<uint8_t>(0);

    m_isConnected = Send(&bs, HIGH_PRIORITY, RELIABLE, 0);
    return m_isConnected;
}

void RakClient::Disconnect() {
    m_socket.Close();
    m_isConnected = false;
}

bool RakClient::Send(BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel) {
    if (!bitStream) return false;
    return m_socket.Send(m_serverAddress, bitStream->GetData(), bitStream->GetNumberOfBytesUsed());
}

Packet* RakClient::Receive() {
    uint8_t buffer[2048];
    PlayerID sender;
    int bytesRead = m_socket.Receive(sender, buffer, sizeof(buffer));
    if (bytesRead <= 0) return nullptr;

    Packet* packet = new Packet();
    packet->playerID = sender;
    packet->length = static_cast<uint32_t>(bytesRead);
    packet->bitSize = bytesRead * 8;
    packet->data = new uint8_t[bytesRead];
    std::memcpy(packet->data, buffer, bytesRead);

    return packet;
}

void RakClient::DeallocatePacket(Packet* packet) {
    if (!packet) return;
    delete[] packet->data;
    delete packet;
}

bool RakClient::IsConnected() const { return m_isConnected; }
