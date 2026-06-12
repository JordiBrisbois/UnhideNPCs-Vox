#include "ui.hpp"

#include "imgui.h"
#include "MumbleLink.hpp"
#include "options_sections.hpp"
#include "profile_ui.hpp"
#include "unpc.hpp"

void ui::renderOptions()
{
    if (!unpc::settings || !unpc::settings->loaded())
        return;

    std::unique_lock profilesLock(unpc::settings->profilesMutex());
    auto& settings = *unpc::settings;

    if (!unpc::mumbleLink || unpc::mumbleLink->getContext().isCompetitiveMode())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 100, 100, 255));
        ImGui::Text("Disabled in Competitive");
        ImGui::PopStyleColor();
        return;
    }

    if (labelOffset < 0)
    {
        labelOffset = ImGui::GetFontSize() * 12;
        fieldWidth  = labelOffset + ImGui::GetFrameHeight();
    }

    ImGui::PushTextWrapPos(0.0f);
    ImGui::TextDisabled(
        "Never Hide > Force Visible > Hide. Top to bottom priority. Disabled in competitive modes."
    );
    ImGui::PopTextWrapPos();
    ImGui::Spacing();

    renderProfileOptions();
    auto& profile = settings.profile();

    if (ImGui::CollapsingHeader("Visibility", ImGuiTreeNodeFlags_DefaultOpen))
        renderVisibilityOptions(profile);

    if (ImGui::CollapsingHeader("Misc", ImGuiTreeNodeFlags_DefaultOpen))
        renderMiscOptions(settings, profile);

    profilesLock.unlock();
    renderOptionsFooter();
}
