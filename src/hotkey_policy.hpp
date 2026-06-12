#ifndef UNHIDENPCS_HOTKEY_POLICY_HPP
#define UNHIDENPCS_HOTKEY_POLICY_HPP
#pragma once

namespace hotkey_policy
{
    [[nodiscard]] constexpr bool canTrigger(const bool competitiveMode)
    {
        return !competitiveMode;
    }
}

#endif //UNHIDENPCS_HOTKEY_POLICY_HPP
