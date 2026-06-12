#include "hotkey.hpp"

#include "fw/logger.hpp"
#include "imgui.h"
#include "ui.hpp"

namespace
{
    void renderHotkeyRow(
        const std::string& id,
        Hotkey&            hotkey,
        std::string&       hotkeyCapturing,
        bool&              needSave
    )
    {
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        const bool capturing = hotkeyCapturing == id;
        ImGui::TextUnformatted(capturing ? "Esc to cancel" : hotkey.label.c_str());
        if (!capturing && !hotkey.description.empty())
        {
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            ui::tooltip(hotkey.description.c_str());
        }
        ImGui::TableSetColumnIndex(1);

        if (capturing)
        {
            ImGui::PushID((id + "_capturing").c_str());
            if (ImGui::Button("Press Keys...", ImVec2(-FLT_MIN, 0)))
            {
                hotkeyCapturing.clear();
                hotkey.vkCode = 0;
                LOG_INFO("Updated hotkey \"{}\" to {}", id, hotkey.toString());
                needSave = true;
            }
        }
        else
        {
            ImGui::PushID(id.c_str());
            if (ImGui::Button(hotkey.toString().c_str(), ImVec2(-FLT_MIN, 0)))
            {
                hotkeyCapturing = id;
            }
        }

        ImGui::PopID();
    }
}

void HotkeyManager::renderHotkey(const std::string& id, Hotkey& hotkey)
{
    std::lock_guard lock(_mutex);
    renderHotkeyRow(id, hotkey, _hotkeyCapturing, _needSave);
}

void HotkeyManager::renderHotkeys()
{
    std::lock_guard lock(_mutex);
    if (!ImGui::BeginTable(
        "hotkey_table",
        2,
        ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY,
        ImVec2(0, 300)
    ))
        return;

    ImGui::TableSetupColumn("Text", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Button", ImGuiTableColumnFlags_WidthFixed, ui::fieldWidth);
    for (auto& [id, hotkey] : _hotkeys)
    {
        ImGui::TableNextRow();
        renderHotkeyRow(id, hotkey, _hotkeyCapturing, _needSave);
    }
    ImGui::EndTable();
}
