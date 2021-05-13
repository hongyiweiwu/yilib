#pragma once

#include "cstddef.hpp"

namespace std {
    /* 20.5.3 Class template tuple */
    template<class ...Types> class tuple;

    /* 20.5.6 Tuple helper classes */
    template<class T> struct tuple_size;

    template<size_t I, class T> struct tuple_element;

    template<size_t I, class T> using tuple_element_t = typename tuple_element<I, T>::type;
}