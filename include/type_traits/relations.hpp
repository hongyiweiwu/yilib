#pragma once

#include "type_traits/base.hpp"
#include "type_traits/cv_manip.hpp"
#include "type_traits/is_type.hpp"

#include "utility/declval.hpp"

namespace std::__internal {
    template<bool B, class T, class F> struct conditional { using type = T; };
    template<class T, class F> struct conditional<false, T, F> { using type = F; };

    template<class B> struct negation : bool_constant<!bool(B::value)> {};

    template<class ...Bn> struct conjunction : true_type {};
    template<class B> struct conjunction<B> : B {};
    template<class B1, class ...Bn> struct conjunction<B1, Bn...>
        : conditional<bool(B1::value), conjunction<Bn...>, B1>::type {};

    template<class ...B> struct disjunction : false_type {};
    template<class B> struct disjunction<B> : B {};
    template<class B1, class ...Bn> struct disjunction<B1, Bn...>
        : conditional<bool(B1::value), B1, disjunction<Bn...>>::type {};

    template<bool B, class T = void> struct enable_if {};
    template<class T> struct enable_if<true, T> { using type = T; };

    template<class T, class U> struct is_same: false_type {};
    template<class T> struct is_same<T, T>: true_type {};

    template<class B> auto __is_base_of_test(B*) -> true_type;
    template<class B> auto __is_base_of_test(...) -> false_type;

    template<class Base, class Derived, class = void> struct __is_base_of_impl : true_type {};
    template<class Base, class Derived> struct __is_base_of_impl<Base, Derived, void_t<decltype(__is_base_of_test<Base>(declval<Derived*>()))>> : decltype(__is_base_of_test<Base>(declval<Derived*>())) {};

    template<class Base, class Derived> struct is_base_of
        : bool_constant<is_class<Base>::value && is_class<Derived>::value && __is_base_of_impl<typename remove_cv<Base>::type, typename remove_cv<Derived>::type>::value> {};
}