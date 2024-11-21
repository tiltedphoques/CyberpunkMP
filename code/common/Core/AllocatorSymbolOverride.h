
// these symbols need to live in the global namespace.

#include <memory>
#include <Allocator.hpp>


{
    inline void* TPAlloc(const size_t acSize)
    {
        return TiltedPhoques::Allocator::GetDefault()->Allocate(acSize);
    }

    inline void TPFree(void* apBlock)
    {
        TiltedPhoques::Allocator::GetDefault()->Free(apBlock);
    }
}

// NOTE(Force): this replaces the global c++ operators, note that this is transitive, e.g.
// it replaces the symbols for all projects.
#ifdef TPCORE_CXX_ALLOCATOR_OVERRIDE

void* operator new(size_t size)
{
    return TiltedPhoques::TPAlloc(size);
}

void operator delete(void* p) noexcept
{
    TiltedPhoques::TPFree(p);
}

void* operator new[](size_t size)
{
    return TiltedPhoques::TPAlloc(size);
}

void operator delete[](void* p) noexcept
{
    TiltedPhoques::TPFree(p);
}

void* operator new(size_t size, const std::nothrow_t&) noexcept
{
    return TiltedPhoques::TPAlloc(size);
}

void* operator new[](size_t size, const std::nothrow_t&) noexcept
{
    return TiltedPhoques::TPAlloc(size);
}

void operator delete(void* p, const std::nothrow_t&) noexcept
{
    TiltedPhoques::TPFree(p);
}

void operator delete[](void* p, const std::nothrow_t&) noexcept
{
    TiltedPhoques::TPFree(p);
}

void operator delete(void* p, size_t) noexcept
{
    TiltedPhoques::TPFree(p);
}

void operator delete[](void* p, size_t) noexcept
{
    TiltedPhoques::TPFree(p);
}

#endif
