#pragma once

#include "type_traits/reference_manip.hpp"
#include "type_traits/relations.hpp"
#include "type_traits/pointer_manip.hpp"
#include "type_traits/array_manip.hpp"

namespace std::__internal {
    template<class T> requires is_array<T>::value 
    auto __decay(int) -> typename remove_extent<T>::type*;
    template<class T> requires is_function<T>::value 
    auto __decay(int) -> typename add_pointer<T>::type;
    template<class T> auto __decay(...) -> typename remove_cv<T>::type;

    template<class T> struct decay {
        using type = decltype(__decay<typename remove_reference<T>::type>(0));
    }; 
}