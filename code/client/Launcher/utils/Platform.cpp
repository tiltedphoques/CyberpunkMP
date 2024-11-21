#include "Platform.h"
#include <Windows.h>

bool GetWindowTitleStartingWith(const char* acpStart, std::string& aResult)
{
    char data[8192];
    size_t len = strlen(acpStart);

    HWND hWnd = FindWindowA(nullptr, nullptr);
    while (hWnd)
    {
        GetWindowTextA(hWnd, data, std::size(data));
        if (strncmp(data, acpStart, len) == 0)
        {
            aResult = data;
            return true;
        }

        hWnd = GetWindow(hWnd, GW_HWNDNEXT);
    }

    return false;
}
