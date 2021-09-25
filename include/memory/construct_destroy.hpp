#pragma once

#include "type_traits.hpp"
#include "concepts.hpp"
#include "memory/pointer_util.hpp"


namespace std {
    /* 25.11.8 construct_at */
    template<class T, class ...Args>
    requires requires (void* ptr, Args&& ...args) { ::new (ptr) T(forward<Args>(args)...); }
    constexpr T* construct_at(T* p, Args&& ...args) {
        return ::new (const_cast<void*>(static_cast<const volatile void*>(p))) T(forward<Args>(args)...);
    }

    namespace ranges {
        struct __construct_at_fn {
            template<class T, class ...Args>
            requires requires (void* ptr, Args&& ...args) { ::new (ptr) T(forward<Args>(args)...); }
            constexpr T* operator()(T* p, Args&& ...args) const {
                return ::std::construct_at(p, forward<Args>(args)...);
            }
        };

        inline namespace __fn_objects {
            inline constexpr ::std::ranges::__construct_at_fn construct_at;
        }
    }

    /* 25.11.9 destroy */
    template<class T>
    constexpr void destroy_at(T* location) {
        if constexpr (is_array<T>::value) {
            // Equivalent to destroy(begin(*location), end(*location)).
            for (T& elem: *location) {
                destroy_at(addressof(elem));
            }
        } else {
            location->~T();
        }
    }

    namespace ranges {
        struct __destroy_at_fn {
            template<destructible T>
            constexpr void operator()(T* location) const noexcept {
                return ::std::destroy_at(location);
            }
        };

        inline namespace __fn_objects {
            inline constexpr ::std::ranges::__destroy_at_fn destroy_at;
        }
    }
}