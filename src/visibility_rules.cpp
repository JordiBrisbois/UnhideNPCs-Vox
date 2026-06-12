#include "visibility_rules.hpp"

namespace
{
    bool isGroupMember(const visibility_rules::Character& character)
    {
        return character.partyMember || character.squadMember;
    }

    bool isGuildMember(const visibility_rules::Character& character)
    {
        return character.activeGuildMember || character.guildMember;
    }

    bool isStranger(const visibility_rules::Character& character)
    {
        return !character.friendPlayer && !isGuildMember(character) && !isGroupMember(character);
    }

    bool beyondPlayerDistance(const SettingsProfile& profile, const visibility_rules::Character& character)
    {
        const auto maxDistance = profile.MaximumPlayerDistance.get();
        return maxDistance > 0 && character.distance >= maxDistance;
    }

    bool playerFiltered(
        const SettingsProfile& profile,
        const visibility_rules::Character& character,
        const visibility_rules::Context& context
    )
    {
        return profile.HidePlayers.get()
            || (profile.HideNonFriends.get() && !character.friendPlayer)
            || (profile.HideBlockedPlayers.get() && character.blockedPlayer)
            || (profile.HideNonGuildMembers.get() && !isGuildMember(character))
            || (profile.HideNonGroupMembers.get() && !isGroupMember(character))
            || (profile.HideStrangers.get() && isStranger(character))
            || (profile.HidePlayersInCombat.get() && context.inCombat);
    }

    bool playerOwnedFiltered(
        const SettingsProfile& profile,
        const visibility_rules::Character& character,
        const visibility_rules::Context& context
    )
    {
        return (profile.HidePlayerOwnedSelf.get() && character.ownerLocalPlayer)
            || (profile.HidePlayerOwned.get() && !character.ownerLocalPlayer)
            || (profile.HideNonFriendsOwned.get() && !character.friendPlayer)
            || (profile.HideBlockedPlayersOwned.get() && character.blockedPlayer)
            || (profile.HideNonGuildMembersOwned.get() && !isGuildMember(character))
            || (profile.HideNonGroupMembersOwned.get() && !isGroupMember(character))
            || (profile.HideStrangersOwned.get() && !character.ownerLocalPlayer && isStranger(character))
            || (profile.HidePlayerOwnedInCombat.get() && !character.ownerLocalPlayer && context.inCombat);
    }
}

bool visibility_rules::hasAlwaysShowPriority(const SettingsProfile& profile, const Character& character)
{
    if (character.target && profile.AlwaysShowTarget.get())
        return true;
    if (character.targetOwned && profile.AlwaysShowTargetOwned.get())
        return true;
    if (profile.AlwaysShowGroupMembers.get() && isGroupMember(character))
        return true;
    if (profile.AlwaysShowFriends.get() && character.friendPlayer)
        return true;
    if (profile.AlwaysShowGuildMembers.get() && isGuildMember(character))
        return true;
    if (character.playerOwned && profile.AlwaysShowPlayerOwned.get())
        return true;
    return false;
}

bool visibility_rules::shouldHide(
    const SettingsProfile& profile,
    const Character&       character,
    const Context&         context
)
{
    if (hasAlwaysShowPriority(profile, character))
        return false;

    if (character.player)
    {
        return beyondPlayerDistance(profile, character)
            || (profile.MaxPlayersVisible.get() > 0
                && context.playersVisible > static_cast<uint32_t>(profile.MaxPlayersVisible.get()))
            || playerFiltered(profile, character, context);
    }

    if (character.playerOwned)
    {
        return beyondPlayerDistance(profile, character)
            || (profile.MaxPlayerOwnedVisible.get() > 0
                && context.playerOwnedVisible > static_cast<uint32_t>(profile.MaxPlayerOwnedVisible.get()))
            || playerOwnedFiltered(profile, character, context);
    }

    return profile.MaxNpcs.get() > 0 && context.npcsVisible > static_cast<uint32_t>(profile.MaxNpcs.get());
}

bool visibility_rules::shouldShow(
    const SettingsProfile& profile,
    const Character&       character,
    const Context&         context
)
{
    if (hasAlwaysShowPriority(profile, character))
        return true;

    if (character.player)
    {
        return profile.UnhidePlayers.get()
            && !beyondPlayerDistance(profile, character)
            && (profile.MaxPlayersVisible.get() == 0
                || context.playersVisible < static_cast<uint32_t>(profile.MaxPlayersVisible.get()))
            && !playerFiltered(profile, character, context);
    }

    if (character.playerOwned)
    {
        return profile.PlayerOwned.get()
            && !beyondPlayerDistance(profile, character)
            && (profile.MaxPlayerOwnedVisible.get() == 0
                || context.playerOwnedVisible < static_cast<uint32_t>(profile.MaxPlayerOwnedVisible.get()))
            && !playerOwnedFiltered(profile, character, context);
    }

    if (!profile.UnhideNPCs.get())
        return false;
    if (profile.MaxNpcs.get() > 0 && context.npcsVisible >= static_cast<uint32_t>(profile.MaxNpcs.get()))
        return false;
    if (character.rank < profile.MinimumRank.get())
        return false;
    if (profile.Attackable.get() == 1 && !character.attackable)
        return false;
    if (profile.Attackable.get() == 2 && character.attackable)
        return false;
    if (profile.MaximumDistance.get() > 0 && character.distance >= profile.MaximumDistance.get())
        return false;

    return true;
}
