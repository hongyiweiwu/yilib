// Declares several static methods in <tuple> that are used in headers that <tuple> depends on, e.g. <utility>.

#pragma once

#include "cstddef.hpp"
#include "type_traits.hpp"
#include "compare.hpp"

namespace std {
    template<class ...T> class tuple;

    template<class T> struct tuple_size;

    template<class T> struct tuple_size<const T> {};
    template<class T> requires requires { tuple_size<remove_cv_t<T>>::value; }
    struct tuple_size<const T> : integral_constant<std::size_t, tuple_size<remove_cv_t<T>>::value> {};

    template<class ...T> struct tuple_size<tuple<T...>> : integral_constant<std::size_t, sizeof...(T)> {};

    template<class T> inline constexpr std::size_t tuple_size_v = tuple_size<T>::value;

    template<std::size_t I, class T> struct tuple_element;
    template<std::size_t I, class T> struct tuple_element<I, const T> {
        using type = add_const_t<typename tuple_element<I, remove_cv_t<T>>::type>;
    };

    template<std::size_t I, class T> using tuple_element_t = typename tuple_element<I, T>::type;
}