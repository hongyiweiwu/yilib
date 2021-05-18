#pragma once

#include "type_traits/decay.hpp"
#include "type_traits/base.hpp"

namespace std {
    // Forward declaration of reference_wrapper, declared in "functional.hpp".
    template<class T> class reference_wrapper;

    namespace __internal {
        template<class T> struct is_reference_wrapper : false_type {};
        template<class T> struct is_reference_wrapper<::std::reference_wrapper<T>> : true_type {};

        template<class T> struct unwrap_reference { using type = T; };
        template<class T> struct unwrap_reference<::std::reference_wrapper<T>> { using type = T&; };

        template<class T> struct unwrap_ref_decay : unwrap_reference<typename decay<T>::type> {};
    }
}