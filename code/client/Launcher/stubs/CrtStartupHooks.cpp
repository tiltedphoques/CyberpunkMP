#include "Launcher/utils/IAT.h"

namespace
{
void (*Real_RaiseException)(DWORD, DWORD, DWORD, const ULONG_PTR*) = nullptr;
void TP_RaiseException(DWORD dwExceptionCode, DWORD dwExceptionFlags, DWORD nNumberOfArguments, const ULONG_PTR* lpArguments)
{
    if (dwExceptionCode == 0x406D1388 && !IsDebuggerPresent())
        return; // thread naming

    Real_RaiseException(dwExceptionCode, dwExceptionFlags, nNumberOfArguments, lpArguments);
}
} // namespace

void ApplyStartupHook()
{
    //iat("Kernel32.dll", "RaiseException", &TP_RaiseException, Real_RaiseException);
}
