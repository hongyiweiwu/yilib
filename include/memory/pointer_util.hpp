#pragma once

#include "cstddef.hpp"
#include "type_traits.hpp"

namespace std {
    /* 20.10.11 addressof */
    template<class T> constexpr T* addressof(T& r) noexcept {
        if constexpr (is_object_v<T>) {
            // And we cast back to the desired T* type.
            return reinterpret_cast<T*>(
                // And then we remove the cv-qualifiers, and take the address -- we know char doesn't have an overload & operator.
                &const_cast<char&>(
                    // First we cast r to the most permissible (cv-qualified) char type.
                    reinterpret_cast<const volatile char&>(r)
                )
            );
        } else {
            // For non-object types, overloading & is impossible so we can safely use it.
            return &r;
        }
    }

    template<class T> const T* addressof(const T&&) = delete;

    /* 20.10.3 Pointer traits */
    namespace __internal {
        template<class Ptr> struct __pointer_element_type {};
        template<class Ptr> requires requires { typename Ptr::element_type; }
        struct __pointer_element_type<Ptr> { using type = typename Ptr::element_type; };
        template<template<class, class...> class SomePointer, class T, class ...Args> requires (!requires { typename SomePointer<T, Args...>::element_type; })
        struct __pointer_element_type<SomePointer<T, Args...>> { using type = T; };

        template<class Ptr> auto __pointer_element_difference_type() -> ptrdiff_t;
        template<class Ptr> requires requires { typename Ptr::difference_type; }
        auto __pointer_element_difference_type() -> typename Ptr::difference_type;

        template<class Ptr, class U> struct __pointer_rebind_type {};
        template<class Ptr, class U> requires requires { typename Ptr::template rebind<U>; }
        struct __pointer_rebind_type<Ptr, U> { using type = typename Ptr::template rebind<U>; };
        template<template<class, class...> class SomePointer, class U, class T, class ...Args> requires (!requires { typename SomePointer<T, Args...>::template rebind<U>; })
        struct __pointer_rebind_type<SomePointer<T, Args...>, U> { using type = SomePointer<U, Args...>; };
    }

    template<class Ptr> struct pointer_traits {
        using pointer = Ptr;
        using element_type = typename __internal::__pointer_element_type<Ptr>::type;
        using difference_type = decltype(__internal::__pointer_element_difference_type<Ptr>());

        template<class U> using rebind = typename __internal::__pointer_rebind_type<Ptr, U>::type;
    };

    template<class T> struct pointer_traits<T*> {
        using pointer = T*;
        using element_type = T;
        using difference_type = ptrdiff_t;

        template<class U> using rebind = U*;

        static constexpr pointer pointer_to(conditional_t<is_void_v<element_type>, char&, element_type&> r) noexcept {
            return addressof(r);
        }
    };

    /* 20.10.4 Pointer conversion */
    template<class T> requires (!is_function_v<T>)
    constexpr T* to_address(T* p) noexcept { return p; }

    template<class Ptr> auto to_address(const Ptr& p) noexcept {
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
    template<class T> T* undeclare_reachable(T* p) { return p; }
    void declare_no_pointers(char* p, size_t n);
    void undeclare_no_pointers(char* p, size_t n);
    pointer_safety get_pointer_safety() noexcept;

    /* 20.10.6 Pointer alignment */
    void* align(size_t alignment, size_t size, void*& ptr, size_t& space);

    template<size_t N, class T> requires (N != 0) && ((N & (N - 1)) == 0) // N is a power of two.
    [[nodiscard]] constexpr T* assume_aligned(T* ptr) { return ptr; }
}