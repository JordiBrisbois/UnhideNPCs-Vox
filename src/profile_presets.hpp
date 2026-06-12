#ifndef UNHIDENPCS_PROFILE_PRESETS_HPP
#define UNHIDENPCS_PROFILE_PRESETS_HPP
#pragma once

#include "settings.hpp"

namespace profile_presets
{
    enum class Preset
    {
        Default,
        ShowEverything,
        BossVisibility,
        HidePlayerClutter,
        MaximumPerformance
    };

    inline constexpr const char* names[] = {
        "Recommended Default",
        "Show Everything",
        "Boss Visibility",
        "Hide Player Clutter",
        "Maximum Performance"
    };

    void apply(SettingsProfile& profile, Preset preset);
}

#endif //UNHIDENPCS_PROFILE_PRESETS_HPP
