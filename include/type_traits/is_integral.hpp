#pragma once

#include "type_traits/base.hpp"
#include "type_traits/cv_manip.hpp"

namespace yilib::__internal {
    template<class T> struct __is_integral_impl : false_type {};
    template<> struct __is_integral_impl<bool> : true_type {};
    template<> struct __is_integral_impl<char> : true_type {};
    template<> struct __is_integral_impl<signed char> : true_type {};
    template<> struct __is_integral_impl<unsigned char> : true_type {};
    template<> struct __is_integral_impl<wchar_t> : true_type {};
    template<> struct __is_integral_impl<char8_t> : true_type {};
    template<> struct __is_integral_impl<char16_t> : true_type {};
    template<> struct __is_integral_impl<char32_t> : true_type {};
    template<> struct __is_integral_impl<short> : true_type {};
    template<> struct __is_integral_impl<unsigned short> : true_type {};
    template<> struct __is_integral_impl<int> : true_type {};
    template<> struct __is_integral_impl<unsigned int> : true_type {};
    template<> struct __is_integral_impl<long> : true_type {};
    template<> struct __is_integral_impl<unsigned long> : true_type {};
    template<> struct __is_integral_impl<long long> : true_type {};
    template<> struct __is_integral_impl<unsigned long long> : true_type {};

    template<class T> struct is_integral : __is_integral_impl<typename remove_cv<T>::type> {};
}