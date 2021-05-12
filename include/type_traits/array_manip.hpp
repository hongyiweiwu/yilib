#pragma once

#include "cstddef.hpp"
#include "type_traits/base.hpp"

namespace yilib::__internal {
    template<class T> struct remove_extent { using type = T; };
    template<class T> struct remove_extent<T[]> { using type = T; };
    template<class T, size_t N> struct remove_extent<T[N]> { using type = T; };

    template<class T> struct remove_all_extents { using type = T; };
    template<class T> struct remove_all_extents<T[]> { using type = remove_all_extents<T>; };
    template<class T, size_t N> struct remove_all_extents<T[N]> { using type = remove_all_extents<T>; };

    template<class T> struct rank : integral_constant<size_t, 0> {};
    template<class T, size_t N> struct rank<T[N]> : integral_constant<size_t, 1 + rank<T>::value> {};
    template<class T> struct rank<T[]> : integral_constant<size_t, 1 + rank<T>::value> {};

    template<class T, unsigned int I = 0> struct extent : integral_constant<size_t, 0> {};
    template<class T, size_t N> struct extent<T[N], 0> : integral_constant<size_t, N> {};
    template<class T, unsigned int I, size_t N> struct extent<T[N], I> : extent<T, I - 1> {};
    template<class T> struct extent<T[], 0> : integral_constant<size_t, 0> {};
    template<class T, unsigned int I> struct extent<T[], I> : extent<T, I - 1> {};
}