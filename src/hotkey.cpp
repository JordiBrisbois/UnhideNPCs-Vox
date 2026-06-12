#include "hotkey.hpp"

#include "imgui.h"
#include "unpc.hpp"
#include "fw/logger.hpp"

std::string Hotkey::toString() const
{
    if (vkCode == 0)
    {
        return "None";
    }

    std::string s;

    if (ctrl)
    {
        s += "Ctrl + ";
    }
    if (shift)
    {
        s += "Shift + ";
    }
    if (alt)
    {
        s += "Alt + ";
    }

    char name[64];
    const auto  scan = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);
    GetKeyNameTextA(scan << 16, name, 64);

    s += name;

    return s;
}

HotkeyManager::HotkeyManager(std::filesystem::path filePath)
{
    _filePath = std::move(filePath);
}

uintptr_t HotkeyManager::onWndProc(HWND hWnd, const UINT msg, const WPARAM wParam, LPARAM lParam)
{
    std::lock_guard lock(_mutex);

    if (msg != WM_KEYDOWN)
    {
        return msg;
    }

    const bool justPressed = (lParam & (1 << 30)) == 0;
    if (!justPressed)
    {
        return msg;
    }

    if (unpc::mumbleLink && unpc::mumbleLink->getContext().isTextboxFocused())
    {
        return msg;
    }

    if (ImGui::GetCurrentContext())
    {
        const auto& io = ImGui::GetIO();
        if (io.WantCaptureKeyboard || io.WantTextInput)
        {
            return msg;
        }
    }

    if (wParam == VK_ESCAPE && isCapturing())
    {
        stopCapturing(true);
        return 0;
    }

    const auto vkCode = wParam;

    switch (vkCode)
    {
    case 0:
    case VK_LBUTTON:
    case VK_RBUTTON:
    case VK_MBUTTON:
    case VK_XBUTTON1:
    case VK_XBUTTON2:
    case VK_BACK:
    case VK_TAB:
    case VK_RETURN:
    case VK_PAUSE:
    case VK_CAPITAL:
    case VK_NONAME:
    case VK_CONTROL:
    case VK_LCONTROL:
    case VK_RCONTROL:
    case VK_SHIFT:
    case VK_LSHIFT:
    case VK_RSHIFT:
    case VK_MENU:
    case VK_LMENU:
    case VK_RMENU: return msg;
    default: break;
    }

    const bool ctrl  = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    const bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    const bool alt   = (GetKeyState(VK_MENU) & 0x8000) != 0;

    if (!_hotkeyCapturing.empty())
    {
        const auto hotkey = getHotkey(_hotkeyCapturing);
        if (!hotkey)
        {
            _hotkeyCapturing = "";
            return 0;
        }

        hotkey->vkCode   = vkCode;
        hotkey->ctrl     = ctrl;
        hotkey->shift    = shift;
        hotkey->alt      = alt;
        for (auto& [otherId, other] : _hotkeys)
        {
            if (otherId != _hotkeyCapturing && other.matches(vkCode, ctrl, shift, alt))
            {
                other.vkCode = 0;
                LOG_INFO("Cleared duplicate hotkey \"{}\"", otherId);
            }
        }

        LOG_INFO("Updated hotkey \"{}\" to {}", _hotkeyCapturing, hotkey->toString());

        _hotkeyCapturing = "";
        _needSave        = true;

        return 0;
    }

    for (auto& [id, hotkey] : _hotkeys)
    {
        if (!hotkey.matches(vkCode, ctrl, shift, alt))
            continue;

        triggerCallbacks(id);
    }

    return msg;
}

void HotkeyManager::registerHotkey(const std::string& id, const std::string& label, const std::string& description)
{
    std::lock_guard lock(_mutex);
    if (getHotkey(id))
    {
        LOG_WARN("Hotkey \"{}\" already registered", id);
        return;
    }

    _hotkeys.emplace_back(id, Hotkey { .label = label, .description = description });
}

void HotkeyManager::unregisterHotkey(const std::string& id)
{
    std::lock_guard lock(_mutex);
    _hotkeys.erase(
        std::remove_if(
            _hotkeys.begin(),
            _hotkeys.end(),
            [&](const auto& pair)
            {
                return pair.first == id;
            }
        ),
        _hotkeys.end()
    );
}

void HotkeyManager::triggerCallbacks(const std::string& id) const
{
    std::lock_guard lock(_mutex);
    for (const auto& callback : _callbacks)
    {
        callback(id);
    }
}

void HotkeyManager::registerCallback(const std::function<void(const std::string&)>& callback)
{
    std::lock_guard lock(_mutex);
    _callbacks.push_back(callback);
}

void HotkeyManager::reset()
{
    std::lock_guard lock(_mutex);
    _hotkeys.clear();
    _callbacks.clear();
    _hotkeyCapturing.clear();
}

void HotkeyManager::update()
{
    std::lock_guard lock(_mutex);
    if (_needSave)
    {
        save();
    }
}

void HotkeyManager::stopCapturing(const bool clearHotkey)
{
    std::lock_guard lock(_mutex);
    if (!_hotkeyCapturing.empty())
    {
        if (clearHotkey)
        {
            const auto hotkey = getHotkey(_hotkeyCapturing);
            if (hotkey)
            {
                hotkey->vkCode = 0;
                _needSave      = true;
            }
        }

        _hotkeyCapturing = "";
    }
}

bool HotkeyManager::isCapturing() const
{
    std::lock_guard lock(_mutex);
    return !_hotkeyCapturing.empty();
}

Hotkey* HotkeyManager::getHotkey(const std::string& id)
{
    std::lock_guard lock(_mutex);
    for (auto& [key, hotkey] : _hotkeys)
    {
        if (key == id)
        {
            return &hotkey;
        }
    }

    return nullptr;
}

std::string HotkeyManager::label(const std::string& id) const
{
    std::lock_guard lock(_mutex);
    for (const auto& [key, hotkey] : _hotkeys)
    {
        if (key == id)
            return hotkey.label;
    }
    return id;
}
