#pragma once

#include "cstddef.hpp"
#include "type_traits.hpp"

namespace std {
    /* 20.10.11 addressof */
    template<class T>
    constexpr T* addressof(T& r) noexcept {
        return __builtin_addressof(r);
    }

    template<class T>
    const T* addressof(const T&&) = delete;

    /* 20.10.3 Pointer traits */
    template<class Ptr>
    struct pointer_traits {
    private:
        template<class Pointer>
        struct get_element_type {};

        template<class Pointer>
        requires requires { typename Pointer::element_type; }
        struct get_element_type<Pointer> {
            using type = typename Pointer::element_type;
        };

        template<template<class, class...> class SomePointer, class T, class ...Args>
        requires (!requires { typename SomePointer<T, Args...>::element_type; })
        struct get_element_type<SomePointer<T, Args...>> {
            using type = T;
        };

        template<class Pointer>
        struct get_difference_type {};

        template<class Pointer>
        requires requires { typename Ptr::difference_type; }
        struct get_difference_type<Pointer> {
            using type = typename Ptr::difference_type;
        };

        template<class Pointer, class U>
        struct get_rebind_type {};

        template<class Pointer, class U>
        requires requires { typename Pointer::template rebind<U>; }
        struct get_rebind_type<Pointer, U> {
            using type = typename Pointer::template rebind<U>;
        };

        template<template<class, class...> class SomePointer, class U, class T, class ...Args>
        requires (!requires { typename SomePointer<T, Args...>::template rebind<U>; })
        struct get_rebind_type<SomePointer<T, Args...>, U> {
            using type = SomePointer<U, Args...>;
        };
    public:
        using pointer = Ptr;
        using element_type = typename get_element_type<Ptr>::type;
        using difference_type = typename get_difference_type<Ptr>::type;

        template<class U>
        using rebind = typename get_rebind_type<Ptr, U>::type;
    };

    template<class T>
    struct pointer_traits<T*> {
        using pointer = T*;
        using element_type = T;
        using difference_type = ptrdiff_t;

        template<class U>
        using rebind = U*;

        static constexpr pointer pointer_to(conditional_t<is_void_v<element_type>, char&, element_type&> r) noexcept {
            return addressof(r);
        }
    };

    /* 20.10.4 Pointer conversion */
    template<class T>
    requires (!is_function_v<T>)
    constexpr T* to_address(T* p) noexcept {
        return p;
    }

    template<class Ptr>
    auto to_address(const Ptr& p) noexcept {
        if constexpr (requires (const Ptr* p) { pointer_traits<Ptr>::to_address(p); }) {
            return pointer_traits<Ptr>::to_address(p);
        } else {
            return to_address(p.operator->());
        }
    }

    /* 20.10.5 Pointer safety */
    // NOTE: This section of the code hasn't been implemented. The empty declarations are included purely to satisfy
    // the standard. It's unlikely that it'll ever be implemented, and there's an active proposal (P2186) to remove
    // it from the standard.
    enum class pointer_safety { relaxed, preferred, strict };
    void declare_reachable(void* p);

    template<class T>
    T* undeclare_reachable(T* p) {
        return p;
    }

    void declare_no_pointers(char* p, std::size_t n);
    void undeclare_no_pointers(char* p, std::size_t n);
    pointer_safety get_pointer_safety() noexcept;

    /* 20.10.6 Pointer alignment */
    void* align(std::size_t alignment, std::size_t size, void*& ptr, std::size_t& space);

    template<std::size_t N, class T>
    requires (N != 0) && ((N & (N - 1)) == 0) // N is a power of two.
    [[nodiscard]] constexpr T* assume_aligned(T* ptr) {
        return ptr;
    }
}