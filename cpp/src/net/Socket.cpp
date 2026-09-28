#include "../../include/net/Socket.h"

#if defined(_WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif

Socket::Socket() : m_handle(-1) {
#if defined(_WIN32)
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

Socket::~Socket() {
    Close();
#if defined(_WIN32)
    WSACleanup();
#endif
}

bool Socket::Open(uint16_t port) {
    m_handle = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_handle < 0) return false;

#if defined(_WIN32)
    u_long mode = 1;
    ioctlsocket(m_handle, FIONBIO, &mode);
#else
    int flags = fcntl(m_handle, F_GETFL, 0);
    fcntl(m_handle, F_SETFL, flags | O_NONBLOCK);
#endif

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    return bind(m_handle, reinterpret_cast<sockaddr*>(&address), sizeof(address)) >= 0;
}

void Socket::Close() {
    if (m_handle >= 0) {
#if defined(_WIN32)
        closesocket(m_handle);
#else
        close(m_handle);
#endif
        m_handle = -1;
    }
}

bool Socket::Send(const PlayerID& destination, const uint8_t* data, std::size_t length) {
    if (m_handle < 0) return false;

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = destination.binaryAddress;
    address.sin_port = destination.port;

    int sent = sendto(m_handle, reinterpret_cast<const char*>(data), static_cast<int>(length), 0, reinterpret_cast<sockaddr*>(&address), sizeof(address));
    return sent == static_cast<int>(length);
}

int Socket::Receive(PlayerID& sender, uint8_t* buffer, std::size_t bufferSize) {
    if (m_handle < 0) return -1;

    sockaddr_in from{};
#if defined(_WIN32)
    int fromLen = sizeof(from);
#else
    socklen_t fromLen = sizeof(from);
#endif

    int bytesRead = recvfrom(m_handle, reinterpret_cast<char*>(buffer), static_cast<int>(bufferSize), 0, reinterpret_cast<sockaddr*>(&from), &fromLen);
    if (bytesRead <= 0) return -1;

    sender.binaryAddress = from.sin_addr.s_addr;
    sender.port = from.sin_port;
    return bytesRead;
}
