#pragma once

#include "util/macros.hpp"
#include "type_traits/base.hpp"
#include "type_traits/is_type.hpp"
#include "utility/declval.hpp"

namespace std::__internal {
    template<class T, class U, class _Dummy> struct __is_assignable_impl : false_type {};
    template<class T, class U> struct __is_assignable_impl<T, U, void_t<decltype(declval<T>() = declval<U>())>> : true_type {};
    template<class T, class U> 
        requires (is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value) 
            && (is_complete<U>::value || is_void<U>::value || is_unbounded_array<U>::value)
    struct is_assignable : __is_assignable_impl<T, U, void> {};
    
    template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_copy_assignable 
        : bool_constant<is_assignable<T, const T&>::value> {};
    template<class T> struct __is_copy_assignable<T, false> : false_type {};
    template<class T> requires (is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value) 
    struct is_copy_assignable : __is_copy_assignable<T> {};

    template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_move_assignable 
        : bool_constant<is_assignable<T, T&&>::value> {};
    template<class T> struct __is_move_assignable<T, false> : false_type {};
    template<class T> requires (is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value) 
    struct is_move_assignable : __is_move_assignable<T> {};

#if __has_intrinsics_for(is_trivially_assignable)
    template<class T, class U>
        requires (is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value) 
        && (is_complete<U>::value || is_void<U>::value || is_unbounded_array<U>::value)
    struct is_trivially_assignable
        : bool_constant<__is_trivially_assignable(T, U)> {};
#endif

#if __has_intrinsics_for(is_trivially_assignable)
    template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_trivially_copy_assignable 
        : bool_constant<is_trivially_assignable<T, const T&>::value> {};
    template<class T> struct __is_trivially_copy_assignable<T, false> : false_type {};
    template<class T> requires (is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value) 
    struct is_trivially_copy_assignable : __is_trivially_copy_assignable<T> {};
#endif

#if __has_intrinsics_for(is_trivially_assignable)
    template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_trivially_move_assignable 
        : bool_constant<is_trivially_assignable<T, T&&>::value> {};
    template<class T> struct __is_trivially_move_assignable<T, false> : false_type {};
    template<class T> requires (is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value) 
    struct is_trivially_move_assignable : __is_trivially_move_assignable<T> {};
#endif

    template<class T, class U, class _Dummy> struct __is_nothrow_assignable_impl : false_type {};
    template<class T, class U> struct __is_nothrow_assignable_impl<T, U, decltype(noexcept(declval<T>() = declval<U>()))> : true_type {};
    template<class T, class U>         
        requires (is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value) 
            && (is_complete<U>::value || is_void<U>::value || is_unbounded_array<U>::value)
    struct is_nothrow_assignable : __is_nothrow_assignable_impl<T, U, bool> {};
    
    template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_nothrow_copy_assignable 
        : bool_constant<is_nothrow_assignable<T, const T&>::value> {};
    template<class T> struct __is_nothrow_copy_assignable<T, false> : false_type {};
    template<class T> requires (is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value)
    struct is_nothrow_copy_assignable : __is_nothrow_copy_assignable<T> {};

    template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_nothrow_move_assignable 
        : bool_constant<is_nothrow_assignable<T, T&&>::value> {};
    template<class T> struct __is_nothrow_move_assignable<T, false> : false_type {};
    template<class T> requires (is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value)
    struct is_nothrow_move_assignable : __is_nothrow_move_assignable<T> {};
}