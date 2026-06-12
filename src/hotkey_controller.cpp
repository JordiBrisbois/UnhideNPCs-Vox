#include "hotkey_controller.hpp"

#include "hook.hpp"
#include "hotkey_policy.hpp"
#include "integration/nexus.hpp"
#include "unpc.hpp"

namespace
{
    bool toggleGlobalBoolean(const std::string& id, fw::Settings& settings)
    {
        for (auto& candidate : settings.settings())
        {
            bool matched = false;
            std::visit([&](auto&& setting)
            {
                using SettingType = std::decay_t<decltype(*setting)>;
                if constexpr (std::is_same_v<SettingType, fw::Setting<bool>>)
                {
                    if (id == setting->name())
                    {
                        const bool enabled = setting->set(!setting->get());
                        nexus::alert(fmt::format("{}: {}", unpc::hotkeyManager.label(id), enabled ? "ON" : "OFF"));
                        matched = true;
                    }
                }
            }, candidate);

            if (matched)
            {
                LOG_INFO("Hotkey triggered: {}", id);
                return true;
            }
        }
        return false;
    }

    void alertProfileSetting(const std::string& id, SettingsProfile& profile)
    {
        for (auto& candidate : profile.settings())
        {
            std::visit([&](auto&& setting)
            {
                using SettingType = std::decay_t<decltype(*setting)>;
                if constexpr (std::is_same_v<SettingType, fw::Setting<bool>>)
                {
                    if (setting->name() == id)
                    {
                        nexus::alert(
                            fmt::format("{}: {}", unpc::hotkeyManager.label(id), setting->get() ? "ON" : "OFF")
                        );
                    }
                }
            }, candidate);
        }
    }

    void callback(const std::string& id)
    {
        if (!unpc::settings)
            return;

        if (!hotkey_policy::canTrigger(unpc::mumbleLink && unpc::mumbleLink->getContext().isCompetitiveMode()))
        {
            LOG_INFO("Hotkey ignored in competitive mode: {}", id);
            nexus::alert("UnhideNPCs disabled in competitive mode");
            return;
        }

        std::lock_guard profilesLock(unpc::settings->profilesMutex());

        if (toggleGlobalBoolean(id, *unpc::settings))
            return;

        auto& profile = unpc::settings->profile();
        if (profile.toggleBoolean(id))
        {
            alertProfileSetting(id, profile);
            return;
        }

        if (id == "ForceVisibility")
        {
            ++re::forceVisibility;
            nexus::alert("Visibility refreshed");
            return;
        }

        if (id != "SwitchProfile")
            return;

        auto index = unpc::settings->ActiveProfile.get() + 1;
        if (index >= static_cast<int>(unpc::settings->children().size()))
            index = 0;

        if (index == unpc::settings->ActiveProfile.get())
            return;

        unpc::settings->ActiveProfile.set(index);
        unpc::settings->needSave();
        ++re::forceVisibility;
        nexus::alert(fmt::format("Profile: {}", unpc::settings->profile().name()));
    }

    void registerBooleanSettings(fw::Settings& settings)
    {
        for (auto& candidate : settings.settings())
        {
            std::visit([](auto&& setting)
            {
                using SettingType = std::decay_t<decltype(*setting)>;
                if constexpr (std::is_same_v<SettingType, fw::Setting<bool>>)
                {
                    unpc::hotkeyManager.registerHotkey(
                        setting->name(),
                        shortHotkeyLabel(setting->name()),
                        setting->description()
                    );
                }
            }, candidate);
        }
    }
}

void hotkey_controller::initialize()
{
    unpc::hotkeyManager.reset();
    unpc::hotkeyManager.registerHotkey(
        "ForceVisibility",
        shortHotkeyLabel("ForceVisibility"),
        "Reapply visibility rules"
    );
    unpc::hotkeyManager.registerHotkey(
        "SwitchProfile",
        shortHotkeyLabel("SwitchProfile"),
        "Switch to the next profile"
    );

    registerBooleanSettings(*unpc::settings);
    registerBooleanSettings(unpc::settings->profile());

    unpc::hotkeyManager.registerCallback(callback);
    unpc::hotkeyManager.load();
}

void hotkey_controller::shutdown()
{
    unpc::hotkeyManager.update();
    unpc::hotkeyManager.reset();
}
