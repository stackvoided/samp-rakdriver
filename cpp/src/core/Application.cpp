#include "../../include/core/Application.h"
#include "../../include/core/Logger.h"
#include "../../include/samp/SAMPPacket.h"
#include <chrono>

Application::Application(BotConfig config) : m_config(std::move(config)) {
    m_client = std::make_unique<RakClient>();
    m_rpcManager = std::make_unique<RPCManager>(m_client.get());
    m_playerPed = std::make_unique<PlayerPed>();
}

Application::~Application() {
    Stop();
}

void Application::Run() {
    m_running = true;
    m_worker = std::thread(&Application::NetworkWorker, this);
}

void Application::Stop() {
    if (!m_running) return;
    m_running = false;
    if (m_worker.joinable()) {
        m_worker.join();
    }
    m_client->Disconnect();
}

void Application::NetworkWorker() {
    LOG_INFO("Connecting to " + m_config.address + ":" + std::to_string(m_config.port));

    if (!m_client->Connect(m_config.address.c_str(), m_config.port)) {
        LOG_ERR("Failed to initialize network interface.");
        return;
    }

    auto lastSync = std::chrono::steady_clock::now();

    while (m_running) {
        Packet* packet = m_client->Receive();
        if (packet) {
            HandlePacket(packet);
            m_client->DeallocatePacket(packet);
        }

        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSync).count() >= 100) {
            m_playerPed->SendOnFootSync(m_client.get());
            lastSync = now;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

void Application::HandlePacket(Packet* packet) {
    if (!packet || packet->length == 0) return;

    uint8_t packetId = packet->data[0];
    BitStream bs(packet->data, packet->length, false);

    switch (packetId) {
        case ID_CONNECTION_REQUEST_ACCEPTED: {
            LOG_INFO("Connection accepted. Joining server...");
            BitStream joinBs = SAMPPacket::BuildClientJoin(m_config, 0x1337);
            m_client->Send(&joinBs, HIGH_PRIORITY, RELIABLE, 0);
            break;
        }
        case ID_RPC: {
            bs.SetReadOffset(8);
            m_rpcManager->HandleRPC(bs);
            break;
        }
        case ID_DISCONNECTION_NOTIFICATION:
            LOG_WARN("Disconnected by server.");
            break;
        case ID_CONNECTION_LOST:
            LOG_ERR("Connection lost.");
            break;
        default:
            break;
    }
}
