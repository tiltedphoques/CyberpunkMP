#pragma once

#include <utility>


{
    template <class Result, class Error>
    struct Outcome
    {
        Outcome() noexcept
            : m_failed{ true }
        {}

        Outcome(const Result& aResult) noexcept
            : m_failed{ false }
            , m_result{ aResult }
        {}

        Outcome(const Error& aError) noexcept
            : m_failed{ true }
            , m_error(aError)
        {}

        Outcome(Result&& aResult) noexcept
            : m_failed{ false }
            , m_result{ std::forward<Result>(aResult) }
        {}

        Outcome(Error&& aError) noexcept
            : m_failed{ true }
            , m_error{ std::forward<Error>(aError) }
        {}

        Outcome(const Outcome& aOutcome) noexcept
        {
            *this = aOutcome;
        }

        Outcome(Outcome&& aOutcome) noexcept
        {
            *this = std::move(aOutcome);
        }

        Outcome& operator=(const Outcome& aOutcome) noexcept
        {
            if (&aOutcome != this)
            {
                m_failed = aOutcome.m_failed;
                m_error = aOutcome.m_error;
                m_result = aOutcome.m_result;
            }

            return *this;
        }

        Outcome& operator=(Outcome&& aOutcome) noexcept
        {
            if (&aOutcome != this)
            {
                m_failed = std::move(aOutcome.m_failed);
                m_error = std::move(aOutcome.m_error);
                m_result = std::move(aOutcome.m_result);
            }

            return *this;
        }

        [[nodiscard]] operator bool() const noexcept
        {
            return !HasError();
        }

        [[nodiscard]] bool HasError() const noexcept
        {
            return m_failed;
        }

        [[nodiscard]] const Error& GetError() const noexcept
        {
            return m_error;
        }

        [[nodiscard]] const Result& GetResult() const noexcept
        {
            return m_result;
        }

        [[nodiscard]] Result& GetResult() noexcept
        {
            return m_result;
        }

        [[nodiscard]] Result&& MoveResult() noexcept
        {
            return std::move(m_result);
        }

    private:

        bool m_failed;
        Result m_result;
        Error m_error;
    };
}
