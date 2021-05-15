#pragma once

#include "type_traits/is_type.hpp"

namespace std::__internal {
    template<class T> struct remove_reference { using type = T; };
    template<class T> struct remove_reference<T&> { using type = T; };
    template<class T> struct remove_reference<T&&> { using type = T; };

    template<class T> struct add_lvalue_reference { using type = T&; };
    template<class T> requires (!is_referenceable<T>::value)
    struct add_lvalue_reference<T> { using type = T; };

    template<class T> struct add_rvalue_reference { using type = T&&; };
    template<class T> requires (!is_referenceable<T>::value)
    struct add_rvalue_reference<T> { using type = T; };
}