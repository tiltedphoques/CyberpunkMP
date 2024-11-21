#include "BufferScriptInstance.h"
#include "IRpc.h"

struct RpcCallExtended : RpcCall
{
    RpcCallExtended(uint32_t id, uint64_t player)
        : RpcCall()
    {
        Args = &m_buffer;
        Id = id;
        PlayerId = player;
    }

    RpcCallExtended() = default;
    ~RpcCallExtended();
    RpcCallExtended(const RpcCallExtended&) = delete;
    RpcCallExtended& operator=(const RpcCallExtended&) = delete;

    RpcCallExtended(RpcCallExtended&&) noexcept = default;
    RpcCallExtended& operator=(RpcCallExtended&&) noexcept = default;

    BufferScriptInstance m_buffer;
};

struct RpcId
{
    uint64_t Klass;
    uint64_t Function;

    bool operator==(const RpcId& acRhs) const
    {
        return Klass == acRhs.Klass && Function == acRhs.Function;
    }
};

template <> struct std::hash<RpcId>
{
    std::size_t operator()(const RpcId& s) const noexcept { return s.Klass ^ (s.Function << 1); }
};

struct RpcScriptInstance : IRpc
{
    RpcScriptInstance();

    void Call(uint64_t Player, uint64_t Klass, uint64_t Function, IBuffer* Arguments) override;
    uint32_t Count() override;
    RpcCall* GetRpc(uint32_t aIndex) override;
    void Clear() override;
    uint32_t RegisterClient(uint64_t Klass, uint64_t Function) override;
    uint32_t RegisterServer(uint64_t Klass, uint64_t Function) override;
    void Serialize(server::RpcDefinitions& aDefinitions) const;

protected:

    void HandleRpcCall(const PacketEvent<client::RpcCall>& aMessage);

private:

    uint32_t GetId();

    struct RpcDefinition
    {
        uint32_t Id;
        uint64_t Klass;
        uint64_t Function;
    };

    std::atomic<uint32_t> m_id;
    Vector<RpcCallExtended> m_calls;
    Vector<RpcDefinition> m_clientRpcs;
    Vector<RpcDefinition> m_serverRpcs;
    Map<RpcId, uint32_t> m_clientIds;
};
