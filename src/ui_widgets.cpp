#include "ui.hpp"

#include "imgui.h"

float ui::labelOffset = -1;
float ui::fieldWidth  = -1;

void ui::tooltip(const char* text)
{
    if (!ImGui::IsItemHovered())
        return;

    ImGui::BeginTooltip();
    ImGui::TextUnformatted(text);
    ImGui::EndTooltip();
}

bool ui::checkbox(const char* label, const char* id, bool& value, const char* tip)
{
    const float initialCursorX = ImGui::GetCursorPosX();
    ImGui::Text("%s", label);
    tooltip(tip);
    ImGui::SameLine();
    ImGui::SetCursorPosX(initialCursorX + labelOffset);
    return ImGui::Checkbox(id, &value);
}

bool ui::combo(const char* label, const char* id, int& value, const char* const* items, int count, const char* tip)
{
    const float initialCursorX = ImGui::GetCursorPosX();
    ImGui::Text("%s", label);
    tooltip(tip);
    ImGui::SameLine();
    ImGui::SetCursorPosX(initialCursorX + labelOffset);
    ImGui::PushItemWidth(fieldWidth);
    const bool result = ImGui::Combo(id, &value, items, count);
    ImGui::PopItemWidth();
    return result;
}

bool ui::sliderInt(
    const char*   label,
    const char*   id,
    int32_t&      value,
    const int32_t min,
    const int32_t max,
    const char*   fmt,
    const char*   tip
)
{
    const float initialCursorX = ImGui::GetCursorPosX();
    ImGui::Text("%s", label);
    tooltip(tip);
    ImGui::SameLine();
    ImGui::SetCursorPosX(initialCursorX + labelOffset);
    ImGui::PushItemWidth(fieldWidth);
    const bool result = ImGui::SliderInt(id, &value, min, max, fmt);
    ImGui::PopItemWidth();
    return result;
}

bool ui::textboxbutton(const char* tag, const char* hint, char* buffer, const size_t bufferSize, const char* buttonText)
{
    const float initialCursorX = ImGui::GetCursorPosX();
    const float width          = labelOffset - ImGui::GetStyle().ItemSpacing.x;
    ImGui::PushItemWidth(width);
    ImGui::InputTextWithHint(tag, hint, buffer, bufferSize);
    ImGui::PopItemWidth();
    tooltip(hint);
    ImGui::SameLine();
    ImGui::SetCursorPosX(initialCursorX + width + ImGui::GetStyle().ItemSpacing.x);
    return ImGui::Button(buttonText, { fieldWidth, 30 });
}

void ui::separatorText(const char* text)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    const float fullWidth   = ImGui::GetContentRegionAvail().x;
    const float textWidth   = ImGui::CalcTextSize(text).x;
    const float lineWidth   = std::max(1.0f, (fullWidth - textWidth - style.ItemSpacing.x * 2.0f) * 0.5f);
    const ImVec2 cursor     = ImGui::GetCursorScreenPos();
    ImDrawList*  draw       = ImGui::GetWindowDrawList();
    const float y           = cursor.y + ImGui::GetTextLineHeight() * 0.5f;
    const ImU32 color       = ImGui::GetColorU32(ImGuiCol_Separator);

    draw->AddLine(ImVec2(cursor.x, y), ImVec2(cursor.x + lineWidth, y), color);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + lineWidth + style.ItemSpacing.x);
    ImGui::TextUnformatted(text);

    const ImVec2 textEnd = ImGui::GetItemRectMax();
    draw->AddLine(ImVec2(textEnd.x + style.ItemSpacing.x, y), ImVec2(cursor.x + fullWidth, y), color);
}

bool ui::textLink(const char* label, const bool centered)
{
    const ImVec2 textSize = ImGui::CalcTextSize(label);
    const float  startX   = ImGui::GetCursorPosX();

    if (centered)
    {
        const float avail = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX(startX + (avail - textSize.x) * 0.5f);
    }

    const ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton(label, textSize);
    const bool hovered = ImGui::IsItemHovered();
    const bool clicked = ImGui::IsItemClicked();
    const ImU32 color  = ImGui::GetColorU32(hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

    ImGui::GetWindowDrawList()->AddText(pos, color, label);
    if (hovered)
    {
        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(pos.x, pos.y + textSize.y),
            ImVec2(pos.x + textSize.x, pos.y + textSize.y),
            color,
            1.0f
        );
    }

    return clicked;
}
