#pragma once

#include "util/priority_tag.hpp"
#include "type_traits.hpp"

namespace std {
    /* 20.10.3 Pointer traits */
    namespace __internal {
        /*
        template<class Ptr> struct __pointer_element_type {};
        template<class Ptr> requires requires { typename Ptr::element_type; }
        struct __pointer_element_type<Ptr> { using type = typename Ptr::element_type; };
        template<template<class, class...> class SomePointer, class T, class ...Args> requires (!requires { typename SomePointer<T, Args...>::element_type; })
        struct __pointer_element_type<SomePointer<T, Args...>> { using type = T; }; */
        template<class Ptr> requires requires { typename Ptr::element_type; }
        auto __pointer_element_type(priority_tag<2>) -> typename Ptr::element_type;

        
    }

    template<class Ptr> struct pointer_traits {
        using pointer = Ptr;
        using element_type = typename __internal::__pointer_element_type<Ptr>::type;  
    };

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
}