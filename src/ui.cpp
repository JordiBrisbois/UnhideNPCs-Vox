#include "ui.hpp"

#include "unpc.hpp"

uintptr_t ui::onWndProc(HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
    return unpc::hotkeyManager.onWndProc(hWnd, msg, wParam, lParam);
}

uint32_t ui::onWndProcNexus(HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
    return onWndProc(hWnd, msg, wParam, lParam);
}
