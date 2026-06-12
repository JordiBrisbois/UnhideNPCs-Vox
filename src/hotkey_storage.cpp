#include "hotkey.hpp"

#include "fw/logger.hpp"

void HotkeyManager::save()
{
    std::lock_guard lock(_mutex);
    const std::filesystem::path dir = _filePath.parent_path();
    if (!exists(dir))
        create_directories(dir);

    std::ofstream file(_filePath);
    if (!file.is_open())
    {
        LOG_ERR("Failed to open {} for writing", _filePath.string());
        return;
    }

    const nlohmann::json json = _hotkeys;
    file << std::setw(4) << json << '\n';
    _needSave = false;
}

void HotkeyManager::load()
{
    std::lock_guard lock(_mutex);
    if (!exists(_filePath))
        return;

    std::ifstream file(_filePath);
    if (!file.is_open())
    {
        LOG_ERR("Failed to open {} for reading", _filePath.string());
        return;
    }

    nlohmann::json json;
    file >> json;

    const std::vector<std::pair<std::string, Hotkey>> hotkeys = json;
    std::vector<std::tuple<uint32_t, bool, bool, bool>> loadedCombinations;
    for (const auto& [id, hotkey] : hotkeys)
    {
        auto* registered = getHotkey(id);
        if (!registered)
        {
            LOG_WARN("Unknown hotkey \"{}\"", id);
            continue;
        }

        const auto combination = std::make_tuple(hotkey.vkCode, hotkey.ctrl, hotkey.shift, hotkey.alt);
        if (hotkey.vkCode != 0 && std::find(loadedCombinations.begin(), loadedCombinations.end(), combination)
            != loadedCombinations.end())
        {
            LOG_WARN("Duplicate hotkey combination ignored for \"{}\": {}", id, hotkey.toString());
            continue;
        }

        registered->vkCode = hotkey.vkCode;
        registered->ctrl   = hotkey.ctrl;
        registered->shift  = hotkey.shift;
        registered->alt    = hotkey.alt;
        if (hotkey.vkCode != 0)
            loadedCombinations.push_back(combination);
        LOG_INFO("Loaded hotkey \"{}\": {}", id, registered->toString());
    }
}
