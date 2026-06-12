#include "checkbox_group.hpp"

#include "imgui.h"
#include "ui.hpp"

bool checkboxGroup(const char* label, const char* tooltip, const std::vector<CheckboxGroupEntry>& items)
{
    const size_t count = items.size();

    if (count == 0)
        return false;

    const char* groupId = label ? label : "group";

    auto checkboxId = [&](const CheckboxGroupEntry& item, char* buffer, const size_t bufferSize)
    {
        snprintf(
            buffer,
            bufferSize,
            "##%s_%s",
            groupId,
            item.label ? item.label : "item"
        );
    };

    char id[48];
    if (count == 1)
    {
        checkboxId(items[0], id, sizeof(id));
        const bool result = ui::checkbox(items[0].label, id, *items[0].value, items[0].tooltip);
        if (result)
            *items[0].changed = true;
        return result;
    }

    const ImVec2 initialCursorPos = ImGui::GetCursorPos();
    const ImVec2 spacing          = ImGui::GetStyle().ItemSpacing;
    const float  checkBoxSize     = ImGui::GetFrameHeight();
    const float  rowHeight        = spacing.y + checkBoxSize;

    if (label)
    {
        ImGui::TextUnformatted(label);
        if (tooltip)
            ui::tooltip(tooltip);
    }

    bool result = false;

    for (size_t i = 0; i < count; ++i)
    {
        const bool   isOdd = (i & 1) != 0;
        const size_t row   = i >> 1;
        const auto&  item  = items[i];

        const float y = initialCursorPos.y + static_cast<float>(row) * rowHeight;
        const float x = initialCursorPos.x + (isOdd ? 2.0f : 1.0f) * ui::labelOffset;

        if (item.label)
        {
            const ImVec2 labelSize = ImGui::CalcTextSize(item.label);
            const float  labelX    = x - spacing.x - labelSize.x;

            ImGui::SetCursorPos(ImVec2(labelX, y + ImGui::GetStyle().FramePadding.y));
            ImGui::TextUnformatted(item.label);
            if (item.tooltip)
                ui::tooltip(item.tooltip);
        }

        ImGui::SetCursorPos(ImVec2(x, y));
        checkboxId(item, id, sizeof(id));

        if (ImGui::Checkbox(id, item.value))
        {
            *item.changed = true;
            result        = true;
        }
    }

    const size_t totalRows = (count + 1) >> 1;
    ImGui::SetCursorPos(ImVec2(initialCursorPos.x, initialCursorPos.y + static_cast<float>(totalRows) * rowHeight));

    return result;
}
