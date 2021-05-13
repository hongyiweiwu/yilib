#include "coroutine.hpp"
#include "cstddef.hpp"

#include "util/macros.hpp"

namespace std {
    coroutine_handle<>& coroutine_handle<void>::operator=(nullptr_t) noexcept {
        ptr = nullptr;
        return *this;
    }

#if __has_intrinsics_for(builtin_coro_done)
    bool coroutine_handle<void>::done() const {
        return __builtin_coro_done(ptr);
    }
#endif

#if __has_intrinsics_for(builtin_coro_resume)
    void coroutine_handle<void>::operator()() const {
        resume();
    }

    void coroutine_handle<void>::resume() const {
        __builtin_coro_resume(ptr);
    }
#endif

#if __has_intrinsics_for(builtin_coro_destroy)
    void coroutine_handle<void>::destroy() const {
        __builtin_coro_destroy(ptr);
    }
#endif

#if __has_intrinsics_for(builtin_coro_promise)
    noop_coroutine_promise& coroutine_handle<noop_coroutine_promise>::promise() const noexcept {
        return *reinterpret_cast<noop_coroutine_promise*>(__builtin_coro_promise(ptr, 0, false));
    }
#endif

#if __has_intrinsics_for(builtin_coro_noop)
        coroutine_handle<noop_coroutine_promise>::coroutine_handle() noexcept
            : coroutine_handle<>(__builtin_coro_noop()) {}
#endif

    noop_coroutine_handle noop_coroutine() noexcept {
        return noop_coroutine_handle{};
    }
}