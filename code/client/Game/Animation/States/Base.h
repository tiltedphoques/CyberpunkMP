#pragma once

struct MultiMovementController;

namespace States
{
struct Base
{
    static inline float kWalkSpeed = 3.f;
    static inline float kRunSpeed = 5.f;

    struct Update
    {
        float Delta;
        float Speed;
    };

    struct Jump
    {
    };

    struct Move
    {
    };

    struct Idle
    {
    };

    struct Transition
    {
        UniquePtr<Base> State;
    };

    Base(MultiMovementController& aParent)
        : m_parent(aParent)
    {
    }

    virtual ~Base(){}
    virtual void Enter() noexcept {}
    virtual void Exit() noexcept {}

    virtual void GetAnimationData(AnimationData& aData) const;

    [[nodiscard]] virtual std::optional<Transition> Process(const Update& acEvent) noexcept;
    [[nodiscard]] virtual std::optional<Transition> Process(const Jump& acEvent) noexcept { return std::nullopt; }
    [[nodiscard]] virtual std::optional<Transition> Process(const Move& acEvent) noexcept { return std::nullopt; }
    [[nodiscard]] virtual std::optional<Transition> Process(const Idle& acEvent) noexcept { return std::nullopt; }

    template <class T> std::optional<Transition> Transit() noexcept
    {
        Exit();
        auto pNewState = MakeUnique<T>(m_parent);
        pNewState->Enter();

        return Transition{std::move(pNewState)};
    }

protected:
    MultiMovementController& m_parent;
};
}

