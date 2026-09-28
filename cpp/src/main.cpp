#include "core/Application.h"
#include "core/Config.h"
#include "core/Logger.h"

int main(int argc, char* argv[]) {
    BotConfig config = ConfigLoader::ParseArguments(argc, argv);
    
    LOG_INFO("Initializing SA-MP RakBot Service...");
    Application app(config);
    app.Run();

    LOG_INFO("Press ENTER to terminate application.");
    std::cin.get();

    app.Stop();
    LOG_INFO("Application stopped.");
    return 0;
}
