#pragma once

#include "type_traits/alignment.hpp"
#include "type_traits/array_manip.hpp"
#include "type_traits/base.hpp"
#include "type_traits/common_reference.hpp"
#include "type_traits/cv_manip.hpp"
#include "type_traits/decay.hpp"
#include "type_traits/is_assignable.hpp"
#include "type_traits/is_constructible.hpp"
#include "type_traits/is_integral.hpp"
#include "type_traits/is_invocable.hpp"
#include "type_traits/is_operation.hpp"
#include "type_traits/is_type.hpp"
#include "type_traits/pointer_manip.hpp"
#include "type_traits/reference_wrapper.hpp"
#include "type_traits/reference_manip.hpp"
#include "type_traits/relations.hpp"
#include "type_traits/signedness_manip.hpp"
#include "type_traits/others.hpp"

#include "util/macros.hpp"

namespace std {
    /* 20.15.4 Helper class */
    using __internal::integral_constant;
    using __internal::bool_constant;
    using __internal::true_type;
    using __internal::false_type;

    /* 20.15.5.2 Primary type categories */
    using __internal::is_void;
    template<class T> inline constexpr bool is_void_v = is_void<T>::value;
    using __internal::is_null_pointer;
    template<class T> inline constexpr bool is_null_pointer_v = is_null_pointer<T>::value;
    using __internal::is_integral;
    template<class T> inline constexpr bool is_integral_v = is_integral<T>::value;
    using __internal::is_floating_point;
    template<class T> inline constexpr bool is_floating_point_v = is_floating_point<T>::value;
    using __internal::is_array;
    template<class T> inline constexpr bool is_array_v = is_array<T>::value;
    using __internal::is_pointer;
    template<class T> inline constexpr bool is_pointer_v = is_pointer<T>::value;
    using __internal::is_lvalue_reference;
    template<class T> inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;
    using __internal::is_rvalue_reference;
    template<class T> inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;
    using __internal::is_member_object_pointer;
    template<class T> inline constexpr bool is_member_object_pointer_v = is_member_object_pointer<T>::value;
    using __internal::is_member_function_pointer;
    template<class T> inline constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;
#if __has_intrinsics_for(is_enum)
    using __internal::is_enum;
    template<class T> inline constexpr bool is_enum_v = is_enum<T>::value;
#endif
#if __has_intrinsics_for(is_union)
    using __internal::is_union;
    template<class T> inline constexpr bool is_union_v = is_union<T>::value;
#endif
    using __internal::is_class;
    template<class T> inline constexpr bool is_class_v = is_class<T>::value;
    using __internal::is_function;
    template<class T> inline constexpr bool is_function_v = is_function<T>::value;

    /* 20.15.5.3 Composite type categories */
    using __internal::is_reference;
    template<class T> inline constexpr bool is_reference_v = is_reference<T>::value;
    using __internal::is_arithmetic;
    template<class T> inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;
    using __internal::is_fundamental;
    template<class T> inline constexpr bool is_fundamental_v = is_fundamental<T>::value;
    using __internal::is_object;
    template<class T> inline constexpr bool is_object_v = is_object<T>::value;
    using __internal::is_scalar;
    template<class T> inline constexpr bool is_scalar_v = is_scalar<T>::value;
    using __internal::is_compound;
    template<class T> inline constexpr bool is_compound_v = is_compound<T>::value;
    using __internal::is_member_pointer;
    template<class T> inline constexpr bool is_member_pointer_v = is_member_pointer<T>::value;

    /* 20.15.5.4 Type properties */
    using __internal::is_const;
    template<class T> inline constexpr bool is_const_v = is_const<T>::value;
    using __internal::is_volatile;
    template<class T> inline constexpr bool is_volatile_v = is_volatile<T>::value;
#if __has_intrinsics_for(is_trivally_constructible) && __has_intrinsics_for(is_trivially_copyable)
    using __internal::is_trivial;
    template<class T> inline constexpr bool is_trivial_v = is_trivial<T>::value;
#endif
#if __has_intrinsics_for(is_trivially_copyable)
    using __internal::is_trivially_copyable;
    template<class T> inline constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;
#endif
#if __has_intrinsics_for(is_standard_layout)
    using __internal::is_standard_layout;
    template<class T> inline constexpr bool is_standard_layout_v = is_standard_layout<T>::value;
#endif
#if __has_intrinsics_for(is_empty)
    using __internal::is_empty;
    template<class T> inline constexpr bool is_empty_v = is_empty<T>::value;
#endif
    using __internal::is_polymorphic;
    template<class T> inline constexpr bool is_polymorphic_v = is_polymorphic<T>::value;
#if __has_intrinsics_for(is_abstract)
    using __internal::is_abstract;
    template<class T> inline constexpr bool is_abstract_v = is_abstract<T>::value;
#endif
#if __has_intrinsics_for(is_final)
    using __internal::is_final;
    template<class T> inline constexpr bool is_final_v = is_final<T>::value;
#endif
#if __has_intrinsics_for(is_aggregate)
    using __internal::is_aggregate;
    template<class T> inline constexpr bool is_aggregate_v = is_aggregate<T>::value;
#endif
    
    using __internal::is_signed;
    template<class T> inline constexpr bool is_signed_v = is_signed<T>::value;
    using __internal::is_unsigned;
    template<class T> inline constexpr bool is_unsigned_v = is_unsigned<T>::value;
    using __internal::is_bounded_array;
    template<class T> inline constexpr bool is_bounded_array_v = is_bounded_array<T>::value;
    using __internal::is_unbounded_array;
    template<class T> inline constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;

    using __internal::is_constructible;
    template<class T, class ...Args> inline constexpr bool is_constructible_v = is_constructible<T, Args...>::value;
    using __internal::is_default_constructible;
    template<class T> inline constexpr bool is_default_constructible_v = is_default_constructible<T>::value;
    using __internal::is_copy_constructible;
    template<class T> inline constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;
    using __internal::is_move_constructible;
    template<class T> inline constexpr bool is_move_constructible_v = is_move_constructible<T>::value;

    using __internal::is_assignable;
    template<class T, class U> inline constexpr bool is_assignable_v = is_assignable<T, U>::value;
    using __internal::is_copy_assignable;
    template<class T> inline constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;
    using __internal::is_move_assignable;
    template<class T> inline constexpr bool is_move_assignable_v = is_move_assignable<T>::value;

    using __internal::is_swappable_with;
    template<class T, class U> inline constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;
    using __internal::is_swappable;
    template<class T> inline constexpr bool is_swappable_v = is_swappable<T>::value;
    using __internal::is_destructible;
    template<class T> inline constexpr bool is_destructible_v = is_destructible<T>::value;

#if __has_intrinsics_for(is_trivially_constructible)
    using __internal::is_trivially_constructible;
    template<class T, class ...Args> inline constexpr bool is_trivially_constructible_v = is_trivially_constructible<T, Args...>::value;
#endif
#if __has_intrinsics_for(is_trivially_constructible)
    using __internal::is_trivially_default_constructible;
    template<class T> inline constexpr bool is_trivially_default_constructible_v = is_trivially_default_constructible<T>::value;
#endif
#if __has_intrinsics_for(is_trivially_constructible)
    using __internal::is_trivially_copy_constructible;
    template<class T> inline constexpr bool is_trivially_copy_constructible_v = is_trivially_copy_constructible<T>::value;
#endif
#if __has_intrinsics_for(is_trivially_constructible)
    using __internal::is_trivially_move_constructible;
    template<class T> inline constexpr bool is_trivially_move_constructible_v = is_trivially_move_constructible<T>::value;
#endif

#if __has_intrinsics_for(is_trivially_assignable)
    using __internal::is_trivially_assignable;
    template<class T, class U> inline constexpr bool is_trivially_assignable_v = is_trivially_assignable<T, U>::value;
#endif
#if __has_intrinsics_for(is_trivially_assignable)
    using __internal::is_trivially_copy_assignable;
    template<class T> inline constexpr bool is_trivially_copy_assignable_v = is_trivially_copy_assignable<T>::value;
#endif
#if __has_intrinsics_for(is_trivially_assignable)
    using __internal::is_trivially_move_assignable;
    template<class T> inline constexpr bool is_trivially_move_assignable_v = is_trivially_move_assignable<T>::value;
#endif

#if __has_intrinsics_for(is_trivially_destructible) || __has_intrinsics_for(has_trivial_destructor)
    using __internal::is_trivially_destructible;
    template<class T> inline constexpr bool is_trivially_destructible_v = is_trivially_destructible<T>::value;
#endif

    using __internal::is_nothrow_constructible;
    template<class T, class ...Args> inline constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<T, Args...>::value;
    using __internal::is_nothrow_default_constructible;
    template<class T> inline constexpr bool is_nothrow_default_constructible_v = is_nothrow_default_constructible<T>::value;
    using __internal::is_nothrow_copy_constructible;
    template<class T> inline constexpr bool is_nothrow_copy_constructible_v = is_nothrow_copy_constructible<T>::value;
    using __internal::is_nothrow_move_constructible;
    template<class T> inline constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;

    using __internal::is_nothrow_assignable;
    template<class T, class U> inline constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<T, U>::value;
    using __internal::is_nothrow_copy_assignable;
    template<class T> inline constexpr bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<T>::value;
    using __internal::is_nothrow_move_assignable;
    template<class T> inline constexpr bool is_nothrow_move_assignable_v = is_nothrow_move_assignable<T>::value;

    using __internal::is_nothrow_swappable_with;
    template<class T, class U> inline constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<T, U>::value;
    using __internal::is_nothrow_swappable;
    template<class T> inline constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;

    using __internal::is_nothrow_destructible;
    template<class T> inline constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<T>::value;

#if __has_intrinsics_for(has_virtual_destructor)
    using __internal::has_virtual_destructor;
    template<class T> inline constexpr bool has_virtual_destructor_v = has_virtual_destructor<T>::value;
#endif

#if __has_intrinsics_for(has_unique_object_representations)
    using __internal::has_unique_object_representations;
    template<class T> inline constexpr bool has_unique_object_representations_v = has_unique_object_representations<T>::value;
#endif

    /* 20.15.6 Type property queries */
    using __internal::alignment_of;
    template<class T> inline constexpr bool alignment_of_v = alignment_of<T>::value;
    using __internal::rank;
    template<class T> inline constexpr bool rank_v = rank<T>::value;
    using __internal::extent;
    template<class T, unsigned I = 0> inline constexpr bool extent_v = extent<T, I>::value;

    /* 20.15.7 Type relations */
    using __internal::is_same;
    template<class T, class U> inline constexpr bool is_same_v = is_same<T, U>::value;
    using __internal::is_base_of;
    template<class Base, class Derived> inline constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;
    using __internal::is_convertible;
    template<class From, class To> inline constexpr bool is_convertible_v = is_convertible<From, To>::value;
    using __internal::is_nothrow_convertible;
    template<class From, class To> inline constexpr bool is_nothrow_convertible_v = is_nothrow_convertible<From, To>::value;
#if __has_intrinsics_for(is_layout_compatible)
    using __internal::is_layout_compatible;
    template<class T, class U> inline constexpr bool is_layout_compatible_v = is_layout_compatible<T, U>::value;
#endif

#if __has_intrinsics_for(is_pointer_interconvertible_base_of)
    using __internal::is_pointer_interconvertible_base_of;
    template<class Base, class Derived> inline constexpr bool is_pointer_interconvertible_base_of_v = is_pointer_interconvertible_base_of<Base, Derived>::value;
#endif

    using __internal::is_invocable;
    template<class Fn, class ...ArgTypes> inline constexpr bool is_invocable_v = is_invocable<Fn, ArgTypes...>::value;
    using __internal::is_invocable_r;
    template<class R, class Fn, class ...ArgTypes> inline constexpr bool is_invocable_r_v = is_invocable_r<R, Fn, ArgTypes...>::value;
    using __internal::is_nothrow_invocable;
    template<class Fn, class ...ArgTypes> inline constexpr bool is_nothrow_invocable_v = is_nothrow_invocable<Fn, ArgTypes...>::value;
    using __internal::is_nothrow_invocable_r;
    template<class R, class Fn, class ...ArgTypes> inline constexpr bool is_nothrow_invocable_r_v = is_nothrow_invocable_r<R, Fn, ArgTypes...>::value;

    /* 20.15.8.2 Const-volatile modifications */
    using __internal::remove_const;
    template<class T> using remove_const_t = typename remove_const<T>::type;
    using __internal::remove_volatile;
    template<class T> using remove_volatile_t = typename remove_volatile<T>::type;
    using __internal::remove_cv;
    template<class T> using remove_cv_t = typename remove_cv<T>::type;
    using __internal::add_const;
    template<class T> using add_const_t = typename add_const<T>::type;
    using __internal::add_volatile;
    template<class T> using add_volatile_t = typename add_volatile<T>::type;
    using __internal::add_cv;
    template<class T> using add_cv_t = typename add_cv<T>::type;

    /* 20.15.8.3 Reference modifications */
    using __internal::remove_reference;
    template<class T> using remove_reference_t = typename remove_reference<T>::type;
    using __internal::add_lvalue_reference;
    template<class T> using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;
    using __internal::add_rvalue_reference;
    template<class T> using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

    /* 20.15.8.4 Sign modifications */
#if __has_intrinsics_for(is_enum)
    using __internal::make_signed;
    template<class T> using make_signed_t = typename make_signed<T>::type;
#endif
#if __has_intrinsics_for(is_enum)
    using __internal::make_unsigned;
    template<class T> using make_unsigned_t = typename make_unsigned<T>::type;
#endif

    /* 20.15.8.5 Array modifications */
    using __internal::remove_extent;
    template<class T> using remove_extent_t = typename remove_extent<T>::type;
    using __internal::remove_all_extents;
    template<class T> using remove_all_extents_t = typename remove_all_extents<T>::type;

    /* 20.15.8.6 Pointer modifications */
    using __internal::remove_pointer;
    template<class T> using remove_pointer_t = typename remove_pointer<T>::type;
    using __internal::add_pointer;
    template<class T> using add_pointer_t = typename add_pointer<T>::type;

    /* 20.15.8.7 Other transformations */
    using __internal::type_identity;
    template<class T> using type_identity_t = typename type_identity<T>::type;
    using __internal::aligned_storage;
    template<size_t Len, size_t Align = __internal::__default_alignment<Len>()>
    using aligned_storage_t = typename aligned_storage<Len, Align>::type;
    using __internal::aligned_union;
    template<size_t Len, class ...Types> using aligned_union_t = typename aligned_union<Len, Types...>::type;
    using __internal::remove_cvref;
    template<class T> using remove_cvref_t = typename remove_cvref<T>::type;
    using __internal::decay;
    template<class T> using decay_t = typename decay<T>::type;
    using __internal::enable_if;
    template<bool B, class T = void> using enable_if_t = typename enable_if<B, T>::type;
    using __internal::conditional;
    template<bool B, class T, class F> using conditional_t = typename conditional<B, T, F>::type;
    using __internal::common_type;
    template<class ...T> using common_type_t = typename common_type<T...>::type;
    using __internal::basic_common_reference;
    using __internal::common_reference;
    template<class ...T> using common_reference_t = typename common_reference<T...>::type;
#if __has_intrinsics_for(underlying_type) && __has_intrinsics_for(is_enum)
    using __internal::underlying_type;
    template<class T> using underlying_type_t = typename underlying_type<T>::type;
#endif
    using __internal::invoke_result;
    template<class Fn, class ...ArgTypes> using invoke_result_t = typename invoke_result<Fn, ArgTypes...>::type;
    using __internal::unwrap_reference;
    template<class T> using unwrap_reference_t = typename unwrap_reference<T>::type;
    using __internal::unwrap_ref_decay;
    template<class T> using unwrap_ref_decay_t = typename unwrap_ref_decay<T>::type;
    using __internal::void_t;

    /* 20.15.9 Logical operator traits */
    using __internal::conjunction;
    template<class ...B> inline constexpr bool conjunction_v = conjunction<B...>::value;
    using __internal::disjunction;
    template<class ...B> inline constexpr bool disjunction_v = disjunction<B...>::value;
    using __internal::negation;
    template<class B> inline constexpr bool negation_v = negation<B>::value;

    /* 20.15.10 Member relationships */
#if __has_intrinsics_for(is_pointer_interconvertible_with_class)
    using __internal::is_pointer_interconvertible_with_class;
#endif

#if __has_intrinsics_for(is_corresponding_member)
    using __internal::is_corresponding_member;
#endif

    /* 20.15.11 Constant evaluation context */
#if __has_intrinsics_for(builtin_is_constant_evaluated)
    using __internal::is_constant_evaluated;
#endif
}