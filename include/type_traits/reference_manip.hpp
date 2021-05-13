#pragma once

#include "type_traits/is_type.hpp"

namespace std::__internal {
    template<class T> struct remove_reference { using type = T; };
    template<class T> struct remove_reference<T&> { using type = T; };
    template<class T> struct remove_reference<T&&> { using type = T; };

    template<class T, bool Referenceable = is_referenceable<T>::value> struct __add_lvalue_reference { using type = T&; };
    template<class T> struct __add_lvalue_reference<T, false> { using type = T; };
    template<class T> struct add_lvalue_reference : __add_lvalue_reference<T> {};

    template<class T, bool Referenceable = is_referenceable<T>::value> struct __add_rvalue_reference { using type = T&&; };
    template<class T> struct __add_rvalue_reference<T, false> { using type = T; };
    template<class T> struct add_rvalue_reference : __add_rvalue_reference<T> {};
}