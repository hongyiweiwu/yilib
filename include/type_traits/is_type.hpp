#pragma once

#include "cstddef.hpp"

#include "type_traits/base.hpp"
#include "type_traits/cv_manip.hpp"
#include "type_traits/is_integral.hpp"
#include "type_traits/array_manip.hpp"
#include "util/macros.hpp"

namespace std::__internal {
    template<class T> struct is_complete : false_type {};
    template<class T> requires requires { sizeof(T); }
    struct is_complete<T> : true_type {};

    template<class T> struct __is_void_impl : false_type {};
    template<> struct __is_void_impl<void> : true_type {};
    template<class T> struct is_void : __is_void_impl<typename remove_cv<T>::type> {};

    template<class T> struct __is_null_pointer : false_type {};
    template<> struct __is_null_pointer<nullptr_t> : true_type {};
    template<class T> struct is_null_pointer : __is_null_pointer<typename remove_cv<T>::type> {};

    template<class T> struct __is_floating_point_impl : false_type {};
    template<> struct __is_floating_point_impl<float> : true_type {};
    template<> struct __is_floating_point_impl<double> : true_type {};
    template<> struct __is_floating_point_impl<long double> : true_type {};
    template<class T> struct is_floating_point : __is_floating_point_impl<typename remove_cv<T>::type> {};

    template<class T> struct __is_unbounded_array : false_type {};
    template<class T> struct __is_unbounded_array<T[]> : true_type {};
    template<class T> struct is_unbounded_array : __is_unbounded_array<typename remove_cv<T>::type> {};

    template<class T> struct __is_bounded_array : false_type {};
    template<class T, size_t N> struct __is_bounded_array<T[N]> : true_type {};
    template<class T> struct is_bounded_array : __is_bounded_array<typename remove_cv<T>::type> {};

    template<class T> struct is_array : bool_constant<is_unbounded_array<T>::value || is_bounded_array<T>::value> {};

    template<class T> struct __is_pointer_impl : false_type {};
    template<class T> struct __is_pointer_impl<T*> : true_type {};
    template<class T> struct is_pointer : __is_pointer_impl<typename remove_cv<T>::type> {};

    template<class T> struct __is_lvalue_reference_impl : false_type {};
    template<class T> struct __is_lvalue_reference_impl<T&> : true_type {};
    template<class T> struct is_lvalue_reference : __is_lvalue_reference_impl<typename remove_cv<T>::type> {};

    template<class T> struct __is_rvalue_reference_impl : false_type {};
    template<class T> struct __is_rvalue_reference_impl<T&&> : true_type {};
    template<class T> struct is_rvalue_reference : __is_rvalue_reference_impl<typename remove_cv<T>::type> {};

    template<class T> struct __is_member_object_pointer__impl : false_type {};
    template<class T, class U> struct __is_member_object_pointer__impl<T U::*> : true_type {};
    template<class T> struct is_member_object_pointer : __is_member_object_pointer__impl<typename remove_cv<T>::type> {};

#if __has_intrinsics_for(is_enum)
    template<class T> struct is_enum : bool_constant<__is_enum(T)> {};
#endif

#if __has_intrinsics_for(is_union)
    template<class T> struct is_union : bool_constant<__is_union(T)> {};
#endif

    template<class T> struct __is_class_impl : false_type {};
    template<class T> requires (!is_union<T>::value) && requires (int T::* a) { a; }
    struct __is_class_impl<T> : true_type {};
    template<class T> struct is_class : __is_class_impl<typename remove_cv<T>::type> {};

    template<class T> struct is_reference : bool_constant<is_lvalue_reference<T>::value || is_rvalue_reference<T>::value> {};
    
    template<class T> struct is_const : false_type {};
    template<class T> struct is_const<const T> : true_type {};

    template<class T> struct is_function : bool_constant<!is_const<const T>::value && !is_reference<T>::value> {};

    template<class T> struct __is_member_function_pointer_impl : false_type {};
    template<class T, class U> struct __is_member_function_pointer_impl<U T::*> : is_function<U> {};
    template<class T> struct is_member_function_pointer : __is_member_function_pointer_impl<typename remove_cv<T>::type> {};

    template<class T> struct is_arithmetic : bool_constant<is_integral<T>::value || is_floating_point<T>::value> {};

    template<class T> struct is_fundamental : bool_constant<is_arithmetic<T>::value || is_void<T>::value || is_null_pointer<T>::value> {};

    template<class T> struct is_object : bool_constant<!is_function<T>::value && !is_reference<T>::value && !is_void<T>::value> {};

    template<class T> struct is_member_pointer : bool_constant<is_member_function_pointer<T>::value || is_member_object_pointer<T>::value> {};

    template<class T> struct is_scalar : bool_constant<is_arithmetic<T>::value || is_pointer<T>::value || is_member_pointer<T>::value || is_enum<T>::value || is_null_pointer<T>::value> {};

    template<class T> using is_compound = bool_constant<!is_fundamental<T>::value>;

    template<class T> struct is_volatile : false_type {};
    template<class T> struct is_volatile<volatile T> : true_type {};

#if __has_intrinsics_for(is_empty)
    template<class T> requires (!is_class<T>::value) || is_complete<T>::value
    struct is_empty : bool_constant<__is_empty(T)> {};
#endif

    template<class T> struct __is_polymorphic_impl : false_type {};
    template<class T> requires requires { dynamic_cast<void*>(static_cast<T*>(nullptr)); }
    struct __is_polymorphic_impl<T> : true_type {};

    template<class T> requires (!is_class<T>::value) || is_complete<T>::value
    using is_polymorphic = __is_polymorphic_impl<typename remove_cv<T>::type>;

#if __has_intrinsics_for(is_abstract)
    template<class T> requires (!is_class<T>::value) || is_complete<T>::value
    struct is_abstract : bool_constant<__is_abstract(T)> {};
#endif

#if __has_intrinsics_for(is_final)
    template<class T> requires (!is_class<T>::value) || is_complete<T>::value || (!is_union<T>::value)
    struct is_final : bool_constant<__is_final(T)> {};
#endif

#if __has_intrinsics_for(is_aggregate)
    template<class T> 
        requires is_complete<typename remove_all_extents<T>::type>::value || is_void<typename remove_all_extents<T>::type>::value
    struct is_aggregate : bool_constant<__is_aggregate(T)> {};
#endif

    template<class T> struct is_signed
        : bool_constant<is_arithmetic<T>::value && (T(-1) < T(0))> {};

    template<class T> struct is_unsigned
        : bool_constant<is_arithmetic<T>::value && (T(-1) > T(0))> {};

    template<class T> auto __is_referenceable_test(int) -> T&;
    template<class T> auto __is_referenceable_test(...) -> void;
    template<class T> struct is_referenceable : bool_constant<!is_void<decltype(__is_referenceable_test<T>(0))>::value> {};
}