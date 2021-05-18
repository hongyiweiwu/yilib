// This header collects type traits the test whether a type is swappable/destructible/convertible to another type.

#pragma once

#include "util/macros.hpp"
#include "utility/declval.hpp"
#include "type_traits/is_type.hpp"
#include "type_traits/base.hpp"
#include "type_traits/relations.hpp"
#include "type_traits/array_manip.hpp"

namespace std::__internal {
#if __has_intrinsics_for(is_trivially_copyable)
    template<class T> requires is_complete<typename remove_all_extents<T>::type>::value || is_void<typename remove_all_extents<T>::type>::value
    struct is_trivially_copyable : bool_constant<__is_trivially_copyable(T)> {};
#endif

    template<class T, class U> 
        requires (is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value)
            && (is_complete<U>::value || is_void<U>::value || is_unbounded_array<U>::value)
    struct is_swappable_with : bool_constant<requires {
        swap(declval<T>(), declval<U>());
        swap(declval<U>(), declval<T>());
    }> {};

    template<class T> struct __is_swappable : bool_constant<is_swappable_with<T&, T&>::value> {};
    template<class T> requires (!is_referenceable<T>::value) struct __is_swappable<T> : false_type {};
    template<class T> requires is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value
    struct is_swappable : __is_swappable<T> {};

    template<class T> struct __is_destructible : false_type {};
    template<class T> requires is_reference<T>::value
    struct __is_destructible<T> : true_type {};
    template<class T> requires is_object<T>::value && requires { declval<T&>().~T(); }
    struct __is_destructible<T> : true_type {};
    template<class T> requires is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value
    struct is_destructible : __is_destructible<typename remove_all_extents<T>::type> {};

#if __has_intrinsics_for(is_trivially_destructible)
    template<class T> requires is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value
    struct is_trivially_destructible : bool_constant<__is_trivially_destructible(T)> {};
#endif

    template<class T, class U> struct __is_nothrow_swappable_with : false_type {};
    template<class T, class U> requires (noexcept(swap(declval<T>(), declval<U>()))) && (noexcept(swap(declval<U>(), declval<T>())))
    struct __is_nothrow_swappable_with<T, U> : true_type {};
    template<class T, class U>         
        requires (is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value)
            && (is_complete<U>::value || is_void<U>::value || is_unbounded_array<U>::value)
    struct is_nothrow_swappable_with : __is_nothrow_swappable_with<T, U> {};

    template<class T> struct __is_nothrow_swappable : bool_constant<is_nothrow_swappable_with<T&, T&>::value> {};
    template<class T> requires (!is_referenceable<T>::value)
    struct __is_nothrow_swappable<T> : false_type {};
    template<class T> requires is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value
    struct is_nothrow_swappable : __is_nothrow_swappable<T> {};

    template<class T> struct __is_nothrow_destructible : false_type {};
    template<class T> requires is_reference<T>::value struct __is_nothrow_destructible<T> : true_type {};
    template<class T> requires is_object<T>::value && (noexcept(declval<T&>().~T())) 
    struct __is_nothrow_destructible<T> : true_type {};
    template<class T> requires is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value
    struct is_nothrow_destructible : __is_nothrow_destructible<typename remove_all_extents<T>::type> {};

    template<class From, class To> struct __is_convertible_impl : false_type {};
    template<class From, class To> requires is_void<From>::value && is_void<To>::value
    struct __is_convertible_impl<From, To> : true_type {};
    template<class From, class To> requires requires { declval<void (To)>()(declval<From>()); }
    struct __is_convertible_impl<From, To> : true_type {};
    template<class From, class To>
        requires (is_complete<From>::value || is_void<From>::value || is_unbounded_array<From>::value)
            && (is_complete<To>::value || is_void<To>::value || is_unbounded_array<To>::value)
    struct is_convertible : __is_convertible_impl<From, To> {};

    template<class From, class To> struct __is_nothrow_convertible : false_type {};
    template<class From, class To> requires is_void<From>::value && is_void<To>::value
    struct __is_nothrow_convertible<From, To> : true_type {};
    template<class From, class To> requires (noexcept(declval<void (To) noexcept>()(declval<From>())))
    struct __is_nothrow_convertible<From, To> : true_type {};
    template<class From, class To> 
        requires (is_complete<From>::value || is_void<From>::value || is_unbounded_array<From>::value)
            && (is_complete<To>::value || is_void<To>::value || is_unbounded_array<To>::value)
    struct is_nothrow_convertible : __is_nothrow_convertible<From, To> {};

}