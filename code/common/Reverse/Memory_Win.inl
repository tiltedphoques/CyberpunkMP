ScopedContext::ScopedContext(const mem::pointer aEa, const size_t aSize) noexcept
    : m_oldProtect(0)
    , m_ea(aEa)
    , m_size(aSize)
{
    VirtualProtect(aEa.as<void*>(), m_size, PAGE_EXECUTE_READWRITE, reinterpret_cast<DWORD*>(&m_oldProtect));
}

ScopedContext::~ScopedContext()
{
    VirtualProtect(m_ea.as<void*>(), m_size, m_oldProtect, reinterpret_cast<DWORD*>(&m_oldProtect));
}

bool ScopedContext::WriteBuffer(const uint8_t* acpData, const size_t aSize, const size_t aOffset) const noexcept
{
    if (aSize + aOffset > m_size)
        return false;

    auto memory = m_ea + aOffset;
    std::copy(acpData, acpData + aSize, memory.as<uint8_t*>());

    return true;
}