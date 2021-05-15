#pragma once

#include "util/macros.hpp"

#include "type_traits/reference_manip.hpp"
#include "type_traits/base.hpp"
#include "type_traits/cv_manip.hpp"
#include "type_traits/is_type.hpp"
#include "type_traits/array_manip.hpp"
#include "type_traits/relations.hpp"
#include "type_traits/is_constructible.hpp"

namespace std::__internal {
#if __has_intrinsics_for(is_standard_layout)
    template<class T>
        requires is_complete<typename remove_all_extents<T>::type>::value || is_void<typename remove_all_extents<T>::type>::value
    struct is_standard_layout : bool_constant<__is_standard_layout(T)> {};
#endif

#if __has_intrinsics_for(is_trivally_constructible) && __has_intrinsics_for(is_trivially_copyable)
    template<class T> struct __is_trivial_impl
        : bool_constant<is_scalar<T>::value || (is_trivially_copyable<T>::value && is_trivially_default_constructible<T>::value)> {};
    template<class T> 
        requires is_complete<typename remove_all_extents<T>::type>::value || is_void<typename remove_all_extents<T>::type>::value
    struct is_trivial : __is_trivial_impl<typename remove_all_extents<typename remove_cv<T>::type>::type> {};
#endif

#if __has_intrinsics_for(has_virtual_destructor)
    template<class T> requires (!is_class<T>::value) || is_complete<T>::value
    struct has_virtual_destructor : bool_constant<__has_virtual_destructor(T)> {};
#endif

#if __has_intrinsics_for(has_unique_object_representations)
    template<class T> requires (is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value) 
    struct has_unique_object_representations
        : bool_constant<__has_unique_object_representations(typename remove_cv<typename remove_all_extents<T>::type>::type)> {};
#endif

    template<class T> struct remove_cvref : remove_cv<typename remove_reference<T>::type> {};

#if __has_intrinsics_for(is_layout_compatible)
    template<class T, class U>
        requires (is_complete<T>::value || is_void<T>::value || is_unbounded_array<T>::value)
            && (is_complete<U>::value || is_void<U>::value || is_unbounded_array<U>::value)
    struct is_layout_compatible : bool_constant<__is_layout_compatible(T, U)> {};
#endif

#if __has_intrinsics_for(is_pointer_interconvertible_base_of)
    template<class Base, class Derived>
        requires (!is_class<Base>::value) || (!is_class<Derived>::value) 
            || is_same<typename remove_cv<Base>::type, typename remove_cv<Derived>::type>::value || is_complete<Derived>::value
    struct is_pointer_interconvertible_base_of
        : bool_constant<__is_pointer_interconvertible_base_of(Base, Derived)> {};
#endif

#if __has_intrinsics_for(underlying_type) && __has_intrinsics_for(is_enum)
    template<class T> struct __underlying_type_impl {};
    template<class T> requires is_enum<T>::value
    struct __underlying_type_impl<T> { using type = __underlying_type(T); };

    template<class T> requires (!is_enum<T>::value) || is_complete<T>::value
    struct underlying_type : __underlying_type_impl<T> {};
#endif

#if __has_intrinsics_for(builtin_is_constant_evaluated)
    constexpr bool is_constant_evaluated() noexcept {
        return __builtin_is_constant_evaluated();
    }
#endif

#if __has_intrinsics_for(is_pointer_interconvertible_with_class)
    template<class S, class M> requires is_complete<S>::value
    constexpr bool is_pointer_interconvertible_with_class(M S::* mp) noexcept {
        return __is_pointer_interconvertible_with_class(mp);
    }
#endif

#if __has_intrinsics_for(is_corresponding_member)
    template<class S1, class S2, class M1, class M2> requires is_complete<S1>::value && is_complete<S2>::value
    constexpr bool is_corresponding_member(M1 S1::* mp, M2 S2::* mq) noexcept {
        return __is_corresponding_member(mp, mq);
    }
#endif
}