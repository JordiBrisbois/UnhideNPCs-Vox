#include "visibility_rules.hpp"
#include "profile_presets.hpp"

#include <cstdlib>
#include <iostream>

void forceVisibility(fw::SettingRefVariant&)
{
}

fw::Settings::Settings(Settings* owner, std::string name, std::filesystem::path filePath)
    : _owner(owner), _name(std::move(name)), _filePath(std::move(filePath))
{
}

void fw::Settings::needSave()
{
}

std::vector<fw::SettingVariant>& fw::Settings::settings()
{
    return _settings;
}

namespace
{
    int failures = 0;

    void expect(const bool condition, const char* message)
    {
        if (condition)
            return;

        std::cerr << "FAIL: " << message << '\n';
        ++failures;
    }

    SettingsProfile defaultProfile()
    {
        return SettingsProfile(nullptr, "Test");
    }

    void targetOverridesEveryRule()
    {
        auto profile = defaultProfile();
        profile.HidePlayers.set(true, false);
        profile.AlwaysShowTarget.set(true, false);

        visibility_rules::Character character { .player = true, .target = true };
        const visibility_rules::Context context {};

        expect(!visibility_rules::shouldHide(profile, character, context), "target must never be hidden");
        expect(visibility_rules::shouldShow(profile, character, context), "target must always be shown");
    }

    void socialPriorityOverridesHideRules()
    {
        auto profile = defaultProfile();
        profile.HidePlayers.set(true, false);
        profile.HidePlayerOwned.set(true, false);
        profile.MaximumPlayerDistance.set(50.0f, false);
        const visibility_rules::Context context {};

        profile.AlwaysShowGroupMembers.set(true, false);
        visibility_rules::Character grouped { .player = true, .distance = 999.0f, .partyMember = true };
        expect(!visibility_rules::shouldHide(profile, grouped, context), "group member must bypass hide rules");
        expect(visibility_rules::shouldShow(profile, grouped, context), "group member must remain visible");

        profile.AlwaysShowGroupMembers.set(false, false);
        profile.AlwaysShowFriends.set(true, false);
        visibility_rules::Character friendPlayer { .player = true, .distance = 999.0f, .friendPlayer = true };
        expect(!visibility_rules::shouldHide(profile, friendPlayer, context), "friend must bypass hide rules");

        profile.AlwaysShowFriends.set(false, false);
        profile.AlwaysShowGuildMembers.set(true, false);
        visibility_rules::Character guildMember { .player = true, .distance = 999.0f, .guildMember = true };
        expect(!visibility_rules::shouldHide(profile, guildMember, context), "guild member must bypass hide rules");
    }

    void targetOwnedOverridesHideRules()
    {
        auto profile = defaultProfile();
        profile.HidePlayerOwned.set(true, false);
        profile.AlwaysShowTargetOwned.set(true, false);

        const visibility_rules::Context context {};
        const visibility_rules::Character targetPet { .playerOwned = true, .targetOwned = true };

        expect(!visibility_rules::shouldHide(profile, targetPet, context), "target-owned must bypass hide rules");
        expect(visibility_rules::shouldShow(profile, targetPet, context), "target-owned must remain visible");
    }

    void playerOwnedOverridesHideRules()
    {
        auto profile = defaultProfile();
        profile.HidePlayerOwned.set(true, false);
        profile.HidePlayerOwnedSelf.set(true, false);
        profile.AlwaysShowPlayerOwned.set(true, false);

        const visibility_rules::Context context {};
        const visibility_rules::Character theirs { .playerOwned = true };
        const visibility_rules::Character mine { .playerOwned = true, .ownerLocalPlayer = true };

        expect(!visibility_rules::shouldHide(profile, theirs, context), "player-owned must bypass hide rules");
        expect(visibility_rules::shouldShow(profile, theirs, context), "player-owned must remain visible");
        expect(!visibility_rules::shouldHide(profile, mine, context), "own player-owned must bypass hide rules");
    }

    void playerDistanceFilterIsApplied()
    {
        auto profile = defaultProfile();
        profile.MaximumPlayerDistance.set(100.0f, false);

        const visibility_rules::Context context {};
        const visibility_rules::Character nearby { .player = true, .distance = 99.0f };
        const visibility_rules::Character farAway { .player = true, .distance = 100.0f };
        const visibility_rules::Character ownedNearby { .playerOwned = true, .distance = 99.0f };
        const visibility_rules::Character ownedFarAway { .playerOwned = true, .distance = 100.0f };

        expect(!visibility_rules::shouldHide(profile, nearby, context), "nearby player must not be hidden by distance");
        expect(visibility_rules::shouldHide(profile, farAway, context), "far player must be hidden by distance");
        expect(
            !visibility_rules::shouldHide(profile, ownedNearby, context),
            "nearby player-owned must not be hidden by distance"
        );
        expect(
            visibility_rules::shouldHide(profile, ownedFarAway, context),
            "far player-owned must be hidden by distance"
        );

        profile.AlwaysShowFriends.set(true, false);
        visibility_rules::Character farFriend { .player = true, .distance = 500.0f, .friendPlayer = true };
        expect(!visibility_rules::shouldHide(profile, farFriend, context), "always-show friend must ignore distance");
    }

    void playerFiltersAreSymmetric()
    {
        auto profile = defaultProfile();
        profile.UnhidePlayers.set(true, false);
        profile.HideStrangers.set(true, false);

        visibility_rules::Character stranger { .player = true };
        visibility_rules::Character friendPlayer { .player = true, .friendPlayer = true };
        const visibility_rules::Context context {};

        expect(visibility_rules::shouldHide(profile, stranger, context), "stranger player must be hidden");
        expect(!visibility_rules::shouldShow(profile, stranger, context), "stranger player must not be shown");
        expect(!visibility_rules::shouldHide(profile, friendPlayer, context), "friend player must not be hidden");
        expect(visibility_rules::shouldShow(profile, friendPlayer, context), "friend player must be shown");
    }

    void playerMembershipFiltersAreApplied()
    {
        auto profile = defaultProfile();
        const visibility_rules::Context context {};
        const visibility_rules::Character outsider { .player = true };
        const visibility_rules::Character grouped { .player = true, .partyMember = true };
        const visibility_rules::Character guildMember { .player = true, .guildMember = true };
        const visibility_rules::Character blocked { .player = true, .blockedPlayer = true };

        profile.HideNonGroupMembers.set(true, false);
        expect(visibility_rules::shouldHide(profile, outsider, context), "outsider must obey non-group filter");
        expect(!visibility_rules::shouldHide(profile, grouped, context), "party member must pass non-group filter");

        profile.HideNonGroupMembers.set(false, false);
        profile.HideNonGuildMembers.set(true, false);
        expect(!visibility_rules::shouldHide(profile, guildMember, context), "guild member must pass guild filter");

        profile.HideNonGuildMembers.set(false, false);
        profile.HideBlockedPlayers.set(true, false);
        expect(visibility_rules::shouldHide(profile, blocked, context), "blocked player must be hidden");
    }

    void combatFiltersAreApplied()
    {
        auto profile = defaultProfile();
        profile.HidePlayersInCombat.set(true, false);
        profile.HidePlayerOwnedInCombat.set(true, false);

        const visibility_rules::Context combat { .inCombat = true };
        expect(
            visibility_rules::shouldHide(profile, { .player = true }, combat),
            "player must obey combat filter"
        );
        expect(
            visibility_rules::shouldHide(profile, { .playerOwned = true }, combat),
            "other player-owned character must obey combat filter"
        );
        expect(
            !visibility_rules::shouldHide(profile, { .playerOwned = true, .ownerLocalPlayer = true }, combat),
            "own player-owned character must ignore combat filter"
        );
    }

    void conflictingTogglesResolveImmediately()
    {
        auto profile = defaultProfile();
        profile.HidePlayers.set(true, false);
        expect(profile.toggleBoolean("UnhidePlayers"), "UnhidePlayers toggle must exist");
        expect(profile.UnhidePlayers.get(), "UnhidePlayers must be enabled");
        expect(!profile.HidePlayers.get(), "enabling UnhidePlayers must disable HidePlayers");

        profile.HidePlayerOwned.set(true, false);
        expect(profile.toggleBoolean("PlayerOwned"), "PlayerOwned toggle must exist");
        expect(profile.PlayerOwned.get(), "PlayerOwned must be enabled");
        expect(!profile.HidePlayerOwned.get(), "enabling PlayerOwned must disable HidePlayerOwned");
    }

    void defaultPresetRestoresFactoryValues()
    {
        auto profile = defaultProfile();
        profile.UnhideNPCs.set(false, false);
        profile.HidePlayers.set(true, false);
        profile.MaxNpcs.set(42, false);

        profile_presets::apply(profile, profile_presets::Preset::Default);

        expect(profile.UnhideNPCs.get(), "default preset must restore UnhideNPCs");
        expect(!profile.HidePlayers.get(), "default preset must restore HidePlayers");
        expect(profile.MaxNpcs.get() == 0, "default preset must restore MaxNpcs");
        expect(profile.AlwaysShowTarget.get(), "default preset must restore AlwaysShowTarget");
    }

    void playerLimitsUseCurrentVisibleCount()
    {
        auto profile = defaultProfile();
        profile.UnhidePlayers.set(true, false);
        profile.MaxPlayersVisible.set(2, false);

        const visibility_rules::Character player { .player = true };

        expect(
            visibility_rules::shouldShow(profile, player, { .playersVisible = 1 }),
            "player below limit must be shown"
        );
        expect(
            !visibility_rules::shouldShow(profile, player, { .playersVisible = 2 }),
            "player at limit must not be shown"
        );
        expect(
            visibility_rules::shouldHide(profile, player, { .playersVisible = 3 }),
            "player above limit must be hidden"
        );
    }

    void ownedRulesDistinguishLocalOwner()
    {
        auto profile = defaultProfile();
        profile.PlayerOwned.set(true, false);
        profile.HidePlayerOwned.set(true, false);

        const visibility_rules::Character mine { .playerOwned = true, .ownerLocalPlayer = true };
        const visibility_rules::Character theirs { .playerOwned = true };
        const visibility_rules::Context context {};

        expect(!visibility_rules::shouldHide(profile, mine, context), "own character must survive HidePlayerOwned");
        expect(visibility_rules::shouldShow(profile, mine, context), "own character must remain showable");
        expect(visibility_rules::shouldHide(profile, theirs, context), "other owned character must be hidden");

        profile.HidePlayerOwnedSelf.set(true, false);
        expect(visibility_rules::shouldHide(profile, mine, context), "own character must obey HidePlayerOwnedSelf");
    }

    void npcConstraintsAreApplied()
    {
        auto profile = defaultProfile();
        profile.MinimumRank.set(2, false);
        profile.Attackable.set(1, false);
        profile.MaximumDistance.set(100.0f, false);

        const visibility_rules::Context context {};
        const visibility_rules::Character eligible { .attackable = true, .rank = 2, .distance = 99.0f };
        const visibility_rules::Character tooWeak { .attackable = true, .rank = 1, .distance = 99.0f };
        const visibility_rules::Character tooFar { .attackable = true, .rank = 2, .distance = 100.0f };
        const visibility_rules::Character passive { .rank = 2, .distance = 99.0f };

        expect(visibility_rules::shouldShow(profile, eligible, context), "eligible NPC must be shown");
        expect(!visibility_rules::shouldShow(profile, tooWeak, context), "low-rank NPC must not be shown");
        expect(!visibility_rules::shouldShow(profile, tooFar, context), "NPC at maximum distance must not be shown");
        expect(!visibility_rules::shouldShow(profile, passive, context), "passive NPC must not pass attackable filter");
    }
}

int main()
{
    targetOverridesEveryRule();
    socialPriorityOverridesHideRules();
    targetOwnedOverridesHideRules();
    playerOwnedOverridesHideRules();
    playerDistanceFilterIsApplied();
    playerFiltersAreSymmetric();
    playerMembershipFiltersAreApplied();
    combatFiltersAreApplied();
    playerLimitsUseCurrentVisibleCount();
    ownedRulesDistinguishLocalOwner();
    npcConstraintsAreApplied();
    conflictingTogglesResolveImmediately();
    defaultPresetRestoresFactoryValues();

    if (failures == 0)
        std::cout << "All visibility rule tests passed\n";

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
