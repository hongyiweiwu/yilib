#pragma once

#include "type_traits/relations.hpp"
#include "type_traits/reference_manip.hpp"
#include "type_traits/cv_manip.hpp"
#include "type_traits/is_type.hpp"

namespace std::__internal {
    template<class T> struct remove_pointer { using type = T; };
    template<class T> struct remove_pointer<T*> { using type = T; };
    template<class T> struct remove_pointer<T* const> { using type = T; };
    template<class T> struct remove_pointer<T* volatile> { using type = T; };
    template<class T> struct remove_pointer<T* const volatile> { using type = T; };

    template<class T> requires is_referenceable<T>::value
    auto __add_pointer(int) -> typename remove_reference<T>::type*;
    template<class T> requires is_void<typename remove_cv<T>::type>::value
    auto __add_pointer(int) -> T*;
    template<class T> 
    auto __add_pointer(...) -> T;

    template<class T> struct add_pointer {
        using type = decltype(__add_pointer<T>(0));
    };
}