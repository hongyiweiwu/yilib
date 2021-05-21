#include "new.hpp"
#include "cstddef.hpp"
#include "cstdlib.hpp"

// TODO: Replace with "cmath.hpp" once it's implemented.
#include "math.h"

#include <stdatomic.h>

namespace std {
    const char* bad_alloc::what() const noexcept { return "std::bad_alloc"; }

    const char* bad_array_new_length::what() const noexcept { return "std::bad_array_new_length"; }

    const nothrow_t nothrow{};

    static constinit _Atomic(new_handler) __curr_new_handler;

    new_handler get_new_handler() noexcept {
        return atomic_load(&__curr_new_handler);
    }

    new_handler set_new_handler(new_handler new_p) noexcept {
        return atomic_exchange(&__curr_new_handler, new_p);
    }
}

[[nodiscard]] void* operator new(std::size_t size) {
    // Here, we assume that malloc always uses the maximum alignment needed in the system. We can then
    // defer the call to the new operator that requires alignment.
    return operator new(size, std::align_val_t(__STDCPP_DEFAULT_NEW_ALIGNMENT__));
}

[[nodiscard]] void* operator new(std::size_t size, std::align_val_t alignment) {
    if (static_cast<std::size_t>(alignment) < __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
        alignment = std::align_val_t(__STDCPP_DEFAULT_NEW_ALIGNMENT__);
    }

    if (size % static_cast<size_t>(alignment) != 0) {
        size = ceil(double(size) / static_cast<size_t>(alignment)) * static_cast<size_t>(alignment);
    }

    void* ptr = nullptr;
    while ((ptr = std::aligned_alloc(static_cast<std::size_t>(alignment), size)) == nullptr) {
        auto handler = std::get_new_handler();
        if (handler == nullptr) {
            throw std::bad_alloc();
        } else {
            (*handler)();
        }
    }

    return ptr;
}

[[nodiscard]] void* operator new(std::size_t size, const std::nothrow_t&) noexcept {
    try {
        return operator new(size);
    } catch (const std::bad_alloc&) {
        return nullptr;
    }
}

[[nodiscard]] void* operator new(std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept {
    try {
        return operator new(size, alignment);
    } catch (const std::bad_alloc&) {
        return nullptr;
    }
}

void operator delete(void* ptr) noexcept {
    std::free(ptr);
}

void operator delete(void* ptr, std::size_t size) noexcept {
    operator delete(ptr);
}

void operator delete(void* ptr, std::align_val_t alignment) noexcept {
    operator delete(ptr);
}

void operator delete(void* ptr, std::size_t size, std::align_val_t alignment) noexcept {
    operator delete(ptr, alignment);
}

void operator delete(void* ptr, const std::nothrow_t&) noexcept {
    operator delete(ptr);
}

void operator delete(void* ptr, std::align_val_t alignment, const std::nothrow_t&) noexcept {
    operator delete(ptr, alignment);
}

[[nodiscard]] void* operator new[](std::size_t size) {
    return operator new(size);
}
[[nodiscard]] void* operator new[](std::size_t size, std::align_val_t alignment) {
    return operator new(size, alignment);
}
[[nodiscard]] void* operator new[](std::size_t size, const std::nothrow_t&) noexcept {
    return operator new(size, std::nothrow);
}
[[nodiscard]] void* operator new[](std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept {
    return operator new(size, alignment, std::nothrow);
}

void operator delete[](void* ptr) noexcept {
    operator delete(ptr);
}
void operator delete[](void* ptr, std::size_t size) noexcept {
    operator delete(ptr, size);
}
void operator delete[](void* ptr, std::align_val_t alignment) noexcept {
    operator delete(ptr, alignment);
}
void operator delete[](void* ptr, std::size_t size, std::align_val_t alignment) noexcept {
    operator delete(ptr, size, alignment);
}
void operator delete[](void* ptr, const std::nothrow_t&) noexcept {
    operator delete(ptr, std::nothrow);
}
void operator delete[](void* ptr, std::align_val_t alignment, const std::nothrow_t&) noexcept {
    operator delete(ptr, alignment, std::nothrow);
}

[[nodiscard]] void* operator new(std::size_t size, void* ptr) noexcept {
    return ptr;
}
[[nodiscard]] void* operator new[](std::size_t size, void* ptr) noexcept {
    return ptr;
}
void operator delete(void* ptr, void*) noexcept {}
void operator delete[](void* ptr, void*) noexcept {}
