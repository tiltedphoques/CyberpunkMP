#include "Debug.h"
#include <thread>
#include <cassert>

#include <windows.h>


static PVOID s_pVectoredHandler = nullptr;

void Debug::WaitForDebugger() noexcept
{
    while (!IsDebuggerPresent())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void Debug::CreateConsole() noexcept
{
    if (AllocConsole())
    {
        FILE* file = nullptr;
        freopen_s(&file, "CONOUT$", "w", stdout);
        assert(file != nullptr);
        SetConsoleTitleA("Tilted Reverse Console");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
    }
}

LONG WINAPI OnException(PEXCEPTION_POINTERS apExceptionInfo)
{
    // Release Skyrim's mouse capture - best effort
    ReleaseCapture();

    // Perhaps EXCEPTION_EXECUTE_HANDLER is more in order, in case someone after us handles the exception gracefully?
    return EXCEPTION_CONTINUE_SEARCH;

    UNREFERENCED_PARAMETER(apExceptionInfo);
}

void Debug::OnAttach() noexcept
{
    const int cCallLast = 0;
    const int cCallFirst = 1;
    assert(s_pVectoredHandler == NULL);
    s_pVectoredHandler = AddVectoredExceptionHandler(cCallFirst, OnException);
    assert(s_pVectoredHandler != NULL);
}

void Debug::OnDetach() noexcept
{
    if (s_pVectoredHandler)
    {
        RemoveVectoredExceptionHandler(s_pVectoredHandler);
    }
}

