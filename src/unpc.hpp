#ifndef UNHIDENPCS_UNPC_HPP
#define UNHIDENPCS_UNPC_HPP
#pragma once
#include "fw/logger.hpp"
#include "fw/memory/memory.hpp"
#include "gw2.hpp"
#include "hotkey.hpp"
#include "MumbleLink.hpp"
#include "settings.hpp"
#include "version.hpp"

namespace unpc
{
    constexpr auto SIGNATURE = 1817724315;

    extern std::atomic_bool exit;
    extern std::atomic_bool stopping;

    extern std::optional<logging::Logger> logger;
    extern std::optional<Settings>        settings;
    extern std::optional<memory::Detour>  npcHook;
    extern HotkeyManager                  hotkeyManager;

    extern HANDLE hMutex;

    extern MumbleLink* mumbleLink;
    extern int32_t*    loadingScreenActive;

    extern std::atomic_uint32_t numPlayersVisible;
    extern std::atomic_uint32_t numPlayerOwnedVisible;
    extern std::atomic_uint32_t numNpcsVisible;
    extern std::atomic_uint32_t numPlayersInArea;

    void onHookTick();

    // This function may be called second, returning true means the character will be forced invisible
    bool shouldHide(
        bool    isPlayer,
        bool    isPlayerOwned,
        bool    isOwnerLocalPlayer,
        bool    isTarget,
        bool    isTargetOwned,
        bool    isAttackable,
        uint8_t rank,
        float   distance,
        bool    isFriend,
        bool    isBlocked,
        bool    isActiveGuildMember,
        bool    isGuildMember,
        bool    isPartyMember,
        bool    isSquadMember
    );

    // This function is called first, returning true means the character will be forced visible
    // if returning false, it will go on to call shouldHide
    bool shouldShow(
        bool    isPlayer,
        bool    isPlayerOwned,
        bool    isOwnerLocalPlayer,
        bool    isTarget,
        bool    isTargetOwned,
        bool    isAttackable,
        uint8_t rank,
        float   distance,
        bool    isFriend,
        bool    isBlocked,
        bool    isActiveGuildMember,
        bool    isGuildMember,
        bool    isPartyMember,
        bool    isSquadMember
    );

    [[nodiscard]] bool hasAlwaysShowPriority(
        bool    isPlayer,
        bool    isPlayerOwned,
        bool    isOwnerLocalPlayer,
        bool    isTarget,
        bool    isTargetOwned,
        bool    isAttackable,
        uint8_t rank,
        float   distance,
        bool    isFriend,
        bool    isBlocked,
        bool    isActiveGuildMember,
        bool    isGuildMember,
        bool    isPartyMember,
        bool    isSquadMember
    );

    void start();

    void stop();
}

#endif //UNHIDENPCS_UNPC_HPP
