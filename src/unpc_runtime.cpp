#include "unpc.hpp"

#include "hook.hpp"
#include "hotkey.hpp"
#include "hotkey_controller.hpp"
#include "integration/nexus.hpp"
#include "re.hpp"

using namespace std::chrono_literals;
using namespace memory;

using namespace unpc;

std::atomic_bool               unpc::exit {};
std::atomic_bool               unpc::stopping {};
std::optional<logging::Logger> unpc::logger;
std::optional<Settings>        unpc::settings;
std::optional<Detour>          unpc::npcHook {};

HotkeyManager unpc::hotkeyManager(std::filesystem::current_path() / "addons" / "UnhideNPCs" / "hotkeys.json");
HANDLE        unpc::hMutex {};

MumbleLink* unpc::mumbleLink {};
int32_t*    unpc::loadingScreenActive {};

std::atomic_uint32_t unpc::numPlayersVisible {};
std::atomic_uint32_t unpc::numPlayerOwnedVisible {};
std::atomic_uint32_t unpc::numNpcsVisible {};
std::atomic_uint32_t unpc::numPlayersInArea {};

bool checkInitFile()
{
    const auto rootDir = std::filesystem::current_path() / "addons" / "UnhideNPCs";
    if (!std::filesystem::exists(rootDir))
    {
        if (!std::filesystem::create_directories(rootDir))
        {
            LOG_ERR("Failed to create root directory: {}", rootDir.string());
            return false;
        }
    }

    const auto initFile = rootDir / "init.txt";
    if (std::filesystem::exists(initFile))
    {
        // if the file already exists, it means we must have crashed in a previous initialization
        // compare the contents of the file with the current version string

        std::ifstream file(initFile);
        std::string   version;
        file >> version;
        file.close();

        // if it is the same as the current version, then abort
        if (version == unpc::version::STRING)
        {
            LOG_ERR("Initialization aborted, previous init of same version crashed");
            return false;
        }

        // if it's different from the current version, then remove it
        std::filesystem::remove(initFile);
    }

    // write the current version to the init file
    std::ofstream file(initFile);
    file << unpc::version::STRING;
    file.close();

    return true;
}

void removeInitFile()
{
    std::error_code ec{};
    std::filesystem::remove(std::filesystem::current_path() / "addons" / "UnhideNPCs" / "init.txt", ec);
}

bool initialize()
{
    LOG_INFO("Initializing");

    if (!checkInitFile())
    {
        return false;
    }

    if (!mumbleLink)
    {
        mumbleLink = nexus::getMumbleLink();
        if (!mumbleLink)
        {
            LOG_ERR("MumbleLink Failed");
            removeInitFile();
            return false;
        }
    }
    LOG_INFO("MumbleLink OK");

    settings.emplace(std::filesystem::current_path() / "addons" / "UnhideNPCs" / "settings.json");
    if (!settings->loaded())
    {
        LOG_ERR("Settings Failed");
        removeInitFile();
        return false;
    }
    LOG_INFO("Settings OK");

    Module game {};
    if (!Module::tryGetByName("Gw2-64.exe", game))
    {
        LOG_ERR("Unable to get Gw2-64.exe module");
        removeInitFile();
        return false;
    }
    LOG_INFO("Gw2-64.exe OK");

    Handle pointer {};
    if (!game.findPattern(re::pattern1, pointer))
    {
        LOG_ERR("Compatibility check 1 failed");
        removeInitFile();
        return false;
    }
    re::vtableIndex = pointer.add(2).deref<uint32_t>() / 8;
    LOG_INFO("Compatibility check 1 OK");

    if (!game.findPattern(re::pattern2, pointer))
    {
        LOG_ERR("Compatibility check 2 failed");
        removeInitFile();
        return false;
    }
    pointer = pointer.add(10).resolve_relative_call();
    npcHook.emplace("Hook", pointer.to_ptr<void*>(), reinterpret_cast<void*>(re::hook));
    LOG_INFO("Compatibility check 2 OK");

    if (!game.findPattern(re::pattern3, pointer))
    {
        LOG_ERR("Compatibility check 3 failed");
        removeInitFile();
        return false;
    }
    pointer             = pointer.add(5);
    loadingScreenActive = pointer.add(6).add(pointer.add(2).deref<int32_t>()).to_ptr<int32_t*>();
    LOG_INFO("Compatibility check 3 OK");

    if (!game.findPattern(re::pattern4, pointer))
    {
        LOG_ERR("Compatibility check 4 failed");
        removeInitFile();
        return false;
    }
    re::gw2::getContextCollection = reinterpret_cast<re::gw2::GetContextCollectionFn>(pointer.raw());
    LOG_INFO("Compatibility check 4 OK");

    if (!game.findPattern(re::pattern5, pointer))
    {
        LOG_ERR("Compatibility check 5 failed");
        removeInitFile();
        return false;
    }
    re::gw2::getAvContext = reinterpret_cast<re::gw2::GetAvContextFn>(pointer.add(8).resolve_relative_call().raw());
    LOG_INFO("Compatibility check 5 OK");

    if (!npcHook->enable())
    {
        LOG_ERR("Failed to enable hook");
        removeInitFile();
        return false;
    }
    LOG_INFO("Hook OK");

    hotkey_controller::initialize();

    return true;
}

void unpc::onHookTick()
{
    if (exit)
    {
        return;
    }

    if (!settings || !settings->loaded())
    {
        return;
    }

    hotkeyManager.update();
    std::lock_guard profilesLock(settings->profilesMutex());
    settings->save();

    if (TRIGGER_ONCE)
    {
        // remove the init.txt at this point as all initialization is completed
        removeInitFile();
    }

}

void unpc::start()
{
    if (settings && logger && npcHook)
    {
        return;
    }

    exit.store(false, std::memory_order_release);
    stopping.store(false, std::memory_order_release);

#ifdef BUILDING_ON_GITHUB
    auto logLevel = logging::LogLevel::Info;
#else
    auto logLevel = logging::LogLevel::Debug;
#endif

    logger.emplace("UnhideNPCs", std::filesystem::current_path() / "addons" / "UnhideNPCs" / "log.txt", logLevel);

    LOG_INFO("Starting");
    LOG_INFO("Version {}", version::STRING);
    LOG_INFO("Built on {} at {}", __DATE__, __TIME__);

    LOG_INFO("Mode: Nexus");
    logger->registerCallback(nexus::logCallback);

    if (!initialize())
    {
        LOG_ERR("Initialization failed");
        exit = true;
        return;
    }

    LOG_INFO("Initialization complete");
}

void unpc::stop()
{
    LOG_INFO("Stopping");

    stopping.store(true, std::memory_order_release);

    if (npcHook)
    {
        npcHook->disable(true);

        while (re::hookInFlight.load(std::memory_order_acquire) != 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        re::restoreGameState();
    }

    if (logger)
    {
        logger->unregisterCallback(nexus::logCallback);
    }

    hotkey_controller::shutdown();
    settings.reset();
    logger.reset();

    mumbleLink = nullptr;
    util::closeHandle(hMutex);
}
