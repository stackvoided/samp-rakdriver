#pragma once

#include <cstdint>
#include <string>

struct BotConfig {
    std::string address{ "127.0.0.1" };
    uint16_t port{ 7777 };
    std::string nickname{ "RakBot_Client" };
    std::string clientVersion{ "0.3.7-R1" };
    uint32_t netGameVersion{ 4057 };
    std::string authKey{ "61502447432431" };
};

class ConfigLoader {
public:
    static BotConfig ParseArguments(int argc, char* argv[]);
};
