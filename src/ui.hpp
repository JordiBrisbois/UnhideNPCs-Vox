#ifndef UNHIDENPCS_UI_HPP
#define UNHIDENPCS_UI_HPP
#pragma once

namespace ui
{
    extern float labelOffset;
    extern float fieldWidth;

    void tooltip(const char* text);

    bool checkbox(const char* label, const char* id, bool& value, const char* tip);

    bool combo(
        const char*        label,
        const char*        id,
        int&               value,
        const char* const* items,
        int                count,
        const char*        tip
    );

    bool sliderInt(
        const char* label,
        const char* id,
        int32_t&    value,
        int32_t     min,
        int32_t     max,
        const char* fmt,
        const char* tip
    );

    bool textboxbutton(const char* tag, const char* hint, char* buffer, size_t bufferSize, const char* buttonText);

    void separatorText(const char* text);

    bool textLink(const char* label, bool centered = false);

    void renderOptions();

    uintptr_t onWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    uint32_t onWndProcNexus(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}

#endif //UNHIDENPCS_UI_HPP
