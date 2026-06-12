#include "options_sections.hpp"

#include "hook.hpp"
#include "imgui.h"
#include "settings.hpp"
#include "ui.hpp"
#include "unpc.hpp"

#include <shellapi.h>

namespace
{
    const char* instanceModes[] = { "Always On", "Disabled in instances", "Instances only" };

#ifndef BUILDING_ON_GITHUB
    void diagnosticRow(const char* label, const uint32_t value, const char* description)
    {
        ImGui::BulletText("%s: %u", label, value);
        ui::tooltip(description);
    }

    void renderDiagnostics()
    {
        if (!ImGui::TreeNodeEx("Visibility counts"))
            return;

        ImGui::TextWrapped("Live counts for checking hide and unhide rules.");
        diagnosticRow(
            "Players in area",
            unpc::numPlayersInArea.load(),
            "Players detected nearby, including hidden players."
        );
        diagnosticRow("Visible players", unpc::numPlayersVisible.load(), "Player models currently visible.");
        diagnosticRow(
            "Visible player-owned",
            unpc::numPlayerOwnedVisible.load(),
            "Visible pets, clones, minis and other player-owned models."
        );
        diagnosticRow("Visible NPCs", unpc::numNpcsVisible.load(), "NPC models currently visible.");

        ImGui::TreePop();
    }
#endif
}

void ui::renderMiscOptions(Settings& settings, SettingsProfile& profile)
{
    int instanceBehaviour = profile.InstanceBehaviour.get();
    if (combo(
        "Instance Behaviour",
        "##instances",
        instanceBehaviour,
        instanceModes,
        IM_ARRAYSIZE(instanceModes),
        profile.InstanceBehaviour.description().c_str()
    ))
        profile.InstanceBehaviour.set(instanceBehaviour);

    bool loadScreenBoost = settings.LoadScreenBoost.get();
    if (checkbox(
        "Loading Boost",
        "##loadScreenBoost",
        loadScreenBoost,
        settings.LoadScreenBoost.description().c_str()
    ))
        settings.LoadScreenBoost.set(loadScreenBoost);

    ImGui::Spacing();
    if (ImGui::Button("Force Visibility", { labelOffset + fieldWidth, 30 }))
        ++re::forceVisibility;
    tooltip(
        "Forces all characters to be visible for the next frame\n"
        "Useful for \"resetting\" things after modifying any settings."
    );
}

void ui::renderOptionsFooter()
{
    ImGui::Spacing();
    separatorText("Advanced");

    if (ImGui::TreeNodeEx("Hotkeys", ImGuiTreeNodeFlags_NoTreePushOnOpen))
    {
        unpc::hotkeyManager.renderHotkeys();
        ImGui::TreePop();
    }
    else
    {
        unpc::hotkeyManager.stopCapturing();
    }

#ifndef BUILDING_ON_GITHUB
    renderDiagnostics();
#endif

    ImGui::Spacing();
    ImGui::TextUnformatted("Report issues on");
    ImGui::SameLine();
    if (textLink("GitHub"))
        ShellExecuteA(nullptr, "open", "https://github.com/JordiBrisbois/UnhideNPCs-Vox/", nullptr, nullptr, SW_SHOWNORMAL);
    ImGui::SameLine();

    const std::string version   = fmt::format("v{}", unpc::version::STRING);
    const float       textWidth = ImGui::CalcTextSize(version.c_str()).x;
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - textWidth - (2 * ImGui::GetStyle().ItemSpacing.x));
    ImGui::TextUnformatted(version.c_str());
}
