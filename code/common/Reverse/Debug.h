#pragma once

struct Debug
{
    static void WaitForDebugger() noexcept;
    static void CreateConsole() noexcept;
    static void OnAttach() noexcept;
    static void OnDetach() noexcept;
};

