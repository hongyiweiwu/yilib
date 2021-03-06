#pragma once

#include "util/macros.hpp"
#include "type_traits/base.hpp"
#include "type_traits/is_type.hpp"
#include "utility/declval.hpp"

namespace std::__internal {
    template<class T, class ...Args>
        requires (is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value)
            && ((is_complete<Args>::value || is_void<Args>::value || is_unbounded_array<Args>::value) && ...)
    struct is_constructible : bool_constant<requires { T(declval<Args>()...); }> {};

    template<class T> requires is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value
    struct is_default_constructible : is_constructible<T> {};

    template<class T> struct __is_copy_constructible : bool_constant<is_constructible<T, const T&>::value> {};
    template<class T> requires (!is_referenceable<T>::value) struct __is_copy_constructible<T> : false_type {};
    template<class T> requires is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value
    struct is_copy_constructible : __is_copy_constructible<T> {};

    template<class T> struct __is_move_constructible : bool_constant<is_constructible<T, T&&>::value> {};
    template<class T> requires (!is_referenceable<T>::value) struct __is_move_constructible<T> : false_type {};
    template<class T> requires is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value
    struct is_move_constructible : __is_move_constructible<T> {};

#if __has_intrinsics_for(is_trivially_constructible)
    template<class T, class ...Args> 
        requires (is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value)
            && ((is_complete<Args>::value || is_void<Args>::value || is_unbounded_array<Args>::value) && ...)
    struct is_trivially_constructible
        : bool_constant<__is_trivially_constructible(T, Args...)> {};
#endif

#if __has_intrinsics_for(is_trivially_constructible)
    template<class T> requires is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value
    struct is_trivially_default_constructible : is_trivially_constructible<T> {};
#endif

#if __has_intrinsics_for(is_trivially_constructible)
    template<class T> struct __is_trivially_copy_constructible : bool_constant<is_trivially_constructible<T, const T&>::value> {};
    template<class T> requires (!is_referenceable<T>::value) struct __is_trivially_copy_constructible<T> : false_type {};
    template<class T> requires is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value
    struct is_trivially_copy_constructible : __is_trivially_copy_constructible<T> {};
#endif

#if __has_intrinsics_for(is_trivially_constructible)
    template<class T> struct __is_trivially_move_constructible : bool_constant<is_trivially_constructible<T, T&&>::value> {};
    template<class T> requires (!is_referenceable<T>::value) struct __is_trivially_move_constructible<T> : false_type {};
    template<class T> requires is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value
    struct is_trivially_move_constructible : __is_trivially_move_constructible<T> {};
#endif

    template<class T, class ...Args> struct __is_nothrow_constructible_impl : false_type {};
    template<class T, class ...Args> requires (noexcept(T(declval<Args>()...))) struct __is_nothrow_constructible_impl<T, Args...> : true_type {};
    template<class T, class ...Args> 
        requires (is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value)
            && ((is_complete<Args>::value || is_void<Args>::value || is_unbounded_array<Args>::value) && ...)
    struct is_nothrow_constructible : __is_nothrow_constructible_impl<T, Args...> {};

    template<class T> requires is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value
    struct is_nothrow_default_constructible : is_nothrow_constructible<T> {};

    template<class T> struct __is_nothrow_copy_constructible : bool_constant<is_nothrow_constructible<T, const T&>::value> {};
    template<class T> requires (!is_referenceable<T>::value) struct __is_nothrow_copy_constructible<T> : false_type {};
    template<class T> requires is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value
    struct is_nothrow_copy_constructible : __is_nothrow_copy_constructible<T> {};

    template<class T> struct __is_nothrow_move_constructible : bool_constant<is_nothrow_constructible<T, T&&>::value> {};
    template<class T> requires (!is_referenceable<T>::value) struct __is_nothrow_move_constructible<T> : false_type {};
    template<class T> requires is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value
    struct is_nothrow_move_constructible : __is_nothrow_move_constructible<T> {};
}