#pragma once

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
}