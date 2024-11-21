namespace Game::Rendering
{
struct Context
{
    struct Device
    {
        IDXGISwapChain4* pSwapChain;
        uint8_t pad8[0xB0 - 0x8];
    };

    Context() = delete;
    ~Context() = delete;

    static Context* GetInstance() noexcept
    {
        static Red::UniversalRelocPtr<uint8_t> s_ptr(2468877568UL);
        static uintptr_t callAddress = (uintptr_t)(s_ptr.GetAddr() + 0x1F);
        static const auto offset = *reinterpret_cast<uint32_t*>(callAddress);
        static const auto fixedAddress = callAddress + offset + 4;
        return *(Context**)fixedAddress;
    }

    uint8_t pad0[0xC97F38];
    Device devices[0x30]; // Count unknown, it is at least 0x20
    uint8_t pad[0x13BC4D0 - (0xC97F38 + sizeof(devices))];
    ID3D12CommandQueue* pDirectQueue; // 0x13BC4D0
};

static_assert(sizeof(Context::Device) == 0xB0);
static_assert(offsetof(Context, devices) == 0xC97F38);
static_assert(offsetof(Context, pDirectQueue) == 0x13BC4D0);

constexpr auto CRenderNode_Present_DoInternal = Core::RawFunc<
    /* hash = */ 2468877568UL,
    /* type = */ void* (*)(int32_t*, uint8_t, UINT)>();

constexpr auto CRenderGlobal_Resize = Core::RawFunc<
    /* hash = */ 239671909UL,
    /* type = */ void* (*)(uint32_t a1, uint32_t a2, uint32_t a3, uint8_t a4, int32_t* a5)>();

constexpr auto CRenderGlobal_Shutdown = Core::RawFunc<
    /* hash = */ 3192982283UL,
    /* type = */ void* (*)(uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4)>();
}