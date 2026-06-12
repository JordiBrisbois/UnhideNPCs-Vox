#include "hotkey.hpp"
#include "hotkey_policy.hpp"

#include <cstdlib>
#include <iostream>

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
}

int main()
{
    const Hotkey ctrlF5 { .vkCode = VK_F5, .ctrl = true };
    const Hotkey anotherCtrlF5 { .vkCode = VK_F5, .ctrl = true };
    const Hotkey plainF5 { .vkCode = VK_F5 };
    const Hotkey empty {};

    expect(ctrlF5.matches(VK_F5, true, false, false), "exact modifiers must match");
    expect(!ctrlF5.matches(VK_F5, false, false, false), "missing modifier must not match");
    expect(!plainF5.matches(VK_F5, true, false, false), "extra modifier must not match");
    expect(ctrlF5.sameCombination(anotherCtrlF5), "identical combinations must be duplicates");
    expect(!ctrlF5.sameCombination(plainF5), "different modifiers must not be duplicates");
    expect(!empty.sameCombination(empty), "unassigned hotkeys must not conflict");
    expect(hotkey_policy::canTrigger(false), "hotkeys must work outside competitive mode");
    expect(!hotkey_policy::canTrigger(true), "hotkeys must be blocked in competitive mode");

    if (failures == 0)
        std::cout << "All hotkey model tests passed\n";

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
