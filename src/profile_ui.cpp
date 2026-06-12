#include "profile_ui.hpp"

#include "hook.hpp"
#include "imgui.h"
#include "profile_presets.hpp"
#include "ui.hpp"
#include "unpc.hpp"

void ui::renderProfileOptions()
{
    separatorText("Profile");

    auto& settings = *unpc::settings;
    auto& profile  = settings.profile();

    static char profileNameBuffer[64] { 0 };
    static int  lastSelected = -1;
    int         selected = settings.ActiveProfile.get();

    std::vector<std::string> profileNames;
    profileNames.reserve(settings.children().size());
    for (const auto& child : settings.children())
        profileNames.push_back(child->name());

    std::vector<const char*> profileOptions;
    profileOptions.reserve(profileNames.size());
    for (const auto& name : profileNames)
        profileOptions.push_back(name.c_str());

    if (lastSelected != selected)
    {
        strcpy_s(profileNameBuffer, sizeof(profileNameBuffer), settings.profile().name().c_str());
        lastSelected = selected;
    }

    if (combo("Profile", "##profile", selected, profileOptions.data(), profileOptions.size(), "The profile to use"))
    {
        settings.ActiveProfile.set(selected);
        strcpy_s(profileNameBuffer, sizeof(profileNameBuffer), settings.profile().name().c_str());
        lastSelected = selected;
        ++re::forceVisibility;
    }

    ImGui::TextColored(ImVec4(0.20f, 0.90f, 0.80f, 1.00f), "Active Profile: %s", settings.profile().name().c_str());

    if (ImGui::TreeNodeEx("Manage Profiles"))
    {
        if (textboxbutton(
            "##profileName",
            "Profile Name",
            profileNameBuffer,
            std::size(profileNameBuffer),
            "Rename"
        ))
        {
            if (settings.ActiveProfile.get() > 0 && profileNameBuffer[0])
            {
                settings.renameProfile(settings.children()[settings.ActiveProfile.get()]->name(), profileNameBuffer);
                ++re::forceVisibility;
            }
        }

        if (ImGui::Button("New"))
        {
            const char* newName = profileNameBuffer[0] ? profileNameBuffer : "Profile";
            const auto  name    = settings.uniqueProfileName(newName);
            settings.addProfile(name);
            settings.ActiveProfile.set(settings.children().size() - 1);
            strcpy_s(profileNameBuffer, sizeof(profileNameBuffer), name.c_str());
            ++re::forceVisibility;
        }
        tooltip("Create a new profile");

        ImGui::SameLine();
        if (ImGui::Button("Duplicate"))
        {
            const auto sourceName = profile.name();
            const auto name       = settings.uniqueProfileName(fmt::format("{} Copy", sourceName));
            settings.duplicateProfile(sourceName, name);
            settings.ActiveProfile.set(settings.children().size() - 1);
            strcpy_s(profileNameBuffer, sizeof(profileNameBuffer), name.c_str());
            ++re::forceVisibility;
        }
        tooltip("Create a complete copy of the current profile");

        ImGui::SameLine();
        if (ImGui::Button("Remove"))
        {
            const int index = settings.ActiveProfile.get();
            if (index > 0)
            {
                settings.ActiveProfile.set(index - 1);
                settings.removeProfile(settings.children()[index]->name());
                strcpy_s(
                    profileNameBuffer,
                    sizeof(profileNameBuffer),
                    settings.children()[settings.ActiveProfile.get()]->name().c_str()
                );
                ++re::forceVisibility;
            }
        }
        tooltip("Remove current profile");
        ImGui::TreePop();
    }

    static int preset = 0;
    if (combo(
        "Preset",
        "##profilePreset",
        preset,
        profile_presets::names,
        IM_ARRAYSIZE(profile_presets::names),
        "Apply a ready-to-use configuration to the current profile immediately"
    ))
    {
        profile_presets::apply(settings.profile(), static_cast<profile_presets::Preset>(preset));
        ++re::forceVisibility;
    }

    ImGui::Spacing();
}
