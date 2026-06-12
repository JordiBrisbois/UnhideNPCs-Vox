#include "profile_presets.hpp"

namespace
{
    void reset(SettingsProfile& profile)
    {
        for (auto& setting : profile.settings())
        {
            std::visit([](auto&& value)
            {
                value->set(value->defaultValue());
            }, setting);
        }
    }
}

void profile_presets::apply(SettingsProfile& profile, const Preset preset)
{
    reset(profile);

    switch (preset)
    {
    case Preset::Default:
        break;
    case Preset::ShowEverything:
        profile.UnhidePlayers.set(true);
        profile.PlayerOwned.set(true);
        break;
    case Preset::BossVisibility:
        profile.MinimumRank.set(3);
        profile.Attackable.set(1);
        break;
    case Preset::HidePlayerClutter:
        profile.HideStrangers.set(true);
        profile.HidePlayerOwned.set(true);
        break;
    case Preset::MaximumPerformance:
        profile.UnhideLowQuality.set(true);
        profile.MinimumRank.set(2);
        profile.HidePlayers.set(true);
        profile.HidePlayerOwned.set(true);
        profile.MaxNpcs.set(50);
        break;
    }

    profile.resolveConflicts();
}
