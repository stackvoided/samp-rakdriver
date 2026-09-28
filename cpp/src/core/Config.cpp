#include "../../include/core/Config.h"

BotConfig ConfigLoader::ParseArguments(int argc, char* argv[]) {
    BotConfig config;
    if (argc >= 2) config.address = argv[1];
    if (argc >= 3) config.port = static_cast<uint16_t>(std::stoi(argv[2]));
    if (argc >= 4) config.nickname = argv[3];
    return config;
}
