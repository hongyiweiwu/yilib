#pragma once

namespace yilib {
    // Forward declaration of reference_wrapper, declared in "functional.hpp".
    template<class T> class reference_wrapper;

    namespace __internal {
        template<class T> struct __is_reference_wrapper : false_type {};
        template<class T> struct __is_reference_wrapper<::yilib::reference_wrapper<T>> : true_type {};

        template<class T> struct unwrap_reference {};
        template<class T> struct unwrap_reference<::yilib::reference_wrapper<T>> { using type = T&; };

        template<class T> struct unwrap_ref_decay : unwrap_reference<typename decay<T>::type> {};
    }
}