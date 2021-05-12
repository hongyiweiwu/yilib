#pragma once

#include "cstddef.hpp"

#include "type_traits/base.hpp"
#include "type_traits/is_type.hpp"

namespace yilib::__internal {
    template<class T> struct alignment_of : integral_constant<size_t, alignof(T)> {};

    template<size_t Len> requires (Len > 0) && (Len <= alignof(max_align_t))
    consteval size_t __default_alignment() noexcept {
        size_t alignment = 1;
        while (Len >= alignment) {
            alignment *= 2;
        }

        return alignment / 2;
    }

    template<size_t Align> requires (Align >= 1) && (Align <= alignof(max_align_t))
    consteval bool __is_alignment_of_some_type() noexcept {
        for (size_t alignment = 1; alignment <= alignof(max_align_t); alignment *= 2) {
            if (alignment == Align) {
                return true;
            }
        }

        return false;
    }

    template<size_t Len, size_t Align = __default_alignment<Len>()> 
        requires (Len > 0) && (Align == __default_alignment<Len>() || __is_alignment_of_some_type<Align>())
    struct aligned_storage {
        struct type {
            alignas(Align) unsigned char data[Len];
        };
    };

    template<class T, T N> consteval T __max() noexcept {
        return N;
    }
    template<class T, T N1, T ...N> consteval T __max() noexcept {
        constexpr T remaining_max = __max<T, N...>();
        return remaining_max > N1 ? remaining_max : N1;
    }

    template<size_t Len, class ...Types> requires (sizeof...(Types) > 0) && (is_complete<Types>::value && ...) && (is_object<Types>::value && ...)
    struct aligned_union {
        static constexpr size_t alignment_value = __max<size_t, (alignof(Types), ...)>();
        struct type {
            alignas(alignment_value) unsigned char value[__max<size_t, Len, (sizeof(Types), ...)>()];
        };
    };
}