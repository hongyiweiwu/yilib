#pragma once

#include "cstddef.hpp"

#include "type_traits/base.hpp"
#include "type_traits/is_type.hpp"

namespace std::__internal {
    template<class T> requires requires { alignof(T); }
    struct alignment_of : integral_constant<std::size_t, alignof(T)> {};

    template<std::size_t Len> requires (Len > 0) && (Len <= alignof(std::max_align_t))
    consteval std::size_t __default_alignment() noexcept {
        std::size_t alignment = 1;
        while (Len >= alignment) {
            alignment *= 2;
        }

        return alignment / 2;
    }

    template<std::size_t Align> requires (Align >= 1) && (Align <= alignof(std::max_align_t))
    consteval bool __is_alignment_of_some_type() noexcept {
        for (std::size_t alignment = 1; alignment <= alignof(std::max_align_t); alignment *= 2) {
            if (alignment == Align) {
                return true;
            }
        }

        return false;
    }

    template<std::size_t Len, std::size_t Align = __default_alignment<Len>()> 
        requires (Len > 0) && (Align == __default_alignment<Len>() || __is_alignment_of_some_type<Align>())
    struct aligned_storage {
        struct type {
            alignas(Align) unsigned char data[Len];
        };
    };

    template<class T, T N>
    consteval T __max() noexcept {
        return N;
    }

    template<class T, T N1, T ...N>
    requires (sizeof...(N) >= 1)
    consteval T __max() noexcept {
        constexpr T remaining_max = __max<T, N...>();
        return remaining_max > N1 ? remaining_max : N1;
    }

    template<std::size_t Len, class ...Types> requires (sizeof...(Types) > 0) && (is_complete<Types>::value && ...) && (is_object<Types>::value && ...)
    struct aligned_union {
        static constexpr std::size_t alignment_value = __max<std::size_t, (alignof(Types), ...)>();
        struct type {
            alignas(alignment_value) unsigned char value[__max<std::size_t, Len, (sizeof(Types), ...)>()];
        };
    };
}