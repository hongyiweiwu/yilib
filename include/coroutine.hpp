#pragma once

#include "compare.hpp"
#include "type_traits.hpp"
#include "cstddef.hpp"
#include "util/macros.hpp"

namespace std {
    /* 17.12.3 Coroutine traits */
    namespace __internal {
        template<class R, class Void, class ...ArgTypes> requires is_void_v<Void>
        struct __coroutine_traits {};
        template<class R, class ...ArgTypes> struct __coroutine_traits<R, void_t<typename R::promise_type>, ArgTypes...> {
            using promise_type = typename R::promises_type;
        };
    }

    template<class R, class ...ArgTypes> struct coroutine_traits : __internal::__coroutine_traits<R, void, ArgTypes...> {};

    // Forward declaration
    template<class Promise = void>
    struct coroutine_handle;

    /* 17.12.4 Class template coroutine_handle */
    template<>
    struct coroutine_handle<void> {
    protected:
        constexpr coroutine_handle(void* ptr) noexcept : ptr(ptr) {}
    public:
        constexpr coroutine_handle() noexcept : ptr(nullptr) {}
        constexpr coroutine_handle(nullptr_t) noexcept : ptr(nullptr) {}
        coroutine_handle& operator=(nullptr_t) noexcept;

        constexpr void* address() const noexcept { return ptr; }
        static constexpr coroutine_handle from_address(void* addr) {
            return coroutine_handle(addr);
        }

        constexpr explicit operator bool() const noexcept {
            return ptr != nullptr;
        }

#if __has_intrinsics_for(builtin_coro_done)
        bool done() const;
#endif

#if __has_intrinsics_for(builtin_coro_resume)
        void operator()() const;
        void resume() const;
#endif
#if __has_intrinsics_for(builtin_coro_destroy)
        void destroy() const;
#endif
    protected:
        void* ptr;
    };

    template<class Promise>
    struct coroutine_handle : coroutine_handle<> {
        using coroutine_handle<>::coroutine_handle;
#if __has_intrinsics_for(builtin_coro_promise)
        static coroutine_handle from_promise(Promise& p) {
            return coroutine_handle<Promise>(__builtin_coro_promise(&p, alignof(Promise), true));
        }
#endif
        coroutine_handle& operator=(nullptr_t) noexcept {
            return ptr != nullptr;
        }

        static constexpr coroutine_handle from_address(void* addr) {
            return coroutine_handle<Promise>(addr);
        }

#if __has_intrinsics_for(builtin_coro_promise)
        Promise& promise() const {
            return *reinterpret_cast<Promise*>(__builtin_coro_promise(ptr, alignof(Promise), false));
        }
#endif
    };

    /* 17.12.4.7 Comparison operators */
    constexpr bool operator==(coroutine_handle<> x, coroutine_handle<> y) noexcept {
        return x.address() == y.address();
    }

    constexpr strong_ordering operator<=>(coroutine_handle<> x, coroutine_handle<> y) noexcept {
        return compare_three_way()(x.address(), y.address());
    }

    /* 17.12.4.8 Hash support */
    // TODO Implement hash support.

    /* 17.12.5 No-op coroutines */ 
    struct noop_coroutine_promise {};
    constexpr std::size_t alignment = alignof(noop_coroutine_promise);
    template<> struct coroutine_handle<noop_coroutine_promise> : coroutine_handle<> {
        friend coroutine_handle<noop_coroutine_promise> noop_coroutine() noexcept;

        constexpr explicit operator bool() const noexcept { return true; }
        constexpr bool done() const noexcept { return false; }

        constexpr void operator()() const noexcept {}
        constexpr void resume() const noexcept {}
        constexpr void destroy() const noexcept {}

#if __has_intrinsics_for(builtin_coro_promise)
        noop_coroutine_promise& promise() const noexcept;
#endif

        constexpr void* address() const noexcept { return ptr; }
    private:
#if __has_intrinsics_for(builtin_coro_noop)
        coroutine_handle() noexcept;
#endif
    };

    using noop_coroutine_handle = coroutine_handle<noop_coroutine_promise>;

    noop_coroutine_handle noop_coroutine() noexcept;

    /* 17.12.6 Trivial awaitables */
    struct suspend_never {
        constexpr bool await_ready() const noexcept { return true; }
        constexpr void await_suspend(coroutine_handle<>) const noexcept {}
        constexpr void await_resume() const noexcept {}
    };

    struct suspend_always {
        constexpr bool await_ready() const noexcept { return false; }
        constexpr void await_suspend(coroutine_handle<>) const noexcept {}
        constexpr void await_resume() const noexcept {}
    };
}