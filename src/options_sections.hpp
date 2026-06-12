#ifndef UNHIDENPCS_OPTIONS_SECTIONS_HPP
#define UNHIDENPCS_OPTIONS_SECTIONS_HPP
#pragma once

class Settings;
class SettingsProfile;

namespace ui
{
    void renderVisibilityOptions(SettingsProfile& profile);
    void renderMiscOptions(Settings& settings, SettingsProfile& profile);
    void renderOptionsFooter();
}

#endif //UNHIDENPCS_OPTIONS_SECTIONS_HPP
