#ifndef UNHIDENPCS_PROFILE_SETTINGS_HPP
#define UNHIDENPCS_PROFILE_SETTINGS_HPP
#pragma once
#include <mutex>

#include "hook.hpp"
#include "gw2.hpp"
#include "fw/settings.hpp"

#define S(type, name, value, comment) fw::Setting<type>& name = add<type>(value, #name, comment)

void forceVisibility(fw::SettingRefVariant&);

class SettingsProfile : public fw::Settings
{
protected:
    std::unique_ptr<fw::Settings> createChild(const std::string& name) override;

public:
    fw::Setting<bool>& UnhideNPCs = add<bool>(true, "UnhideNPCs", "Unhide NPCs").onChanged(forceVisibility);

    S(bool, UnhidePlayers, false, "Unhide players").onChanged(forceVisibility);
    S(
        bool,
        PlayerOwned,
        false,
        "Unhide player-owned characters (pets, clones, minis etc).\n"
        "Does not override Hide Player-Owned > Mine for your own characters."
    ).onChanged(forceVisibility);

    S(bool, AlwaysShowTarget, true, "Always show the targeted character, even if it would be hidden.").onChanged(
        forceVisibility
    );
    S(
        bool,
        AlwaysShowGroupMembers,
        false,
        "Always show party and squad members, even if they would be hidden."
    ).onChanged(forceVisibility);
    S(bool, AlwaysShowFriends, false, "Always show friends, even if they would be hidden.").onChanged(forceVisibility);
    S(
        bool,
        AlwaysShowGuildMembers,
        false,
        "Always show guild members, even if they would be hidden."
    ).onChanged(forceVisibility);
    S(
        bool,
        AlwaysShowTargetOwned,
        false,
        "Always show characters owned by your target (pets, clones, minis etc)."
    ).onChanged(forceVisibility);
    S(
        bool,
        AlwaysShowPlayerOwned,
        false,
        "Always show all player-owned characters (pets, clones, minis etc), even if they would be hidden."
    ).onChanged(forceVisibility);

    S(bool, UnhideLowQuality, false, "Use low quality models when unhiding characters").onChanged(forceVisibility);

    S(int32_t, MinimumRank, 0, "Only NPCs that have at least this rank get unhidden.").onChanged(forceVisibility).
validator(
            [](auto& value)
            {
                if (value < 0 || value >= static_cast<int32_t>(re::gw2::ECharacterRankMax))
                {
                    value = 0;
                }
            }
        );

    S(int32_t, Attackable, 0, "Only NPCs that match this get unhidden.").onChanged(forceVisibility).validator(
        [](auto& value)
        {
            if (value < 0 || value > 2)
            {
                value = 0;
            }
        }
    );

    S(float, MaximumDistance, 0, "Characters outside of this distance won't be unhidden. (0=unlimited)").onChanged(
        forceVisibility
    ).validator(
        [](auto& value)
        {
            if (value < 0)
            {
                value = 0;
            }
            else if (value > 1000)
            {
                value = 1000;
            }
        }
    );

    S(
        float,
        MaximumPlayerDistance,
        0,
        "Players and player-owned characters beyond this distance are hidden. (0=unlimited)"
    ).onChanged(forceVisibility).validator(
        [](auto& value)
        {
            if (value < 0)
            {
                value = 0;
            }
            else if (value > 1000)
            {
                value = 1000;
            }
        }
    );

    S(bool, HidePlayers, false, "Hide all players (Not yourself)").onChanged(forceVisibility);

    S(
        bool,
        HidePlayerOwned,
        false,
        "Hide all characters owned by players, except yourself (pets, clones, minis etc)"
    ).onChanged(forceVisibility);

    S(bool, HideBlockedPlayers, false, "Hide any players that you have blocked").onChanged(forceVisibility);
    S(
        bool,
        HideBlockedPlayersOwned,
        false,
        "Hide any characters that are owned by blocked players (pets, clones, minis etc)"
    ).onChanged(forceVisibility);

    S(bool, HideNonGroupMembers, false, "Hide any players who are not in the same group as you (Party or Squad)").onChanged(forceVisibility);

    S(
        bool,
        HideNonGroupMembersOwned,
        false,
        "Hide any characters that are owned by non-group members (pets, clones, minis etc)"
    ).onChanged(forceVisibility);

    S(bool, HideNonGuildMembers, false, "Hide any players that aren't guild members").onChanged(forceVisibility);

    S(
        bool,
        HideNonGuildMembersOwned,
        false,
        "Hide any characters that are owned by non-guild members (pets, clones, minis etc)"
    ).onChanged(forceVisibility);

    S(bool, HideNonFriends, false, "Hide any players that aren't friends").onChanged(forceVisibility);

    S(
        bool,
        HideNonFriendsOwned,
        false,
        "Hide any characters that are owned by non-friend players (pets, clones, minis etc)"
    ).onChanged(forceVisibility);

    S(bool, HideStrangers, false, "Hide any players who are not: friends, guild, party or squad members").onChanged(
        forceVisibility
    );

    S(
        bool,
        HideStrangersOwned,
        false,
        "Hide any characters that are owned by strangers (pets, clones, minis etc)"
    ).onChanged(forceVisibility);

    S(
        bool,
        HidePlayerOwnedSelf,
        false,
        "Hide your pets, clones, minis etc.\n"
        "Takes priority over Unhide Player-Owned for your own characters."
    ).onChanged(forceVisibility);

    S(bool, HidePlayersInCombat, false, "Hide players when you are in combat").onChanged(forceVisibility);

    S(bool, HidePlayerOwnedInCombat, false, "Hide player-owned characters when you are in combat").onChanged(
        forceVisibility
    );

    S(int32_t, MaxPlayersVisible, 0, "Maximum number of visible players. (0=unlimited)").onChanged(forceVisibility).
validator(
            [](auto& value)
            {
                if (value < 0)
                {
                    value = 0;
                }
                else if (value > 1000)
                {
                    value = 1000;
                }
            }
        );

    S(int32_t, MaxPlayerOwnedVisible, 0, "Maximum number of visible player-owned characters. (0=unlimited)").onChanged(
        forceVisibility
    ).validator(
        [](auto& value)
        {
            if (value < 0)
            {
                value = 0;
            }
            else if (value > 1000)
            {
                value = 1000;
            }
        }
    );

    S(int32_t, MaxNpcs, 0, "Maximum number of visible NPCs. (0=unlimited)").onChanged(forceVisibility).validator(
        [](auto& value)
        {
            if (value < 0)
            {
                value = 0;
            }
            else if (value > 1000)
            {
                value = 1000;
            }
        }
    );

    S(int32_t, InstanceBehaviour, 0, "0: Always On\n1: Disabled in instances\n2: Instances only").onChanged(
        forceVisibility
    ).validator(
        [](auto& value)
        {
            if (value < 0 || value > 2)
            {
                value = 0;
            }
        }
    );

    explicit SettingsProfile(fw::Settings* owner, const std::string& name) : fw::Settings(owner, name, "") {}

    void copyFrom(SettingsProfile& source);
    bool toggleBoolean(const std::string& name);
    void resolveConflicts();
};

#undef S

#endif //UNHIDENPCS_PROFILE_SETTINGS_HPP
