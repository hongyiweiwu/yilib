#pragma once

#include "exception.hpp"
#include "type_traits.hpp"
#include "cstddef.hpp"
#include "util/macros.hpp"

namespace yilib {
    /* 17.6.4 Storage allocation errors */
    class bad_alloc : public exception {
    public:
        const char* what() const noexcept override;
    };

    class bad_array_new_length : public exception {
    public:
        const char* what() const noexcept override;
    };

    struct destroying_delete_t {
        explicit destroying_delete_t() = default;
    };
    inline constexpr destroying_delete_t destroying_delete{};

    enum class align_val_t : size_t {};

    struct nothrow_t { explicit nothrow_t() = default; };
    extern const nothrow_t nothrow;

    using new_handler = void (*)();
    new_handler get_new_handler() noexcept;
    new_handler set_new_handler(new_handler new_p) noexcept;

    /* 17.6.5 Pointer optimization barrier */
#if __has_intrinsics_for(builtin_launder)
    template<class T> [[nodiscard]] constexpr T* launder(T* p) noexcept {
        static_assert(!is_function_v<T> && !is_void_v<T>, "T cannot be a function type or void.");
        return __builtin_launder(p);
    }
#endif

    /* 17.6.6 Hardware interference size */
#if __has_intrinsics_for(hardware_destructive_interference_size)
    inline constexpr size_t hardware_destructive_interference_size = __hardware_destructive_interference_size();
#endif

#if __has_intrinsics_for(hardware_constructive_interference_size)
    inline constexpr size_t hardware_constructive_interference_size = __hardware_constructive_interference_size();
#endif
}

/* 17.6.3 Storage allocation and deallocation */
[[nodiscard]] void* operator new(yilib::size_t size);
[[nodiscard]] void* operator new(yilib::size_t size, yilib::align_val_t alignment);
[[nodiscard]] void* operator new(yilib::size_t size, const yilib::nothrow_t&) noexcept;
[[nodiscard]] void* operator new(yilib::size_t size, yilib::align_val_t alignment, const yilib::nothrow_t&) noexcept;

void operator delete(void* ptr) noexcept;
void operator delete(void* ptr, yilib::size_t size) noexcept;
void operator delete(void* ptr, yilib::align_val_t alignment) noexcept;
void operator delete(void* ptr, yilib::size_t size, yilib::align_val_t alignment) noexcept;
void operator delete(void* ptr, const yilib::nothrow_t&) noexcept;
void operator delete(void* ptr, yilib::align_val_t alignment, const yilib::nothrow_t&) noexcept;

[[nodiscard]] void* operator new[](yilib::size_t size);
[[nodiscard]] void* operator new[](yilib::size_t size, yilib::align_val_t alignment);
[[nodiscard]] void* operator new[](yilib::size_t size, const yilib::nothrow_t&) noexcept;
[[nodiscard]] void* operator new[](yilib::size_t size, yilib::align_val_t alignment, const yilib::nothrow_t&) noexcept;

void operator delete[](void* ptr) noexcept;
void operator delete[](void* ptr, yilib::size_t size) noexcept;
void operator delete[](void* ptr, yilib::align_val_t alignment) noexcept;
void operator delete[](void* ptr, yilib::size_t size, yilib::align_val_t alignment) noexcept;
void operator delete[](void* ptr, const yilib::nothrow_t&) noexcept;
void operator delete[](void* ptr, yilib::align_val_t alignment, const yilib::nothrow_t&) noexcept;

[[nodiscard]] void* operator new(yilib::size_t size, void* ptr) noexcept;
[[nodiscard]] void* operator new[](yilib::size_t size, void* ptr) noexcept;
void operator delete(void* ptr, void*) noexcept;
void operator delete[](void* ptr, void*) noexcept;