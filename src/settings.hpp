#ifndef UNHIDENPCS_SETTINGS_HPP
#define UNHIDENPCS_SETTINGS_HPP
#pragma once

#include "profile_settings.hpp"

class Settings : public fw::Settings
{
protected:
    std::unique_ptr<fw::Settings> createChild(const std::string& name) override;

public:
    std::recursive_mutex ProfilesMutex {};

    fw::Setting<bool>& LoadScreenBoost = add<bool>(
        true,
        "LoadScreenBoost",
        "Speed up loading screens by temporarily limiting number of characters to 0 when one is triggered.\n"
        "Note: This will cause characters to start loading after the loading screen is finished,\n"
        "meaning there will be invisible characters for a bit after loading."
    );

    fw::Setting<int32_t>& ActiveProfile = add<int32_t>(0, "ActiveProfile", "The currently active profile index").
        validator(
            [&](auto& value)
            {
                if (value < 0 || static_cast<std::size_t>(value) >= _children.size())
                    value = 0;
            }
        );

    explicit Settings(const std::filesystem::path& filePath);

    [[nodiscard]] SettingsProfile& profile() const;
    [[nodiscard]] std::recursive_mutex& profilesMutex();
    [[nodiscard]] bool profileExists(const std::string& name) const;
    [[nodiscard]] std::string uniqueProfileName(const std::string& baseName) const;

    void addProfile(const std::string& name);
    void duplicateProfile(const std::string& sourceName, const std::string& newName);
    void removeProfile(const std::string& name);
    void renameProfile(const std::string& oldName, const std::string& newName);
};

#endif //UNHIDENPCS_SETTINGS_HPP
