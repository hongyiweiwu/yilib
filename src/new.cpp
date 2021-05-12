#include "new.hpp"
#include "cstddef.hpp"
#include "cstdlib.hpp"

#include <stdatomic.h>

namespace yilib {
    const char* bad_alloc::what() const noexcept { return "yilib::bad_alloc"; }

    const char* bad_array_new_length::what() const noexcept { return "yilib::bad_array_new_length"; }

    const nothrow_t nothrow{};

    static constinit _Atomic(new_handler) __curr_new_handler;

    new_handler get_new_handler() noexcept {
        return atomic_load(&__curr_new_handler);
    }

    new_handler set_new_handler(new_handler new_p) noexcept {
        return atomic_exchange(&__curr_new_handler, new_p);
    }
}

[[nodiscard]] void* operator new(yilib::size_t size) {
    // Here, we assume that malloc always uses the maximum alignment needed in the system. We can then
    // defer the call to the new operator that requires alignment.
    return operator new(size, yilib::align_val_t(__STDCPP_DEFAULT_NEW_ALIGNMENT__));
}

[[nodiscard]] void* operator new(yilib::size_t size, yilib::align_val_t alignment) {
    if (static_cast<yilib::size_t>(alignment) < __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
        alignment = yilib::align_val_t(__STDCPP_DEFAULT_NEW_ALIGNMENT__);
    }
    void* ptr = nullptr;
    while ((ptr = yilib::aligned_alloc(static_cast<yilib::size_t>(alignment), size)) == nullptr) {
        auto handler = yilib::get_new_handler();
        if (handler == nullptr) {
            throw yilib::bad_alloc();
        } else {
            (*handler)();
        }
    }

    return ptr;
}

[[nodiscard]] void* operator new(yilib::size_t size, const yilib::nothrow_t&) noexcept {
    try {
        return operator new(size);
    } catch (const yilib::bad_alloc&) {
        return nullptr;
    }
}

[[nodiscard]] void* operator new(yilib::size_t size, yilib::align_val_t alignment, const yilib::nothrow_t&) noexcept {
    try {
        return operator new(size, alignment);
    } catch (const yilib::bad_alloc&) {
        return nullptr;
    }
}

void operator delete(void* ptr) noexcept {
    yilib::free(ptr);
}

void operator delete(void* ptr, yilib::size_t size) noexcept {
    operator delete(ptr);
}

void operator delete(void* ptr, yilib::align_val_t alignment) noexcept {
    operator delete(ptr);
}

void operator delete(void* ptr, yilib::size_t size, yilib::align_val_t alignment) noexcept {
    operator delete(ptr, alignment);
}

void operator delete(void* ptr, const yilib::nothrow_t&) noexcept {
    operator delete(ptr);
}

void operator delete(void* ptr, yilib::align_val_t alignment, const yilib::nothrow_t&) noexcept {
    operator delete(ptr, alignment);
}

[[nodiscard]] void* operator new[](yilib::size_t size) {
    return operator new(size);
}
[[nodiscard]] void* operator new[](yilib::size_t size, yilib::align_val_t alignment) {
    return operator new(size, alignment);
}
[[nodiscard]] void* operator new[](yilib::size_t size, const yilib::nothrow_t&) noexcept {
    return operator new(size, yilib::nothrow);
}
[[nodiscard]] void* operator new[](yilib::size_t size, yilib::align_val_t alignment, const yilib::nothrow_t&) noexcept {
    return operator new(size, alignment, yilib::nothrow);
}

void operator delete[](void* ptr) noexcept {
    operator delete(ptr);
}
void operator delete[](void* ptr, yilib::size_t size) noexcept {
    operator delete(ptr, size);
}
void operator delete[](void* ptr, yilib::align_val_t alignment) noexcept {
    operator delete(ptr, alignment);
}
void operator delete[](void* ptr, yilib::size_t size, yilib::align_val_t alignment) noexcept {
    operator delete(ptr, size, alignment);
}
void operator delete[](void* ptr, const yilib::nothrow_t&) noexcept {
    operator delete(ptr, yilib::nothrow);
}
void operator delete[](void* ptr, yilib::align_val_t alignment, const yilib::nothrow_t&) noexcept {
    operator delete(ptr, alignment, yilib::nothrow);
}

[[nodiscard]] void* operator new(yilib::size_t size, void* ptr) noexcept {
    return ptr;
}
[[nodiscard]] void* operator new[](yilib::size_t size, void* ptr) noexcept {
    return ptr;
}
void operator delete(void* ptr, void*) noexcept {}
void operator delete[](void* ptr, void*) noexcept {}
