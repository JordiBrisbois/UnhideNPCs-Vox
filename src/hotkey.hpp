#ifndef UNHIDENPCS_HOTKEY_HPP
#define UNHIDENPCS_HOTKEY_HPP

#include <filesystem>
#include <functional>
#include <mutex>
#include <string>
#include <vector>
#include <windows.h>

#include "nlohmann/json.hpp"

struct Hotkey
{
    std::string label {};
    std::string description {};
    uint32_t vkCode {};

    bool ctrl {};
    bool shift {};
    bool alt {};

    [[nodiscard]] std::string toString() const;
    [[nodiscard]] bool matches(uint32_t code, bool control, bool shifted, bool alternate) const
    {
        return vkCode != 0 && vkCode == code && ctrl == control && shift == shifted && alt == alternate;
    }

    [[nodiscard]] bool sameCombination(const Hotkey& other) const
    {
        return other.vkCode != 0 && matches(other.vkCode, other.ctrl, other.shift, other.alt);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Hotkey, vkCode, ctrl, shift, alt)
};

class HotkeyManager
{
private:
    std::vector<std::pair<std::string, Hotkey>> _hotkeys {};
    std::vector<std::function<void(const std::string&)>> _callbacks {};

    std::string _hotkeyCapturing {};
    std::filesystem::path _filePath;
    bool _needSave = false;

    mutable std::recursive_mutex _mutex {};

public:
    explicit HotkeyManager(std::filesystem::path filePath = "");

    uintptr_t onWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void registerHotkey(const std::string& id, const std::string& label, const std::string& description = "");
    void unregisterHotkey(const std::string& id);
    void triggerCallbacks(const std::string& id) const;
    void registerCallback(const std::function<void(const std::string&)>& callback);
    void reset();
    void update();
    void renderHotkey(const std::string& id, Hotkey& hotkey);
    void renderHotkeys();
    void stopCapturing(bool clearHotkey = false);
    [[nodiscard]] bool isCapturing() const;
    [[nodiscard]] Hotkey* getHotkey(const std::string& id);
    [[nodiscard]] std::string label(const std::string& id) const;

    void save();
    void load();
};

[[nodiscard]] std::string shortHotkeyLabel(const std::string& id);

#endif //UNHIDENPCS_HOTKEY_HPP
