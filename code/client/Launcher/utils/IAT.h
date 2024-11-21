#pragma once

#include <Windows.h>

template <typename T> inline T* getRVA(uintptr_t rva)
{
    return (T*)(0x140000000 + rva);
}

template <typename TOrdinal> inline bool iat_matches_ordinal(uintptr_t* nameTableEntry, TOrdinal ordinal)
{
}

template <> inline bool iat_matches_ordinal(uintptr_t* nameTableEntry, int ordinal)
{
    if (IMAGE_SNAP_BY_ORDINAL(*nameTableEntry))
    {
        return IMAGE_ORDINAL(*nameTableEntry) == ordinal;
    }

    return false;
}

template <> inline bool iat_matches_ordinal(uintptr_t* nameTableEntry, const char* ordinal)
{
    if (!IMAGE_SNAP_BY_ORDINAL(*nameTableEntry))
    {
        auto import = getRVA<IMAGE_IMPORT_BY_NAME>(*nameTableEntry);

        return !_stricmp(import->Name, ordinal);
    }

    return false;
}

static ptrdiff_t baseAddressDifference;

template <typename T, typename TOrdinal> T iat(const char* moduleName, TOrdinal ordinal, T function, T& old)
{
#ifdef _M_IX86
    IMAGE_DOS_HEADER* imageHeader = (IMAGE_DOS_HEADER*)(baseAddressDifference + 0x400000);
#elif defined(_M_AMD64)
    IMAGE_DOS_HEADER* imageHeader = (IMAGE_DOS_HEADER*)(GetModuleHandleA(nullptr));
#endif
    IMAGE_NT_HEADERS* ntHeader = getRVA<IMAGE_NT_HEADERS>(imageHeader->e_lfanew);

    IMAGE_IMPORT_DESCRIPTOR* descriptor = getRVA<IMAGE_IMPORT_DESCRIPTOR>(ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    while (descriptor->Name)
    {
        const char* name = getRVA<char>(descriptor->Name);

        if (_stricmp(name, moduleName))
        {
            descriptor++;

            continue;
        }

        if (descriptor->OriginalFirstThunk == 0)
        {
            return nullptr;
        }

        auto nameTableEntry = getRVA<uintptr_t>(descriptor->OriginalFirstThunk);
        auto addressTableEntry = getRVA<uintptr_t>(descriptor->FirstThunk);

        while (*nameTableEntry)
        {
            if (iat_matches_ordinal(nameTableEntry, ordinal))
            {
                T origEntry = (T)*addressTableEntry;

                DWORD oldProtect;
                VirtualProtect(addressTableEntry, sizeof(T), PAGE_READWRITE, &oldProtect);

                old = (T&)*addressTableEntry;
                *addressTableEntry = (uintptr_t)function;

                VirtualProtect(addressTableEntry, sizeof(T), oldProtect, &oldProtect);

                return origEntry;
            }

            nameTableEntry++;
            addressTableEntry++;
        }

        return nullptr;
    }

    return nullptr;
}

inline void set_base(uintptr_t address)
{
#ifdef _M_IX86
    uintptr_t addressDiff = (address - 0x400000);
#elif defined(_M_AMD64)
    uintptr_t addressDiff = (address - 0x140000000);
#endif

    // pointer-style cast to ensure unsigned overflow ends up copied directly into a signed value
    baseAddressDifference = *(ptrdiff_t*)&addressDiff;
}

// sets the base to the process main base
inline void set_base()
{
    set_base((uintptr_t)GetModuleHandle(NULL));
}
