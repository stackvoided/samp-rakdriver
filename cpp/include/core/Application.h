#pragma once

#include "Config.h"
#include "../net/RakClient.h"
#include "../samp/PlayerPed.h"
#include "../samp/RPCManager.h"
#include <atomic>
#include <memory>
#include <thread>

class Application {
public:
    explicit Application(BotConfig config);
    ~Application();

    void Run();
    void Stop();

private:
    void NetworkWorker();
    void HandlePacket(Packet* packet);

    BotConfig m_config;
    std::atomic<bool> m_running{ false };
    std::unique_ptr<RakClient> m_client;
    std::unique_ptr<RPCManager> m_rpcManager;
    std::unique_ptr<PlayerPed> m_playerPed;
    std::thread m_worker;
};
