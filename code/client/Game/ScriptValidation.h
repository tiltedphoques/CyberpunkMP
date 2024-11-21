#pragma once


namespace Game
{
enum class EScriptDefinition : uint8_t
{
    Type = 0,
    Class,
    Constant, // Values for Enums and Bitfields
    Enum,
    Bitfield,
    Function,
    Parameter,
    LocalVar,
    Property,
    SourceFile,
};

enum class EScriptVisibility : uint8_t
{
    Public = 0,
    Protected,
    Private,
};

struct ScriptDefinition
{
    virtual Red::Memory::IAllocator* GetAllocator() = 0; // 00
    virtual ~ScriptDefinition() = 0;                     // 08
    virtual EScriptDefinition GetType() = 0;             // 10
    virtual EScriptVisibility GetVisibility() = 0;       // 18
    virtual ScriptDefinition* GetParent() = 0;           // 20
    virtual Red::CName GetName() = 0;                    // 28
    virtual bool IsNative() = 0;                         // 30

    Red::CName name; // 08
    uint64_t unk10;  // 10
};

enum class EScriptType : uint8_t
{
    Primitive = 0,
    Class, // Class/Enum/Bitfield
    StrongHandle,
    WeakHandle,
    DynArray,
    StaticArray,
    ScriptRef,
};

struct ScriptType : ScriptDefinition
{
    Red::CBaseRTTIType* rttiType; // 18
    ScriptDefinition* innerType;  // 20
    int32_t arraySize;            // 28
    EScriptType metaType;         // 2C
};

struct ScriptProperty : ScriptDefinition
{
    struct Flags
    {
        uint16_t isNative : 1;     // 00
        uint16_t b01 : 1;          // 01
        uint16_t b02 : 1;          // 02
        uint16_t isConst : 1;      // 03
        uint16_t b04 : 1;          // 04
        uint16_t b05 : 1;          // 05
        uint16_t isPersistent : 1; // 06
        uint16_t b07 : 1;          // 07
        uint16_t b08 : 1;          // 08
    };

    Red::CProperty* rttiProp;                          // 18
    ScriptDefinition* parent;                          // 20
    Flags flags;                                       // 28
    Map<Red::CString, Red::CString> defaults;          // 30
    EScriptVisibility visibility;                      // 58
    Red::HashMap<Red::CName, Red::CString> attributes; // 60
    ScriptType* type;                                  // 90
};

struct ScriptClass;
struct ScriptFunction;

struct ScriptParameter : ScriptDefinition
{
    struct Flags
    {
        uint8_t isOptional : 1;     // 00
        uint8_t isOut : 1;          // 01
        uint8_t isShortCircuit : 1; // 02 - Got from redscript
        uint8_t isConst : 1;        // 03
    };

    Red::CProperty* rttiProp; // 18
    ScriptFunction* function; // 20
    ScriptType* type;         // 28
    Flags flags;              // 30
};

struct ScriptLocalVar : ScriptDefinition
{
    Red::CProperty* rttiProp; // 18
    ScriptFunction* function; // 20
    ScriptType* type;         // 28
    uint8_t unk30;            // 30
};

struct ScriptFileInfo : ScriptDefinition
{
    uint32_t crc;      // 18
    uint32_t index;    // 1C
    uint32_t hash;     // 20 - Murmur3 of path
    Red::CString path; // 28
};

struct ScriptFunction : ScriptDefinition
{
    struct Flags
    {
        uint32_t isStatic : 1;     // 00
        uint32_t isExec : 1;       // 01
        uint32_t isTimer : 1;      // 02
        uint32_t isFinal : 1;      // 03
        uint32_t isNative : 1;     // 04
        uint32_t b05 : 1;          // 05
        uint32_t b06 : 1;          // 06
        uint32_t isEvent : 1;      // 07
        uint32_t b08 : 1;          // 08
        uint32_t b09 : 1;          // 09
        uint32_t b10 : 1;          // 0A
        uint32_t b11 : 1;          // 0B
        uint32_t b12 : 1;          // 0C
        uint32_t b13 : 1;          // 0D
        uint32_t b14 : 1;          // 0E
        uint32_t isConst : 1;      // 0F
        uint32_t isThreadsafe : 1; // 10
        uint32_t isScripted : 1;   // 11 - Seems pointless, for all functions isScripted != isNative
    };

    Red::CBaseFunction* rttiFunc;                      // 18
    ScriptClass* parent;                               // 20
    Red::CName shortName;                              // 28
    Red::DynArray<ScriptParameter*> params;            // 30
    Red::DynArray<ScriptLocalVar*> localVars;          // 40
    Red::HashMap<Red::CName, Red::CString> attributes; // 50
    ScriptType* returnType;                            // 80
    uint16_t unk88;                                    // 88
    ScriptFunction* overridden;                        // 90
    Flags flags;                                       // 98
    Red::CName operatorName;                           // A0
    uint8_t castValue;                                 // A8
    EScriptVisibility visibility;                      // A9
    Red::DynArray<uint8_t> code;                       // B0
    ScriptFileInfo* sourceFile;                        // C0
    uint32_t sourceLine;                               // C8
};
RED4EXT_ASSERT_SIZE(ScriptFunction, 0xD0);

struct ScriptClass : ScriptDefinition
{
    struct Flags
    {
        uint8_t isNative : 1;     // 00
        uint8_t isImportOnly : 1; // 01
        uint8_t isAbstract : 1;   // 02
        uint8_t isFinal : 1;      // 03
        uint8_t isStruct : 1;     // 04
    };

    Red::CClass* rttiClass;                            // 18
    ScriptClass* parent;                               // 20
    Red::DynArray<ScriptProperty*> properties;         // 28
    Red::DynArray<ScriptProperty*> overrides;          // 38
    Red::DynArray<ScriptFunction*> functions;          // 48
    Red::HashMap<Red::CName, Red::CString> attributes; // 58
    EScriptVisibility visibility;                      // 88
    Flags flags;                                       // 89
};

struct ScriptBundle
{
    ScriptBundle();
    ~ScriptBundle();

    Red::HashMap<Red::CName, ScriptDefinition*> definitionsByName; // 00
    Red::HashMap<Red::CName, ScriptFunction*> functionsByName;     // 30
    Red::HashMap<Red::CName, ScriptType*> typesByName;             // 60
    Red::DynArray<ScriptFileInfo*> files;                          // 90
    Red::DynArray<ScriptFunction*> globals;                        // A0
    Red::DynArray<void*> enums;                                    // B0
    Red::DynArray<void*> bitfields;                                // C0
    Red::DynArray<ScriptClass*> classes;                           // D0
    Red::DynArray<ScriptType*> types;                              // E0
    Red::DynArray<Red::CString> strings;                           // F0
    Red::HashMap<Red::CName, void*> unk100;                        // 100
    Red::SharedSpinLock typesLock;                                 // 130
    CRITICAL_SECTION unk138;                                       // 138
};
RED4EXT_ASSERT_SIZE(ScriptBundle, 0x160);

struct StringView
{
    const char* str;
    uint32_t length;
};

struct ScriptReport
{
    virtual ~ScriptReport() = default;
    virtual void AddValidationError(StringView&) = 0;

    bool fillErrors;
};
} // namespace Game
