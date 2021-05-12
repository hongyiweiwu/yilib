// This header collects type traits the test whether a type is swappable/destructible/convertible to another type.

#pragma once

#include "util/macros.hpp"
#include "utility/declval.hpp"
#include "type_traits/is_type.hpp"
#include "type_traits/base.hpp"
#include "type_traits/relations.hpp"

namespace yilib::__internal {
#if __has_intrinsics_for(is_trivially_copyable)
    template<class T> struct is_trivially_copyable
        : bool_constant<__is_trivially_copyable(T)> {};
#endif

    template<class T, class U, class __Dummy1, class __Dummy2> struct __is_swappable_with : false_type {};
    template<class T, class U> struct __is_swappable_with<T, U, decltype(swap(declval<T>(), declval<U>())), decltype(swap(declval<U>(), declval<T>()))> : true_type {};
    template<class T, class U> struct is_swappable_with : __is_swappable_with<T, U, void, void> {};

    template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_swappable : bool_constant<is_swappable_with<T&, T&>::value> {};
    template<class T> struct __is_swappable<T, false> : false_type {};
    template<class T> struct is_swappable : __is_swappable<T> {};

    template<class T, class __Dummy = void> struct __is_destructible : false_type {};
    template<class T> struct __is_destructible<T&> : true_type {};
    template<class T> struct __is_destructible<T&&> : true_type {};
    template<class T> struct __is_destructible<T, decltype(declval<T&>().~T())> : true_type {};
    template<class T> struct is_destructible : __is_destructible<typename remove_all_extents<T>::type> {};

#if __has_intrinsics_for(is_trivially_destructible)
    template<class T> struct is_trivially_destructible
        : bool_constant<__is_trivially_destructible(T)> {};
#endif

    template<class T, class U, class __Dummy1, class __Dummy2> struct __is_nothrow_swappable_with : false_type {};
    template<class T, class U> struct __is_nothrow_swappable_with<T, U, decltype(noexcept(swap(declval<T>(), declval<U>()))), decltype(noexcept(swap(declval<U>(), declval<T>())))> : true_type {};
    template<class T, class U> struct is_nothrow_swappable_with : __is_nothrow_swappable_with<T, U, bool, bool> {};

    template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_nothrow_swappable : bool_constant<is_nothrow_swappable_with<T&, T&>::value> {};
    template<class T> struct __is_nothrow_swappable<T, false> : false_type {};
    template<class T> struct is_nothrow_swappable : __is_nothrow_swappable<T> {};

    template<class T, class __Dummy = bool> struct __is_nothrow_destructible : false_type {};
    template<class T> struct __is_nothrow_destructible<T&> : true_type {};
    template<class T> struct __is_nothrow_destructible<T&&> : true_type {};
    template<class T> struct __is_nothrow_destructible<T, decltype(noexcept(declval<T&>().~T()))> : true_type {};
    template<class T> struct is_nothrow_destructible : __is_nothrow_destructible<typename remove_all_extents<T>::type> {};

    template<class From, class To> auto __is_convertible_test()
        -> typename enable_if<is_void<From>::value && is_void<To>::value, int>::type;
    template<class From, class To> requires requires { declval<void (To)>()(declval<From>()); }
    auto __is_convertible_test() -> int;

    template<class From, class To, class = int> struct __is_convertible_impl : false_type {};
    template<class From, class To> struct __is_convertible_impl<From, To, decltype(__is_convertible_test<From, To>())> : true_type {};
    template<class From, class To> struct is_convertible : __is_convertible_impl<From, To> {};

    template<class From, class To> auto __is_nothrow_convertible_test()
        -> typename enable_if<is_void<From>::value && is_void<To>::value, int>::type;
    template<class From, class To> requires requires { noexcept(declval<void (To) noexcept>()(declval<From>())); }
    auto __is_nothrow_convertible_test() -> int;

    template<class From, class To, class = int> struct __is_nothrow_convertible : false_type {};
    template<class From, class To> struct __is_nothrow_convertible<From, To, decltype(__is_nothrow_convertible_test<From, To>())> : true_type {};
    template<class From, class To> struct is_nothrow_convertible : __is_nothrow_convertible<From, To> {};

}