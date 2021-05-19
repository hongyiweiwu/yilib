#pragma once

#include "type_traits.hpp"
#include "utility.hpp"
#include "tuple.hpp"

#include "memory/specialized_algorithms.hpp"

namespace std {
    /* 20.10.7 Allocator argument tag */
    struct allocator_arg_t { explicit allocator_arg_t() = default; };
    inline constexpr allocator_arg_t allocator_arg{};

    /* 20.10.8 uses_allocator */
    template<class T, class Alloc> struct uses_allocator : false_type {};
    template<class T, class Alloc> requires requires { typename T::allocator_type; } && is_convertible_v<Alloc, typename T::allocator_type>
        struct uses_allocator<T, Alloc> : true_type {};
    template<class T, class Alloc> inline constexpr bool uses_allocator_v = uses_allocator<T, Alloc>::value;
    
    namespace __internal {
        // TODO: This merely checks if T is a class based on the pair template. It doesn't check if T is an explicitly defined specialization.
        template<class T> struct __is_specialized_of_pair : false_type {};
        template<class T1, class T2> struct __is_specialized_of_pair<pair<T1, T2>> : true_type {};
        template<class T> struct is_specialized_of_pair : __is_specialized_of_pair<remove_cv_t<T>> {};
    }

    template<class T, class Alloc, class ...Args> requires (!__internal::is_specialized_of_pair<T>::value)
    constexpr auto uses_allocator_construction_args(const Alloc& alloc, Args&& ...args) noexcept {
        if constexpr (!uses_allocator_v<T, Alloc> && is_constructible_v<T, Args...>) {
            return forward_as_tuple(forward<Args>(args)...);
        } else if constexpr (uses_allocator_v<T, Alloc> && is_constructible_v<T, allocator_arg_t, const Alloc&, Args...>) {
            return tuple<allocator_arg_t, const Alloc&, Args&&...>(allocator_arg, alloc, forward<Args>(args)...);
        } else if constexpr (uses_allocator_v<T, Alloc> && is_constructible_v<T, Args..., const Alloc&>) {
            return forward_as_tuple(forward<Args>(args)..., alloc);
        } else {
            static_assert(__internal::always_false<T>);
        }
    }

    template<class T, class Alloc, class Tuple1, class Tuple2> requires __internal::is_specialized_of_pair<T>::value
    constexpr auto uses_allocator_construction_args(const Alloc& alloc, piecewise_construct_t, Tuple1&& x, Tuple2&& y) noexcept {
        using T1 = typename T::first_type;
        using T2 = typename T::second_type;

        return make_tuple(
            piecewise_construct,
            apply([&alloc](auto&& ...args1) {
                return uses_allocator_construction_args<T1>(alloc, forward<decltype(args1)>(args1)...);
            }, forward<Tuple1>(x)),
            apply([&alloc](auto&& ...args2) {
                return uses_allocator_construction_args<T2>(alloc, forward<decltype(args2)>(args2)...);
            }, forward<Tuple2>(y))
        );
    }

    template<class T, class Alloc> requires __internal::is_specialized_of_pair<T>::value
    constexpr auto uses_allocator_construction_args(const Alloc& alloc) noexcept {
        return uses_allocator_construction_args<T>(alloc, piecewise_construct, forward_as_tuple(), forward_as_tuple());
    }

    template<class T, class Alloc, class U, class V> requires __internal::is_specialized_of_pair<T>::value
    constexpr auto uses_allocator_construction_args(const Alloc& alloc, U&& u, V&& v) noexcept {
        return uses_allocator_construction_args<T>(alloc, piecewise_construct, forward_as_tuple(forward<U>(u)), forward_as_tuple(forward<V>(v)));
    }

    template<class T, class Alloc, class U, class V> requires __internal::is_specialized_of_pair<T>::value
    constexpr auto uses_allocator_construction_args(const Alloc& alloc, const pair<U, V>& pr) noexcept {
        return uses_allocator_construction_args<T>(alloc, piecewise_construct, forward_as_tuple(pr.first), forward_as_tuple(pr.second));
    }

    template<class T, class Alloc, class U, class V> requires __internal::is_specialized_of_pair<T>::value
    constexpr auto uses_allocator_construction_args(const Alloc& alloc, pair<U, V>&& pr) noexcept {
        return uses_allocator_construction_args<T>(alloc, piecewise_construct, forward_as_tuple(move(pr).first), forward_as_tuple(move(pr).second));
    }

    template<class T, class Alloc, class ...Args>
    constexpr T make_obj_using_allocator(const Alloc& alloc, Args&& ...args) {
        return make_from_tuple<T>(uses_allocator_construction_args<T>(alloc, forward<Args>(args)...));
    }

    template<class T, class Alloc, class ...Args>
    constexpr T* uninitialized_construct_using_allocator(T* p, const Alloc& alloc, Args&& ...args) {
        return apply([&]<class ...U>(U&& ...xs) {
            return construct_at(p, forward<U>(xs)...);
        }, uses_allocator_construction_args<T>(alloc, forward<Args>(args)...));
    }
}