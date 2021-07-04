#pragma once

#include "cstddef.hpp"
#include "limits.hpp"
#include "condition_variable.hpp"
#include "mutex.hpp"

namespace std {
    class latch {
    public:
        static constexpr std::ptrdiff_t max() noexcept {
            return numeric_limits<std::ptrdiff_t>::max();
        }

        constexpr explicit latch(std::ptrdiff_t expected) : counter(expected) {}
        ~latch() = default;

        latch(const latch&) = delete;
        latch& operator=(const latch&) = delete;

        void count_down(std::ptrdiff_t update = 1);
        bool try_wait() const noexcept;
        void wait() const;
        void arrive_and_wait(std::ptrdiff_t update = 1);

    private:
        std::ptrdiff_t counter;
        mutable mutex mtx;
        mutable condition_variable condvar;
    };
}