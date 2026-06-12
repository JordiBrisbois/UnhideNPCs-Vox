#include "profile_settings.hpp"

std::unique_ptr<fw::Settings> SettingsProfile::createChild(const std::string& name)
{
    return std::make_unique<SettingsProfile>(this, name);
}

void SettingsProfile::copyFrom(SettingsProfile& source)
{
    for (auto& destinationSetting : settings())
    {
        std::visit([&](auto&& destination)
        {
            for (auto& sourceSetting : source.settings())
            {
                std::visit([&](auto&& sourceValue)
                {
                    using DestinationType = std::decay_t<decltype(*destination)>;
                    using SourceType      = std::decay_t<decltype(*sourceValue)>;
                    if constexpr (std::is_same_v<DestinationType, SourceType>)
                    {
                        if (destination->name() == sourceValue->name())
                            destination->set(sourceValue->get());
                    }
                }, sourceSetting);
            }
        }, destinationSetting);
    }
}

bool SettingsProfile::toggleBoolean(const std::string& name)
{
    for (auto& setting : settings())
    {
        bool matched = false;
        std::visit([&](auto&& value)
        {
            using SettingType = std::decay_t<decltype(*value)>;
            if constexpr (std::is_same_v<SettingType, fw::Setting<bool>>)
            {
                if (value->name() == name)
                {
                    const bool enabled = !value->get();
                    value->set(enabled);
                    matched = true;

                    if (enabled && value.get() == &UnhidePlayers)
                        HidePlayers.set(false);
                    else if (enabled && value.get() == &HidePlayers)
                        UnhidePlayers.set(false);
                    else if (enabled && value.get() == &PlayerOwned)
                        HidePlayerOwned.set(false);
                    else if (enabled && value.get() == &HidePlayerOwned)
                        PlayerOwned.set(false);
                }
            }
        }, setting);

        if (matched)
            return true;
    }
    return false;
}

void SettingsProfile::resolveConflicts()
{
    if (HidePlayers.get() && UnhidePlayers.get())
        UnhidePlayers.set(false);

    if (HidePlayerOwned.get() && PlayerOwned.get())
        PlayerOwned.set(false);
}
