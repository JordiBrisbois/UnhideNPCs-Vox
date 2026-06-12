#include "options_sections.hpp"

#include "checkbox_group.hpp"
#include "imgui.h"
#include "settings.hpp"
#include "ui.hpp"

namespace
{
    const char* ranks[] = { "Normal", "Veteran", "Elite", "Champion", "Legendary" };
    const char* modes[]  = { "Both", "Attackable", "Non-Attackable" };

    void renderConflictHints(SettingsProfile& profile)
    {
        bool anyHint = false;

        auto hint = [&](const char* text)
        {
            if (!anyHint)
            {
                ImGui::Spacing();
                anyHint = true;
            }
            ImGui::TextDisabled("%s", text);
        };

        if (profile.PlayerOwned.get() && profile.HidePlayerOwnedSelf.get())
            hint("Your pets stay hidden: Hide Mine overrides Force visible Player-Owned.");

        if (profile.HidePlayerOwned.get() && profile.AlwaysShowPlayerOwned.get())
            hint("Never hide any pets overrides Hide others' pets.");

        if (profile.HidePlayers.get()
            && (profile.AlwaysShowGroupMembers.get() || profile.AlwaysShowFriends.get()
                || profile.AlwaysShowGuildMembers.get()))
            hint("Never hide selections stay visible even with Hide all players.");

        if (profile.HideStrangers.get() && !profile.HideStrangersOwned.get())
            hint("Strangers' pets may still show — enable Strangers in Social filters > Owned.");
    }

    bool socialMatrixCheckbox(const char* id, bool& value)
    {
        ImGui::PushID(id);
        const bool changed = ImGui::Checkbox("##value", &value);
        ImGui::PopID();
        return changed;
    }

    void renderSocialFilterMatrix(SettingsProfile& profile)
    {
        struct Row
        {
            const char*            label;
            fw::Setting<bool>&     playerSetting;
            fw::Setting<bool>&     ownedSetting;
        };

        const Row rows[] = {
            { "Blocked", profile.HideBlockedPlayers, profile.HideBlockedPlayersOwned },
            { "Non-Group", profile.HideNonGroupMembers, profile.HideNonGroupMembersOwned },
            { "Non-Guild", profile.HideNonGuildMembers, profile.HideNonGuildMembersOwned },
            { "Non-Friends", profile.HideNonFriends, profile.HideNonFriendsOwned },
            { "Strangers", profile.HideStrangers, profile.HideStrangersOwned },
        };

        bool hideBlockedPlayers  = profile.HideBlockedPlayers.get();
        bool hideNonGroupMembers = profile.HideNonGroupMembers.get();
        bool hideNonGuildMembers = profile.HideNonGuildMembers.get();
        bool hideNonFriends      = profile.HideNonFriends.get();
        bool hideStrangers       = profile.HideStrangers.get();
        bool hideBlockedOwned    = profile.HideBlockedPlayersOwned.get();
        bool hideNonGroupOwned   = profile.HideNonGroupMembersOwned.get();
        bool hideNonGuildOwned   = profile.HideNonGuildMembersOwned.get();
        bool hideNonFriendsOwned = profile.HideNonFriendsOwned.get();
        bool hideStrangersOwned  = profile.HideStrangersOwned.get();

        bool* playerValues[] = {
            &hideBlockedPlayers, &hideNonGroupMembers, &hideNonGuildMembers, &hideNonFriends, &hideStrangers
        };
        bool* ownedValues[] = {
            &hideBlockedOwned, &hideNonGroupOwned, &hideNonGuildOwned, &hideNonFriendsOwned, &hideStrangersOwned
        };

        ImGui::Spacing();
        ImGui::TextUnformatted("Filter");
        ImGui::SameLine(ui::labelOffset);
        ImGui::TextUnformatted("Players");
        ImGui::SameLine(ui::labelOffset + ui::fieldWidth + ImGui::GetStyle().ItemSpacing.x);
        ImGui::TextUnformatted("Owned");
        ui::tooltip("Checking Players also enables Owned for that filter.");

        for (size_t i = 0; i < IM_ARRAYSIZE(rows); ++i)
        {
            ImGui::TextUnformatted(rows[i].label);
            ImGui::SameLine(ui::labelOffset);

            char playerId[32];
            snprintf(playerId, sizeof(playerId), "player_%zu", i);
            if (socialMatrixCheckbox(playerId, *playerValues[i]))
            {
                rows[i].playerSetting.set(*playerValues[i]);
                if (*playerValues[i])
                {
                    *ownedValues[i] = true;
                    rows[i].ownedSetting.set(true);
                }
            }

            ImGui::SameLine(ui::labelOffset + ui::fieldWidth + ImGui::GetStyle().ItemSpacing.x);

            char ownedId[32];
            snprintf(ownedId, sizeof(ownedId), "owned_%zu", i);
            if (socialMatrixCheckbox(ownedId, *ownedValues[i]))
                rows[i].ownedSetting.set(*ownedValues[i]);
        }
    }
}

void ui::renderVisibilityOptions(SettingsProfile& profile)
{
    bool alwaysShowTargetChanged {}, alwaysShowGroupChanged {}, alwaysShowFriendsChanged {},
         alwaysShowGuildChanged {}, alwaysShowTargetOwnedChanged {}, alwaysShowPlayerOwnedChanged {};
    bool alwaysShowTarget      = profile.AlwaysShowTarget.get();
    bool alwaysShowGroup       = profile.AlwaysShowGroupMembers.get();
    bool alwaysShowFriends     = profile.AlwaysShowFriends.get();
    bool alwaysShowGuild       = profile.AlwaysShowGuildMembers.get();
    bool alwaysShowTargetOwned = profile.AlwaysShowTargetOwned.get();
    bool alwaysShowPlayerOwned = profile.AlwaysShowPlayerOwned.get();

    if (checkboxGroup(
        "Never Hide",
        "Priority overrides — these stay visible even when hide rules apply",
        {
            { "Target", profile.AlwaysShowTarget.description().c_str(), &alwaysShowTarget, &alwaysShowTargetChanged },
            {
                "Group",
                profile.AlwaysShowGroupMembers.description().c_str(),
                &alwaysShowGroup,
                &alwaysShowGroupChanged
            },
            {
                "Friends",
                profile.AlwaysShowFriends.description().c_str(),
                &alwaysShowFriends,
                &alwaysShowFriendsChanged
            },
            {
                "Guild",
                profile.AlwaysShowGuildMembers.description().c_str(),
                &alwaysShowGuild,
                &alwaysShowGuildChanged
            },
            {
                "Target Pets",
                profile.AlwaysShowTargetOwned.description().c_str(),
                &alwaysShowTargetOwned,
                &alwaysShowTargetOwnedChanged
            },
            {
                "All Pets",
                profile.AlwaysShowPlayerOwned.description().c_str(),
                &alwaysShowPlayerOwned,
                &alwaysShowPlayerOwnedChanged
            },
        }
    ))
    {
        if (alwaysShowTargetChanged)
            profile.AlwaysShowTarget.set(alwaysShowTarget);
        if (alwaysShowGroupChanged)
            profile.AlwaysShowGroupMembers.set(alwaysShowGroup);
        if (alwaysShowFriendsChanged)
            profile.AlwaysShowFriends.set(alwaysShowFriends);
        if (alwaysShowGuildChanged)
            profile.AlwaysShowGuildMembers.set(alwaysShowGuild);
        if (alwaysShowTargetOwnedChanged)
            profile.AlwaysShowTargetOwned.set(alwaysShowTargetOwned);
        if (alwaysShowPlayerOwnedChanged)
            profile.AlwaysShowPlayerOwned.set(alwaysShowPlayerOwned);
    }

    bool unhideNpcsChanged {}, unhidePlayersChanged {}, playerOwnedChanged {};
    bool unhideNpcs    = profile.UnhideNPCs.get();
    bool unhidePlayers = profile.UnhidePlayers.get();
    bool playerOwned   = profile.PlayerOwned.get();

    ImGui::Spacing();
    if (checkboxGroup(
        "Force Visible",
        "Unhide categories hidden by the game (weaker than Never Hide)",
        {
            { "NPCs", profile.UnhideNPCs.description().c_str(), &unhideNpcs, &unhideNpcsChanged },
            { "Players", profile.UnhidePlayers.description().c_str(), &unhidePlayers, &unhidePlayersChanged },
            { "Player-Owned", profile.PlayerOwned.description().c_str(), &playerOwned, &playerOwnedChanged },
        }
    ))
    {
        if (unhideNpcsChanged)
            profile.UnhideNPCs.set(unhideNpcs);
        if (unhidePlayersChanged)
        {
            profile.UnhidePlayers.set(unhidePlayers);
            if (unhidePlayers)
                profile.HidePlayers.set(false);
        }
        if (playerOwnedChanged)
        {
            profile.PlayerOwned.set(playerOwned);
            if (playerOwned)
                profile.HidePlayerOwned.set(false);
        }
    }

    bool hideAllChanged {}, hideOthersOwnedChanged {}, hideMineChanged {}, hideStrangersQuickChanged {},
         hideCombatPlayersChanged {}, hideCombatOwnedChanged {};
    bool hideAllPlayers      = profile.HidePlayers.get();
    bool hideOthersOwned     = profile.HidePlayerOwned.get();
    bool hideMine            = profile.HidePlayerOwnedSelf.get();
    bool hideStrangersQuick  = profile.HideStrangers.get();
    bool hideCombatPlayers   = profile.HidePlayersInCombat.get();
    bool hideCombatOwned     = profile.HidePlayerOwnedInCombat.get();

    ImGui::Spacing();
    if (checkboxGroup(
        "Quick Hide",
        "Common hide shortcuts",
        {
            { "All Players", profile.HidePlayers.description().c_str(), &hideAllPlayers, &hideAllChanged },
            {
                "Others' Pets",
                profile.HidePlayerOwned.description().c_str(),
                &hideOthersOwned,
                &hideOthersOwnedChanged
            },
            { "My Pets", profile.HidePlayerOwnedSelf.description().c_str(), &hideMine, &hideMineChanged },
            { "Strangers", profile.HideStrangers.description().c_str(), &hideStrangersQuick, &hideStrangersQuickChanged },
            {
                "In Combat: Players",
                profile.HidePlayersInCombat.description().c_str(),
                &hideCombatPlayers,
                &hideCombatPlayersChanged
            },
            {
                "In Combat: Pets",
                profile.HidePlayerOwnedInCombat.description().c_str(),
                &hideCombatOwned,
                &hideCombatOwnedChanged
            },
        }
    ))
    {
        if (hideAllChanged)
        {
            profile.HidePlayers.set(hideAllPlayers);
            if (hideAllPlayers)
                profile.UnhidePlayers.set(false);
        }
        if (hideOthersOwnedChanged)
        {
            profile.HidePlayerOwned.set(hideOthersOwned);
            if (hideOthersOwned)
                profile.PlayerOwned.set(false);
        }
        if (hideMineChanged)
            profile.HidePlayerOwnedSelf.set(hideMine);
        if (hideStrangersQuickChanged)
        {
            profile.HideStrangers.set(hideStrangersQuick);
            if (hideStrangersQuick)
                profile.HideStrangersOwned.set(true);
        }
        if (hideCombatPlayersChanged)
            profile.HidePlayersInCombat.set(hideCombatPlayers);
        if (hideCombatOwnedChanged)
            profile.HidePlayerOwnedInCombat.set(hideCombatOwned);
    }

    renderConflictHints(profile);

    if (ImGui::TreeNodeEx("Social Filters"))
    {
        renderSocialFilterMatrix(profile);
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("NPC Filters"))
    {
        bool unhideLowQuality = profile.UnhideLowQuality.get();
        if (checkbox(
            "Low Quality Models",
            "##unhideLowQualityModels",
            unhideLowQuality,
            profile.UnhideLowQuality.description().c_str()
        ))
            profile.UnhideLowQuality.set(unhideLowQuality);

        if (auto maxDistance = static_cast<int32_t>(profile.MaximumDistance.get()); sliderInt(
            "Max Distance",
            "##maxDistance",
            maxDistance,
            0,
            1000,
            maxDistance == 0 ? "Unlimited" : "%d meters",
            profile.MaximumDistance.description().c_str()
        ))
            profile.MaximumDistance.set(static_cast<float>(maxDistance));

        int minimumRank = profile.MinimumRank.get();
        if (combo(
            "Minimum Rank",
            "##minRank",
            minimumRank,
            ranks,
            IM_ARRAYSIZE(ranks),
            profile.MinimumRank.description().c_str()
        ))
            profile.MinimumRank.set(minimumRank);

        int attackable = profile.Attackable.get();
        if (combo(
            "Attackable",
            "##attackable",
            attackable,
            modes,
            IM_ARRAYSIZE(modes),
            profile.Attackable.description().c_str()
        ))
            profile.Attackable.set(attackable);
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Limits"))
    {
        if (auto maxPlayerDistance = static_cast<int32_t>(profile.MaximumPlayerDistance.get()); sliderInt(
            "Max Player Distance",
            "##maxPlayerDistance",
            maxPlayerDistance,
            0,
            1000,
            maxPlayerDistance == 0 ? "Unlimited" : "%d meters",
            profile.MaximumPlayerDistance.description().c_str()
        ))
            profile.MaximumPlayerDistance.set(static_cast<float>(maxPlayerDistance));

        int32_t maxPlayersVisible = profile.MaxPlayersVisible.get();
        if (sliderInt(
            "Max Players",
            "##MaxPlayersVisible",
            maxPlayersVisible,
            0,
            250,
            maxPlayersVisible == 0 ? "Unlimited" : "%d",
            profile.MaxPlayersVisible.description().c_str()
        ))
            profile.MaxPlayersVisible.set(maxPlayersVisible);

        int32_t maxPlayerOwnedVisible = profile.MaxPlayerOwnedVisible.get();
        if (sliderInt(
            "Max Player-Owned",
            "##MaxPlayerOwnedVisible",
            maxPlayerOwnedVisible,
            0,
            100,
            maxPlayerOwnedVisible == 0 ? "Unlimited" : "%d",
            profile.MaxPlayerOwnedVisible.description().c_str()
        ))
            profile.MaxPlayerOwnedVisible.set(maxPlayerOwnedVisible);

        int32_t maxNpcs = profile.MaxNpcs.get();
        if (sliderInt(
            "Max NPCs",
            "##maxNpcs",
            maxNpcs,
            0,
            1000,
            maxNpcs == 0 ? "Unlimited" : "%d",
            profile.MaxNpcs.description().c_str()
        ))
            profile.MaxNpcs.set(maxNpcs);

        ImGui::TreePop();
    }
}
