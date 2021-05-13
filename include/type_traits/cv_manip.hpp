#pragma once

namespace std::__internal {
    template<class T> struct remove_const { using type = T; };
    template<class T> struct remove_const<const T> { using type = T; };

    template<class T> struct remove_volatile { using type = T; };
    template<class T> struct remove_volatile<volatile T> { using type = T; };

    template<class T> struct remove_cv { using type = T; };
    template<class T> struct remove_cv<const T> { using type = T; };
    template<class T> struct remove_cv<volatile T> { using type = T; };
    template<class T> struct remove_cv<const volatile T> { using type = T; };

    template<class T> struct add_const { using type = const T; };
    template<class T> struct add_volatile { using type = volatile T; };
    template<class T> struct add_cv { using type = const volatile T; };
}