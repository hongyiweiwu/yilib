#pragma once

#include "concepts.hpp"

namespace std {
    /* 25.8.9 Minimum and maximum */
    template<class T> requires requires (const T& a, const T& b) { { a < b } -> convertible_to<bool>; }
    constexpr const T& min(const T& a, const T& b) { return a < b ? a : b; }

    template<class T, class Compare> requires requires (const T& a, const T& b, Compare cmp) { { cmp(a, b) } -> convertible_to<bool>; }
    constexpr const T& min(const T& a, const T& b, Compare cmp) { return cmp(a, b) ? a : b; }
}