#include "unpc.hpp"

#include "visibility_rules.hpp"

namespace
{
    visibility_rules::Character characterFromGame(
        const bool    isPlayer,
        const bool    isPlayerOwned,
        const bool    isOwnerLocalPlayer,
        const bool    isTarget,
        const bool    isTargetOwned,
        const bool    isAttackable,
        const uint8_t rank,
        const float   distance,
        const bool    isFriend,
        const bool    isBlocked,
        const bool    isActiveGuildMember,
        const bool    isGuildMember,
        const bool    isPartyMember,
        const bool    isSquadMember
    )
    {
        return {
            isPlayer, isPlayerOwned, isOwnerLocalPlayer, isTarget, isTargetOwned, isAttackable, rank, distance,
            isFriend, isBlocked, isActiveGuildMember, isGuildMember, isPartyMember, isSquadMember
        };
    }

    visibility_rules::Context currentContext()
    {
        return {
            unpc::numPlayersVisible.load(),
            unpc::numPlayerOwnedVisible.load(),
            unpc::numNpcsVisible.load(),
            unpc::mumbleLink && unpc::mumbleLink->getContext().isInCombat()
        };
    }
}

bool unpc::shouldHide(
    const bool isPlayer, const bool isPlayerOwned, const bool isOwnerLocalPlayer, const bool isTarget,
    const bool isTargetOwned, const bool isAttackable, const uint8_t rank, const float distance,
    const bool isFriend, const bool isBlocked, const bool isActiveGuildMember, const bool isGuildMember,
    const bool isPartyMember, const bool isSquadMember
)
{
    return settings && visibility_rules::shouldHide(
        settings->profile(),
        characterFromGame(
            isPlayer, isPlayerOwned, isOwnerLocalPlayer, isTarget, isTargetOwned, isAttackable, rank, distance,
            isFriend, isBlocked, isActiveGuildMember, isGuildMember, isPartyMember, isSquadMember
        ),
        currentContext()
    );
}

bool unpc::shouldShow(
    const bool isPlayer, const bool isPlayerOwned, const bool isOwnerLocalPlayer, const bool isTarget,
    const bool isTargetOwned, const bool isAttackable, const uint8_t rank, const float distance,
    const bool isFriend, const bool isBlocked, const bool isActiveGuildMember, const bool isGuildMember,
    const bool isPartyMember, const bool isSquadMember
)
{
    return settings && visibility_rules::shouldShow(
        settings->profile(),
        characterFromGame(
            isPlayer, isPlayerOwned, isOwnerLocalPlayer, isTarget, isTargetOwned, isAttackable, rank, distance,
            isFriend, isBlocked, isActiveGuildMember, isGuildMember, isPartyMember, isSquadMember
        ),
        currentContext()
    );
}

bool unpc::hasAlwaysShowPriority(
    const bool isPlayer, const bool isPlayerOwned, const bool isOwnerLocalPlayer, const bool isTarget,
    const bool isTargetOwned, const bool isAttackable, const uint8_t rank, const float distance,
    const bool isFriend, const bool isBlocked, const bool isActiveGuildMember, const bool isGuildMember,
    const bool isPartyMember, const bool isSquadMember
)
{
    return settings && visibility_rules::hasAlwaysShowPriority(
        settings->profile(),
        characterFromGame(
            isPlayer, isPlayerOwned, isOwnerLocalPlayer, isTarget, isTargetOwned, isAttackable, rank, distance,
            isFriend, isBlocked, isActiveGuildMember, isGuildMember, isPartyMember, isSquadMember
        )
    );
}
