#include "hotkey.hpp"

#include <map>

std::string shortHotkeyLabel(const std::string& id)
{
    static const std::map<std::string, std::string> labels {
        { "ForceVisibility", "Refresh Visibility" },
        { "SwitchProfile", "Next Profile" },
        { "LoadScreenBoost", "Loading Boost" },
        { "UnhideNPCs", "Show NPCs" },
        { "UnhidePlayers", "Show Players" },
        { "PlayerOwned", "Show Player-Owned" },
        { "AlwaysShowTarget", "Always Show Target" },
        { "AlwaysShowGroupMembers", "Always Show Group" },
        { "AlwaysShowFriends", "Always Show Friends" },
        { "AlwaysShowGuildMembers", "Always Show Guild" },
        { "AlwaysShowTargetOwned", "Always Show Target-Owned" },
        { "AlwaysShowPlayerOwned", "Always Show Player-Owned" },
        { "UnhideLowQuality", "Low Quality Models" },
        { "HidePlayers", "Hide Players" },
        { "HidePlayerOwned", "Hide Player-Owned" },
        { "HideBlockedPlayers", "Hide Blocked Players" },
        { "HideBlockedPlayersOwned", "Hide Blocked-Owned" },
        { "HideNonGroupMembers", "Hide Non-Group Players" },
        { "HideNonGroupMembersOwned", "Hide Non-Group Owned" },
        { "HideNonGuildMembers", "Hide Non-Guild Players" },
        { "HideNonGuildMembersOwned", "Hide Non-Guild Owned" },
        { "HideNonFriends", "Hide Non-Friends" },
        { "HideNonFriendsOwned", "Hide Non-Friend Owned" },
        { "HideStrangers", "Hide Strangers" },
        { "HideStrangersOwned", "Hide Stranger-Owned" },
        { "HidePlayerOwnedSelf", "Hide My Player-Owned" },
        { "HidePlayersInCombat", "Hide Players In Combat" },
        { "HidePlayerOwnedInCombat", "Hide Owned In Combat" }
    };

    const auto found = labels.find(id);
    return found != labels.end() ? found->second : id;
}
