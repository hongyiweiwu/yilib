#pragma once

#include "type_traits/reference_manip.hpp"
#include "type_traits/relations.hpp"
#include "type_traits/pointer_manip.hpp"
#include "type_traits/array_manip.hpp"

namespace std::__internal {
    template<class T> auto __decay(int) -> typename enable_if<is_array<T>::value, typename remove_extent<T>::type*>::type;
    template<class T> auto __decay(int) -> typename enable_if<is_function<T>::value, typename add_pointer<T>::type>::type;
    template<class T> auto __decay(...) -> typename remove_cv<T>::type;

    template<class T> struct decay {
        using type = decltype(__decay<typename remove_reference<T>::type>(0));
    }; 
}