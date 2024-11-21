#pragma once

#ifndef REVERSE_ALLOC_STUB
#define REVERSE_ALLOC_STUB(x) malloc(x)
#endif

#include <xbyak/xbyak.h>


{
    // Wraps the generator in a core stub page.
    struct CodeGenerator : Xbyak::CodeGenerator
    {
        // Warning(Force): Ensure those are exponents of 2.
        static constexpr size_t kDefaultAllocStubsize = 64;

        CodeGenerator(const size_t aAllocSize = kDefaultAllocStubsize) :
            Xbyak::CodeGenerator(aAllocSize, REVERSE_ALLOC_STUB(aAllocSize)){}

        void jmp_S(const mem::pointer acEa) {
            Xbyak::CodeGenerator::jmp(acEa.as<const void*>());
        }
    };
}
