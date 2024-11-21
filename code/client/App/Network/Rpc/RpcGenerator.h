#pragma once

struct BaseArgumentPack;

namespace Game
{
struct ScriptFunction;
struct ScriptClass;
}

struct RpcMock
{
};

struct RpcContext
{
    struct Arg
    {
        UniquePtr<BaseArgumentPack> pack;
    };

    Vector<Arg> args;
    Red::CName klass;
    Red::CName function;
};

struct RpcHandler
{
    RpcContext context;
    Red::CClassFunction* function;
};

struct RpcGenerator
{
    using Descriptor = Red::ClassDescriptorDefaultImpl<RpcMock>;

    static void GenerateHandlers();
    static void DumpCsharp();
    static void CreateFunction(Descriptor* desc, const Game::ScriptClass* klass, Game::ScriptFunction* func);
    static const Vector<Red::CClassStaticFunction*>& GetFunctions() { return s_functions; }
    static RpcHandler* GetRpcHandler(uint64_t klass, uint64_t func);

private:

    static inline Vector<Red::CClassStaticFunction*> s_functions;
};

