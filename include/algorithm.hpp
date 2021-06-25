#pragma once

#include "concepts.hpp"
#include "type_traits.hpp"
#include "initializer_list.hpp"

namespace std {
    /* 25.8.9 Minimum and maximum */
    template<class T> requires requires (const T& a, const T& b) { { a < b } -> convertible_to<bool>; }
    constexpr const T& min(const T& a, const T& b) { return a <= b ? a : b; }

    template<class T, class Compare>
    constexpr const T& min(const T& a, const T& b, Compare cmp) { return (cmp(a, b) || a == b) ? a : b; }

    template<class T> requires is_copy_constructible_v<T> && requires (const T& a, const T& b) { { a < b } -> convertible_to<bool>; }
    constexpr T min(initializer_list<T> r) {
        constexpr auto compare_r = [](const T* begin, const T* end, auto& compare_r ) {
            if (begin + 1 == end) {
                return *begin;
            } else {
                return min(*begin, compare_r(begin + 1, end, compare_r));
            }
        };

        return compare_r(r.begin(), r.end(), compare_r);
    }

    template<class T, class Compare> requires is_copy_constructible_v<T>
    constexpr T min(initializer_list<T> r, Compare cmp) {
        constexpr auto compare_r = [cmp](const T* begin, const T* end, auto& compare_r) {
            if (begin + 1 == end) {
                return *begin;
            } else {
                return min(*begin, compare_r(begin + 1, end, compare_r), cmp);
            }
        };

        return compare_r(r.begin(), r.end(), compare_r);
    }

    template<class T> requires requires (const T& a, const T& b) { { a < b } -> convertible_to<bool>; }
    constexpr const T& max(const T& a, const T& b) { return a >= b ? a : b; }

    template<class T, class Compare>
    constexpr const T& max(const T& a, const T& b, Compare cmp) { return (!cmp(a, b) || a == b) ? a : b; }

    template<class T> requires is_copy_constructible_v<T> && requires (const T& a, const T& b) { { a < b } -> convertible_to<bool>; }
    constexpr T max(initializer_list<T> r) {
        constexpr auto compare_r = [](const T* begin, const T* end, auto& compare_r) -> T {
            if (begin + 1 == end) {
                return *begin;
            } else {
                return max(*begin, compare_r(begin + 1, end, compare_r));
            }
        };

        return compare_r(r.begin(), r.end(), compare_r);
    }

    template<class T, class Compare> requires is_copy_constructible_v<T>
    constexpr T max(initializer_list<T> r, Compare cmp) {
        constexpr auto compare_r = [cmp](const T* begin, const T* end, auto& compare_r) {
            if (begin + 1 == end) {
                return *begin;
            } else {
                return max(*begin, compare_r(begin + 1, end, compare_r), cmp);
            }
        };

        return compare_r(r.begin(), r.end(), compare_r);
    }
}