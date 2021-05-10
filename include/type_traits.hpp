#pragma once

#include "__type_traits.hpp"
#include "util/macros.hpp"

namespace yilib {
    /* 20.15.4 Helper class */
    template<class T, T v> struct integral_constant : __internal::integral_constant<T, v> {};
    template<bool B> struct bool_constant : __internal::bool_constant<B> {};
    struct true_type : __internal::true_type {};
    struct false_type : __internal::false_type {};

    /* 20.15.5.2 Primary type categories */
    template<class T> struct is_void : __internal::is_void<T> {};
    template<class T> inline constexpr bool is_void_v = is_void<T>::value;
    template<class T> struct is_null_pointer : __internal::is_null_pointer<T> {};
    template<class T> inline constexpr bool is_null_pointer_v = is_null_pointer<T>::value;
    template<class T> struct is_integral : __internal::is_integral<T> {};
    template<class T> inline constexpr bool is_integral_v = is_integral<T>::value;
    template<class T> struct is_floating_point : __internal::is_floating_point<T> {};
    template<class T> inline constexpr bool is_floating_point_v = is_floating_point<T>::value;
    template<class T> struct is_array : __internal::is_array<T> {};
    template<class T> inline constexpr bool is_array_v = is_array<T>::value;
    template<class T> struct is_pointer : __internal::is_pointer<T> {};
    template<class T> inline constexpr bool is_pointer_v = is_pointer<T>::value;
    template<class T> struct is_lvalue_reference : __internal::is_lvalue_reference<T> {};
    template<class T> inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;
    template<class T> struct is_rvalue_reference : __internal::is_rvalue_reference<T> {};
    template<class T> inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;
    template<class T> struct is_member_object_pointer : __internal::is_member_object_pointer<T> {};
    template<class T> inline constexpr bool is_member_object_pointer_v = is_member_object_pointer<T>::value;
    template<class T> struct is_member_function_pointer : __internal::is_member_function_pointer<T> {};
    template<class T> inline constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;
#if __has_intrinsics_for(is_enum)
    template<class T> struct is_enum : __internal::is_enum<T> {};
    template<class T> inline constexpr bool is_enum_v = is_enum<T>::value;
#endif
#if __has_intrinsics_for(is_union)
    template<class T> struct is_union : __internal::is_union<T> {};
    template<class T> inline constexpr bool is_union_v = is_union<T>::value;
#endif
    template<class T> struct is_class : __internal::is_class<T> {};
    template<class T> inline constexpr bool is_class_v = is_class<T>::value;
    template<class T> struct is_function : __internal::is_function<T> {};
    template<class T> inline constexpr bool is_function_v = is_function<T>::value;

    /* 20.15.5.3 Composite type categories */
    template<class T> struct is_reference : __internal::is_reference<T> {};
    template<class T> inline constexpr bool is_reference_v = is_reference<T>::value;
    template<class T> struct is_arithmetic : __internal::is_arithmetic<T> {};
    template<class T> inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;
    template<class T> struct is_fundamental : __internal::is_fundamental<T> {};
    template<class T> inline constexpr bool is_fundamental_v = is_fundamental<T>::value;
    template<class T> struct is_object : __internal::is_object<T> {};
    template<class T> inline constexpr bool is_object_v = is_object<T>::value;
    template<class T> struct is_scalar : __internal::is_scalar<T> {};
    template<class T> inline constexpr bool is_scalar_v = is_scalar<T>::value;
    template<class T> struct is_compound : __internal::is_compound<T> {};
    template<class T> inline constexpr bool is_compound_v = is_compound<T>::value;
    template<class T> struct is_member_pointer : __internal::is_member_pointer<T> {};
    template<class T> inline constexpr bool is_member_pointer_v = is_member_pointer<T>::value;

    /* 20.15.5.4 Type properties */
    template<class T> struct is_const : __internal::is_const<T> {};
    template<class T> inline constexpr bool is_const_v = is_const<T>::value;
    template<class T> struct is_volatile : __internal::is_volatile<T> {};
    template<class T> inline constexpr bool is_volatile_v = is_volatile<T>::value;
#if __has_intrinsics_for(is_trivally_constructible) && __has_intrinsics_for(is_trivially_copyable)
    template<class T> struct is_trivial : __internal::is_trivial<T> {};
    template<class T> inline constexpr bool is_trivial_v = is_trivial<T>::value;
#endif
#if __has_intrinsics_for(is_trivially_copyable)
    template<class T> struct is_trivially_copyable : __internal::is_trivially_copyable<T> {};
    template<class T> inline constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;
#endif
#if __has_intrinsics_for(is_standard_layout)
    template<class T> struct is_standard_layout : __internal::is_standard_layout<T> {};
    template<class T> inline constexpr bool is_standard_layout_v = is_standard_layout<T>::value;
#endif
#if __has_intrinsics_for(is_empty)
    template<class T> struct is_empty : __internal::is_empty<T> {};
    template<class T> inline constexpr bool is_empty_v = is_empty<T>::value;
#endif
    template<class T> struct is_polymorphic : __internal::is_polymorphic<T> {};
    template<class T> inline constexpr bool is_polymorphic_v = is_polymorphic<T>::value;
#if __has_intrinsics_for(is_abstract)
    template<class T> struct is_abstract : __internal::is_abstract<T> {};
    template<class T> inline constexpr bool is_abstract_v = is_abstract<T>::value;
#endif
#if __has_intrinsics_for(is_final)
    template<class T> struct is_final : __internal::is_final<T> {};
    template<class T> inline constexpr bool is_final_v = is_final<T>::value;
#endif
#if __has_intrinsics_for(is_aggregate)
    template<class T> struct is_aggregate : __internal::is_aggregate<T> {};
    template<class T> inline constexpr bool is_aggregate_v = is_aggregate<T>::value;
#endif
    
    template<class T> struct is_signed : __internal::is_signed<T> {};
    template<class T> inline constexpr bool is_signed_v = is_signed<T>::value;
    template<class T> struct is_unsigned : __internal::is_unsigned<T> {};
    template<class T> inline constexpr bool is_unsigned_v = is_unsigned<T>::value;
    template<class T> struct is_bounded_array : __internal::is_bounded_array<T> {};
    template<class T> inline constexpr bool is_bounded_array_v = is_bounded_array<T>::value;
    template<class T> struct is_unbounded_array : __internal::is_unbounded_array<T> {};
    template<class T> inline constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;

    template<class T, class ...Args> struct is_constructible : __internal::is_constructible<T, Args...> {};
    template<class T, class ...Args> inline constexpr bool is_constructible_v = is_constructible<T, Args...>::value;
    template<class T> struct is_default_constructible : __internal::is_default_constructible<T> {};
    template<class T> inline constexpr bool is_default_constructible_v = is_default_constructible<T>::value;
    template<class T> struct is_copy_constructible : __internal::is_copy_constructible<T> {};
    template<class T> inline constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;
    template<class T> struct is_move_constructible : __internal::is_move_constructible<T> {};
    template<class T> inline constexpr bool is_move_constructible_v = is_move_constructible<T>::value;

    template<class T, class U> struct is_assignable : __internal::is_assignable<T, U> {};
    template<class T, class U> inline constexpr bool is_assignable_v = is_assignable<T, U>::value;
    template<class T> struct is_copy_assignable : __internal::is_copy_assignable<T> {};
    template<class T> inline constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;
    template<class T> struct is_move_assignable : __internal::is_move_assignable<T> {};
    template<class T> inline constexpr bool is_move_assignable_v = is_move_assignable<T>::value;

    template<class T, class U> struct is_swappable_with : __internal::is_swappable_with<T, U> {};
    template<class T, class U> inline constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;
    template<class T> struct is_swappable : __internal::is_swappable<T> {};
    template<class T> inline constexpr bool is_swappable_v = is_swappable<T>::value;
    template<class T> struct is_destructible : __internal::is_destructible<T> {};
    template<class T> inline constexpr bool is_destructible_v = is_destructible<T>::value;

#if __has_intrinsics_for(is_trivially_constructible)
    template<class T, class ...Args> struct is_trivially_constructible : __internal::is_trivially_constructible<T, Args...> {};
    template<class T, class ...Args> inline constexpr bool is_trivially_constructible_v = is_trivially_constructible<T, Args...>::value;
#endif
#if __has_intrinsics_for(is_trivially_constructible)
    template<class T> struct is_trivially_default_constructible : __internal::is_trivially_default_constructible<T> {};
    template<class T> inline constexpr bool is_trivially_default_constructible_v = is_trivially_default_constructible<T>::value;
#endif
#if __has_intrinsics_for(is_trivially_constructible)
    template<class T> struct is_trivially_copy_constructible : __internal::is_trivially_copy_constructible<T> {};
    template<class T> inline constexpr bool is_trivially_copy_constructible_v = is_trivially_copy_constructible<T>::value;
#endif
#if __has_intrinsics_for(is_trivially_constructible)
    template<class T> struct is_trivially_move_constructible : __internal::is_trivially_move_constructible<T> {};
    template<class T> inline constexpr bool is_trivially_move_constructible_v = is_trivially_move_constructible<T>::value;
#endif

#if __has_intrinsics_for(is_trivially_assignable)
    template<class T, class U> struct is_trivially_assignable : __internal::is_trivially_assignable<T, U> {};
    template<class T, class U> inline constexpr bool is_trivially_assignable_v = is_trivially_assignable<T, U>::value;
#endif
#if __has_intrinsics_for(is_trivially_assignable)
    template<class T> struct is_trivially_copy_assignable : __internal::is_trivially_copy_assignable<T> {};
    template<class T> inline constexpr bool is_trivially_copy_assignable_v = is_trivially_copy_assignable<T>::value;
#endif
#if __has_intrinsics_for(is_trivially_assignable)
    template<class T> struct is_trivially_move_assignable : __internal::is_trivially_move_assignable<T> {};
    template<class T> inline constexpr bool is_trivially_move_assignable_v = is_trivially_move_assignable<T>::value;
#endif

#if __has_intrinsics_for(is_trivially_destructible)
    template<class T> struct is_trivially_destructible : __internal::is_trivially_destructible<T> {};
    template<class T> inline constexpr bool is_trivially_destructible_v = is_trivially_destructible<T>::value;
#endif

    template<class T, class ...Args> struct is_nothrow_constructible : __internal::is_nothrow_constructible<T, Args...> {};
    template<class T, class ...Args> inline constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<T, Args...>::value;
    template<class T> struct is_nothrow_default_constructible : __internal::is_nothrow_default_constructible<T> {};
    template<class T> inline constexpr bool is_nothrow_default_constructible_v = is_nothrow_default_constructible<T>::value;
    template<class T> struct is_nothrow_copy_constructible : __internal::is_nothrow_copy_constructible<T> {};
    template<class T> inline constexpr bool is_nothrow_copy_constructible_v = is_nothrow_copy_constructible<T>::value;
    template<class T> struct is_nothrow_move_constructible : __internal::is_nothrow_move_constructible<T> {};
    template<class T> inline constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;

    template<class T, class U> struct is_nothrow_assignable : __internal::is_nothrow_assignable<T, U> {};
    template<class T, class U> inline constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<T, U>::value;
    template<class T> struct is_nothrow_copy_assignable : __internal::is_nothrow_copy_assignable<T> {};
    template<class T> inline constexpr bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<T>::value;
    template<class T> struct is_nothrow_move_assignable : __internal::is_nothrow_move_assignable<T> {};
    template<class T> inline constexpr bool is_nothrow_move_assignable_v = is_nothrow_move_assignable<T>::value;

    template<class T, class U> struct is_nothrow_swappable_with : __internal::is_nothrow_swappable_with<T, U> {};
    template<class T, class U> inline constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<T, U>::value;
    template<class T> struct is_nothrow_swappable : __internal::is_nothrow_swappable<T> {};
    template<class T> inline constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;

    template<class T> struct is_nothrow_destructible : __internal::is_nothrow_destructible<T> {};
    template<class T> inline constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<T>::value;

#if __has_intrinsics_for(has_virtual_destructor)
    template<class T> struct has_virtual_destructor : __internal::has_virtual_destructor<T> {};
    template<class T> inline constexpr bool has_virtual_destructor_v = has_virtual_destructor<T>::value;
#endif

#if __has_intrinsics_for(has_unique_object_representations)
    template<class T> struct has_unique_object_representations : __internal::has_unique_object_representations<T> {};
    template<class T> inline constexpr bool has_unique_object_representations_v = has_unique_object_representations<T>::value;
#endif

    /* 20.15.6 Type property queries */
    template<class T> struct alignment_of : __internal::alignment_of<T> {};
    template<class T> inline constexpr bool alignment_of_v = alignment_of<T>::value;
    template<class T> struct rank : __internal::rank<T> {};
    template<class T> inline constexpr bool rank_v = rank<T>::value;
    template<class T, unsigned I = 0> struct extent : __internal::extent<T, I> {};
    template<class T, unsigned I = 0> inline constexpr bool extent_v = extent<T, I>::value;

    /* 20.15.7 Type relations */
    template<class T, class U> struct is_same : __internal::is_same<T, U> {};
    template<class T, class U> inline constexpr bool is_same_v = is_same<T, U>::value;
    template<class Base, class Derived> struct is_base_of : __internal::is_base_of<Base, Derived> {};
    template<class Base, class Derived> inline constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;
    template<class From, class To> struct is_convertible : __internal::is_convertible<From, To> {};
    template<class From, class To> inline constexpr bool is_convertible_v = is_convertible<From, To>::value;
    template<class From, class To> struct is_nothrow_convertible : __internal::is_nothrow_convertible<From, To> {};
    template<class From, class To> inline constexpr bool is_nothrow_convertible_v = is_nothrow_convertible<From, To>::value;
#if __has_intrinsics_for(is_layout_compatible)
    template<class T, class U> struct is_layout_compatible : __internal::is_layout_compatible<T, U> {};
    template<class T, class U> inline constexpr bool is_layout_compatible_v = is_layout_compatible<T, U>::value;
#endif

#if __has_intrinsics_for(is_pointer_interconvertible_base_of)
    template<class Base, class Derived> struct is_pointer_interconvertible_base_of : __internal::is_pointer_interconvertible_base_of<Base, Derived> {};
    template<class Base, class Derived> inline constexpr bool is_pointer_interconvertible_base_of_v = is_pointer_interconvertible_base_of<Base, Derived>::value;
#endif

    template<class Fn, class ...ArgTypes> struct is_invocable : __internal::is_invocable<Fn, ArgTypes...> {};
    template<class Fn, class ...ArgTypes> inline constexpr bool is_invocable_v = is_invocable<Fn, ArgTypes...>::value;
    template<class R, class Fn, class ...ArgTypes> struct is_invocable_r : __internal::is_invocable_r<R, Fn, ArgTypes...> {};
    template<class R, class Fn, class ...ArgTypes> inline constexpr bool is_invocable_r_v = is_invocable_r<R, Fn, ArgTypes...>::value;
    template<class Fn, class ...ArgTypes> struct is_nothrow_invocable : __internal::is_nothrow_invocable<Fn, ArgTypes...> {};
    template<class Fn, class ...ArgTypes> inline constexpr bool is_nothrow_invocable_v = is_nothrow_invocable<Fn, ArgTypes...>::value;
    template<class R, class Fn, class ...ArgTypes> struct is_nothrow_invocable_r : __internal::is_nothrow_invocable_r<R, Fn, ArgTypes...> {};
    template<class R, class Fn, class ...ArgTypes> inline constexpr bool is_nothrow_invocable_r_v = is_nothrow_invocable_r<R, Fn, ArgTypes...>::value;

    /* 20.15.8.2 Const-volatile modifications */
    template<class T> struct remove_const : __internal::remove_const<T> {};
    template<class T> using remove_const_t = typename remove_const<T>::type;
    template<class T> struct remove_volatile : __internal::remove_volatile<T> {};
    template<class T> using remove_volatile_t = typename remove_volatile<T>::type;
    template<class T> struct remove_cv : __internal::remove_cv<T> {};
    template<class T> using remove_cv_t = typename remove_cv<T>::type;
    template<class T> struct add_const : __internal::add_const<T> {};
    template<class T> using add_const_t = typename add_const<T>::type;
    template<class T> struct add_volatile : __internal::add_volatile<T> {};
    template<class T> using add_volatile_t = typename add_volatile<T>::type;
    template<class T> struct add_cv : __internal::add_cv<T> {};
    template<class T> using add_cv_t = typename add_cv<T>::type;

    /* 20.15.8.3 Reference modifications */
    template<class T> struct remove_reference : __internal::remove_reference<T> {};
    template<class T> using remove_reference_t = typename remove_reference<T>::type;
    template<class T> struct add_lvalue_reference : __internal::add_lvalue_reference<T> {};
    template<class T> using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;
    template<class T> struct add_rvalue_reference : __internal::add_rvalue_reference<T> {};
    template<class T> using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

    /* 20.15.8.4 Sign modifications */
#if __has_intrinsics_for(is_enum)
    template<class T> struct make_signed : __internal::make_signed<T> {};
    template<class T> using make_signed_t = typename make_signed<T>::type;
#endif
#if __has_intrinsics_for(is_enum)
    template<class T> struct make_unsigned : __internal::make_unsigned<T> {};
    template<class T> using make_unsigned_t = typename make_unsigned<T>::type;
#endif

    /* 20.15.8.5 Array modifications */
    template<class T> struct remove_extent : __internal::remove_extent<T> {};
    template<class T> using remove_extent_t = typename remove_extent<T>::type;
    template<class T> struct remove_all_extents : __internal::remove_all_extents<T> {};
    template<class T> using remove_all_extents_t = typename remove_all_extents<T>::type;

    /* 20.15.8.6 Pointer modifications */
    template<class T> struct remove_pointer : __internal::remove_pointer<T> {};
    template<class T> using remove_pointer_t = typename remove_pointer<T>::type;
    template<class T> struct add_pointer : __internal::add_pointer<T> {};
    template<class T> using add_pointer_t = typename add_pointer<T>::type;

    /* 20.15.8.7 Other transformations */
    template<class T> struct type_identity : __internal::type_identity<T> {};
    template<class T> using type_identity_t = typename type_identity<T>::type;
    template<size_t Len, size_t Align = __internal::__default_alignment<Len>()>
    struct aligned_storage : __internal::aligned_storage<Len, Align> {};
    template<size_t Len, size_t Align = __internal::__default_alignment<Len>()>
    using aligned_storage_t = typename aligned_storage<Len, Align>::type;
    template<size_t Len, class ...Types> struct aligned_union : __internal::aligned_union<Len, Types...> {};
    template<size_t Len, class ...Types> using aligned_union_t = typename aligned_union<Len, Types...>::type;
    template<class T> struct remove_cvref : __internal::remove_cvref<T> {};
    template<class T> using remove_cvref_t = typename remove_cvref<T>::type;
    template<class T> struct decay : __internal::decay<T> {};
    template<class T> using decay_t = typename decay<T>::type;
    template<bool B, class T = void> struct enable_if : __internal::enable_if<B, T> {};
    template<bool B, class T = void> using enable_if_t = typename enable_if<B, T>::type;
    template<bool B, class T, class F> struct conditional : __internal::conditional<B, T, F> {};
    template<bool B, class T, class F> using conditional_t = typename conditional<B, T, F>::type;
    template<class T, class U, template<class> class TQual, template<class> class UQual> struct basic_common_reference :
        __internal::basic_common_reference<T, U, TQual, UQual> {};
    template<class ...T> struct common_reference : __internal::common_reference<T...> {};
    template<class ...T> using common_reference_t = typename common_reference<T...>::type;
#if __has_intrinsics_for(underlying_type) && __has_intrinsics_for(is_enum)
    template<class T> struct underlying_type : __internal::underlying_type<T> {};
    template<class T> using underlying_type_t = typename underlying_type<T>::type;
#endif
    template<class Fn, class ...ArgTypes> struct invoke_result : __internal::invoke_result<Fn, ArgTypes...> {};
    template<class Fn, class ...ArgTypes> using invoke_result_t = typename invoke_result<Fn, ArgTypes...>::type;
    template<class T> struct unwrap_reference : __internal::unwrap_reference<T> {};
    template<class T> using unwrap_reference_t = typename unwrap_reference<T>::type;
    template<class T> struct unwrap_ref_decay : __internal::unwrap_ref_decay<T> {};
    template<class T> using unwrap_ref_decay_t = typename unwrap_ref_decay<T>::type;
    using __internal::void_t;

    /* 20.15.9 Logical operator traits */
    template<class ...B> struct conjunction : __internal::conjunction<B...> {};
    template<class ...B> inline constexpr bool conjunction_v = conjunction<B...>::value;
    template<class ...B> struct disjunction : __internal::disjunction<B...> {};
    template<class ...B> inline constexpr bool disjunction_v = disjunction<B...>::value;
    template<class B> struct negation : __internal::negation<B> {};
    template<class B> inline constexpr bool negation_v = negation<B>::value;

    /* 20.15.10 Member relationships */
#if __has_intrinsics_for(is_pointer_interconvertible_with_class)
        template<class S, class M>
        constexpr bool is_pointer_interconvertible_with_class(M S::* mp) noexcept {
            return __internal::is_pointer_interconvertible_with_class(mp);
        }
#endif

#if __has_intrinsics_for(is_corresponding_member)
        template<class S1, class S2, class M1, class M2>
        constexpr bool is_corresponding_member(M1 S1::* mp, M2 S2::* mq) noexcept {
            return __internal::is_corresponding_member(mp, mq);
        }
#endif

    /* 20.15.11 Constant evaluation context */
#if __has_intrinsics_for(builtin_is_constant_evaluated)
    constexpr bool is_constant_evaluated() noexcept { return __internal::is_constant_evaluated(); }
#endif
}