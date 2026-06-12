#ifndef UNHIDENPCS_VISIBILITY_RULES_HPP
#define UNHIDENPCS_VISIBILITY_RULES_HPP
#pragma once

#include "profile_settings.hpp"

namespace visibility_rules
{
    struct Character
    {
        bool    player {};
        bool    playerOwned {};
        bool    ownerLocalPlayer {};
        bool    target {};
        bool    targetOwned {};
        bool    attackable {};
        uint8_t rank {};
        float   distance {};
        bool    friendPlayer {};
        bool    blockedPlayer {};
        bool    activeGuildMember {};
        bool    guildMember {};
        bool    partyMember {};
        bool    squadMember {};
    };

    struct Context
    {
        uint32_t playersVisible {};
        uint32_t playerOwnedVisible {};
        uint32_t npcsVisible {};
        bool     inCombat {};
    };

    [[nodiscard]] bool hasAlwaysShowPriority(const SettingsProfile& profile, const Character& character);
    [[nodiscard]] bool shouldHide(const SettingsProfile& profile, const Character& character, const Context& context);
    [[nodiscard]] bool shouldShow(const SettingsProfile& profile, const Character& character, const Context& context);
}

#endif //UNHIDENPCS_VISIBILITY_RULES_HPP
