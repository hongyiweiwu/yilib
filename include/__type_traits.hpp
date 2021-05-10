#pragma once

#include "cstdlib.hpp"
#include "util/nullptr_t.hpp"
#include "util/max_align_t.hpp"
#include "utility/declval.hpp"

// TODO: This currently only supports Clang++. GCC and MSVC support is being investigated.
#define __has_intrinsics_for(test) __has_builtin(__##test) || !__is_identifier(__##test) || __has_feature(test)

namespace yilib {
    // Forward declaration of reference_wrapper, declared in "functional.hpp".
    template<class T> class reference_wrapper;

    namespace __internal {
        template<class T, T v> struct integral_constant {
            static constexpr T value = v;
            using value_type = T;
            using type = integral_constant;
            constexpr operator value_type() const noexcept { return value; }
            constexpr value_type operator()() const noexcept { return value; }
        };

        template<bool B> using bool_constant = integral_constant<bool, B>;
        using true_type = bool_constant<true>;
        using false_type = bool_constant<false>;

        template<class T, class = size_t> struct __is_complete : false_type {};
        template<class T> struct __is_complete<T, decltype(sizeof(T))> : true_type {};

        template<class T> struct remove_const { using type = T; };
        template<class T> struct remove_const<const T> { using type = T; };

        template<class T> struct remove_volatile { using type = T; };
        template<class T> struct remove_volatile<volatile T> { using type = T; };

        template<class T> struct remove_cv { using type = T; };
        template<class T> struct remove_cv<const T> { using type = T; };
        template<class T> struct remove_cv<volatile T> { using type = T; };
        template<class T> struct remove_cv<const volatile T> { using type = T; };

        template<class T> struct add_const { using type = const T; };
        template<class T> struct add_volatile { using type = volatile T; };
        template<class T> struct add_cv { using type = const volatile T; };

        template<bool B, class T, class F> struct conditional { using type = T; };
        template<class T, class F> struct conditional<false, T, F> { using type = F; };

        template<class T, class U> struct is_same: false_type {};
        template<class T> struct is_same<T, T>: true_type {};

        template<class T, class U> concept same_as = is_same<T, U>::value;

        template<class B> struct negation   
            : bool_constant<!bool(B::value)>{};

        template<class ...Bn> struct conjunction : true_type {};
        template<class B> struct conjunction<B> : B {};
        template<class B1, class ...Bn> struct conjunction<B1, Bn...>
            : conditional<bool(B1::value), conjunction<Bn...>, B1>::type {};

        template<class ...B> struct disjunction : false_type {};
        template<class B> struct disjunction<B> : B {};
        template<class B1, class ...Bn> struct disjunction<B1, Bn...>
            : conditional<bool(B1::value), B1, disjunction<Bn...>>::type {};

        template<class T> struct is_void
            : bool_constant<is_same<typename remove_cv<T>::type, void>::value> {};

        template<class T> struct is_null_pointer
            : bool_constant<is_same<typename remove_cv<T>::type, nullptr_t>::value> {};

        template<class T> struct is_integral
            : bool_constant<disjunction<
                is_same<typename remove_cv<T>::type, bool>, 
                is_same<typename remove_cv<T>::type, char>, 
                is_same<typename remove_cv<T>::type, signed char>, 
                is_same<typename remove_cv<T>::type, unsigned char>, 
                is_same<typename remove_cv<T>::type, wchar_t>, 
                is_same<typename remove_cv<T>::type, char8_t>, 
                is_same<typename remove_cv<T>::type, char16_t>, 
                is_same<typename remove_cv<T>::type, char32_t>, 
                is_same<typename remove_cv<T>::type, short>, 
                is_same<typename remove_cv<T>::type, unsigned short>, 
                is_same<typename remove_cv<T>::type, int>, 
                is_same<typename remove_cv<T>::type, unsigned int>, 
                is_same<typename remove_cv<T>::type, long>, 
                is_same<typename remove_cv<T>::type, unsigned long>, 
                is_same<typename remove_cv<T>::type, long long>, 
                is_same<typename remove_cv<T>::type, unsigned long long>
            >::value> {};

        template<class T> concept integral = is_integral<T>::value;

        template<class T> struct is_floating_point
            : bool_constant<disjunction<
                is_same<typename remove_cv<T>::type, float>,
                is_same<typename remove_cv<T>::type, double>,
                is_same<typename remove_cv<T>::type, long double>
            >::value> {};

        template<class T> struct __is_unbounded_array : false_type {};
        template<class T> struct __is_unbounded_array<T[]> : true_type {};
        template<class T> using is_unbounded_array = __is_unbounded_array<typename remove_cv<T>::type>;

        template<class T> struct __is_bounded_array : false_type {};
        template<class T, size_t N> struct __is_bounded_array<T[N]> : true_type {};
        template<class T> using is_bounded_array = __is_bounded_array<typename remove_cv<T>::type>;

        template<class T> struct is_array : bool_constant<
            disjunction<is_unbounded_array<T>, is_bounded_array<T>>::value> {};

        template<class T> struct __is_pointer_impl : false_type {};
        template<class T> struct __is_pointer_impl<T*> : true_type {};
        template<class T> using is_pointer = __is_pointer_impl<typename remove_cv<T>::type>;

        template<class T> struct __is_lvalue_reference_impl : false_type {};
        template<class T> struct __is_lvalue_reference_impl<T&> : true_type {};
        template<class T> using is_lvalue_reference = __is_lvalue_reference_impl<typename remove_cv<T>::type>;

        template<class T> struct __is_rvalue_reference_impl : false_type {};
        template<class T> struct __is_rvalue_reference_impl<T&&> : true_type {};
        template<class T> using is_rvalue_reference = __is_rvalue_reference_impl<typename remove_cv<T>::type>;

        template<class T> struct __is_member_object_pointer__impl : false_type {};
        template<class T, class U> struct __is_member_object_pointer__impl<T U::*> : true_type {};
        template<class T> using is_member_object_pointer = __is_member_object_pointer__impl<typename remove_cv<T>::type>;

#if __has_intrinsics_for(is_enum)
        template<class T> struct is_enum : bool_constant<__is_enum(T)> {};
        
        template<class T> concept enumeration = is_enum<T>::value;
#endif

#if __has_intrinsics_for(is_union)
        template<class T> struct is_union : bool_constant<__is_union(T)> {};
#endif

        template<class T, bool IsUnion = is_union<T>::value, typename Dummy = int T::*> struct __is_class__impl : false_type {};
        template<class T> struct __is_class__impl<T, false> : true_type {};
        template<class T> using is_class = __is_class__impl<typename remove_cv<T>::type>;

        template<class T> using is_reference = disjunction<is_lvalue_reference<T>, is_rvalue_reference<T>>;
        
        template<class T> struct is_const : false_type {};
        template<class T> struct is_const<const T> : true_type {};

        template<class T> using is_function = conjunction<
            negation<is_const<const T>>,
            negation<is_reference<T>>
        >;

        template<class T> struct __is_member_function_pointer_impl : false_type {};
        template<class T, class U> struct __is_member_function_pointer_impl<U T::*> : is_function<U> {};
        template<class T> using is_member_function_pointer = __is_member_function_pointer_impl<typename remove_cv<T>::type>;

        template<class T> struct is_arithmetic : disjunction<is_integral<T>, is_floating_point<T>> {};

        template<class T> struct is_fundamental : disjunction<is_arithmetic<T>, is_void<T>, is_null_pointer<T>> {};

        template<class T> struct is_object : conjunction<
            negation<is_function<T>>,
            negation<is_reference<T>>,
            negation<is_void<T>>
        > {};

        template<class T> struct is_member_pointer : disjunction<is_member_function_pointer<T>, is_member_object_pointer<T>> {};

        template<class T> using is_scalar = disjunction<
            is_arithmetic<T>,
            is_pointer<T>,
            is_member_pointer<T>,
            is_enum<T>,
            is_null_pointer<T>
        >;

        template<class T> using is_compound = negation<is_fundamental<T>>;

        template<class T> struct is_volatile : false_type {};
        template<class T> struct is_volatile<volatile T> : true_type {};

#if __has_intrinsics_for(is_trivially_copyable)
        template<class T> struct is_trivially_copyable
            : bool_constant<__is_trivially_copyable(T)> {};
#endif

#if __has_intrinsics_for(is_standard_layout)
        template<class T> struct is_standard_layout
            : bool_constant<__is_standard_layout(T)> {};
#endif

#if __has_intrinsics_for(is_empty)
        template<class T> struct is_empty
            : bool_constant<__is_empty(T)> {};
#endif

        template<class T, class U = void*> struct __is_polymorphic_impl : false_type {};
        template<class T> struct __is_polymorphic_impl<T, decltype(dynamic_cast<void*>(static_cast<T*>(nullptr)))> : true_type {};

        template<class T> using is_polymorphic = __is_polymorphic_impl<typename remove_cv<T>::type>;

#if __has_intrinsics_for(is_abstract)
        template<class T> struct is_abstract
            : bool_constant<__is_abstract(T)> {};
#endif

#if __has_intrinsics_for(is_final)
        template<class T> struct is_final
            : bool_constant<__is_final(T)> {};
#endif

#if __has_intrinsics_for(is_aggregate)
        template<class T> struct is_aggregate
            : bool_constant<__is_aggregate(T)> {};
#endif

        template<class T> struct is_signed
            : bool_constant<is_arithmetic<T>::value && (T(-1) < T(0))> {};

        template<class T> struct is_unsigned
            : bool_constant<is_arithmetic<T>::value && (T(-1) > T(0))> {};

        template<class T> struct remove_reference { using type = T; };
        template<class T> struct remove_reference<T&> { using type = T; };
        template<class T> struct remove_reference<T&&> { using type = T; };

        template<class T> auto __is_referenceable(int) -> T&;
        template<class T> auto __is_referenceable(...) -> void;
        template<class T> struct is_referenceable :
            bool_constant<!is_same<decltype(__is_referenceable<T>(0)), void>::value> {};

        template<class T, bool Referenceable = is_referenceable<T>::value> struct __add_lvalue_reference { using type = T&; };
        template<class T> struct __add_lvalue_reference<T, false> { using type = T; };
        template<class T> struct add_lvalue_reference : __add_lvalue_reference<T> {};

        template<class T, bool Referenceable = is_referenceable<T>::value> struct __add_rvalue_reference { using type = T&&; };
        template<class T> struct __add_rvalue_reference<T, false> { using type = T; };
        template<class T> struct add_rvalue_reference : __add_rvalue_reference<T> {};

        template<class T, class _Dummy, class ...Args> struct __is_constructible_impl : false_type {};
        template<class T, class ...Args> struct __is_constructible_impl<T, decltype(::new T(declval<Args>()...)), Args...> : true_type {};
        template<class T, class ...Args> struct is_constructible
            : __is_constructible_impl<T, T*, Args...> {};

        template<class T> struct is_default_constructible 
            : is_constructible<T> {};
        
        template<bool B, class T = void> struct enable_if {};
        template<class T> struct enable_if<true, T> { using type = T; };

        template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_copy_constructible 
            : bool_constant<is_constructible<T, const T&>::value> {};
        template<class T> struct __is_copy_constructible<T, false> : false_type {};
        template<class T> struct is_copy_constructible : __is_copy_constructible<T> {};

        template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_move_constructible 
            : bool_constant<is_constructible<T, T&&>::value> {};
        template<class T> struct __is_move_constructible<T, false> : false_type {};
        template<class T> struct is_move_constructible : __is_move_constructible<T> {};

        template<class ...> using void_t = void; 

        template<class T, class U, class _Dummy> struct __is_assignable_impl : false_type {};
        template<class T, class U> struct __is_assignable_impl<T, U, void_t<decltype(declval<T>() = declval<U>())>> : true_type {};
        template<class T, class U> struct is_assignable : __is_assignable_impl<T, U, void> {};
        
        template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_copy_assignable 
            : bool_constant<is_assignable<T, const T&>::value> {};
        template<class T> struct __is_copy_assignable<T, false> : false_type {};
        template<class T> struct is_copy_assignable : __is_copy_assignable<T> {};

        template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_move_assignable 
            : bool_constant<is_assignable<T, T&&>::value> {};
        template<class T> struct __is_move_assignable<T, false> : false_type {};
        template<class T> struct is_move_assignable : __is_move_assignable<T> {};

        template<class T, class U, class __Dummy1, class __Dummy2> struct __is_swappable_with : false_type {};
        template<class T, class U> struct __is_swappable_with<T, U, decltype(swap(declval<T>(), declval<U>())), decltype(swap(declval<U>(), declval<T>()))> : true_type {};
        template<class T, class U> struct is_swappable_with : __is_swappable_with<T, U, void, void> {};

        template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_swappable : bool_constant<is_swappable_with<T&, T&>::value> {};
        template<class T> struct __is_swappable<T, false> : false_type {};
        template<class T> struct is_swappable : __is_swappable<T> {};

        template<class T> struct remove_extent { using type = T; };
        template<class T> struct remove_extent<T[]> { using type = T; };
        template<class T, size_t N> struct remove_extent<T[N]> { using type = T; };

        template<class T> struct remove_all_extents { using type = T; };
        template<class T> struct remove_all_extents<T[]> { using type = remove_all_extents<T>; };
        template<class T, size_t N> struct remove_all_extents<T[N]> { using type = remove_all_extents<T>; };

        template<class T, class __Dummy = void> struct __is_destructible : false_type {};
        template<class T> struct __is_destructible<T&> : true_type {};
        template<class T> struct __is_destructible<T&&> : true_type {};
        template<class T> struct __is_destructible<T, decltype(declval<T&>().~T())> : true_type {};
        template<class T> struct is_destructible : __is_destructible<typename remove_all_extents<T>::type> {};

        template<class T> struct alignment_of : integral_constant<size_t, alignof(T)> {};
        
        template<class T> struct rank : integral_constant<size_t, 0> {};
        template<class T, size_t N> struct rank<T[N]> : integral_constant<size_t, 1 + rank<T>::value> {};
        template<class T> struct rank<T[]> : integral_constant<size_t, 1 + rank<T>::value> {};

        template<class T, unsigned int I = 0> struct extent : integral_constant<size_t, 0> {};
        template<class T, size_t N> struct extent<T[N], 0> : integral_constant<size_t, N> {};
        template<class T, unsigned int I, size_t N> struct extent<T[N], I> : extent<T, I - 1> {};
        template<class T> struct extent<T[], 0> : integral_constant<size_t, 0> {};
        template<class T, unsigned int I> struct extent<T[], I> : extent<T, I - 1> {};

#if __has_intrinsics_for(is_trivially_constructible)
        template<class T, class ...Args> struct is_trivially_constructible
            : bool_constant<__is_trivially_constructible(T, Args...)> {};
#endif

#if __has_intrinsics_for(is_trivially_constructible)
        template<class T> struct is_trivially_default_constructible 
            : is_trivially_constructible<T> {};
#endif

#if __has_intrinsics_for(is_trivally_constructible) && __has_intrinsics_for(is_trivially_copyable)
        template<class T> struct __is_trivial_impl
            : bool_constant<is_scalar<T>::value || (is_trivially_copyable<T>::value && is_trivially_default_constructible<T>::value)> {};
        template<class T> struct is_trivial
            : __is_trivial_impl<typename remove_all_extents<typename remove_cv<T>::type>::type> {};
#endif

#if __has_intrinsics_for(is_trivially_constructible)
        template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_trivially_copy_constructible 
            : bool_constant<is_trivially_constructible<T, const T&>::value> {};
        template<class T> struct __is_trivially_copy_constructible<T, false> : false_type {};
        template<class T> struct is_trivially_copy_constructible : __is_trivially_copy_constructible<T> {};
#endif

#if __has_intrinsics_for(is_trivially_constructible)
        template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_trivially_move_constructible 
            : bool_constant<is_trivially_constructible<T, T&&>::value> {};
        template<class T> struct __is_trivially_move_constructible<T, false> : false_type {};
        template<class T> struct is_trivially_move_constructible : __is_trivially_move_constructible<T> {};
#endif

#if __has_intrinsics_for(is_trivially_assignable)
        template<class T, class U> struct is_trivially_assignable
            : bool_constant<__is_trivially_assignable(T, U)> {};
#endif

#if __has_intrinsics_for(is_trivially_assignable)
        template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_trivially_copy_assignable 
            : bool_constant<is_trivially_assignable<T, const T&>::value> {};
        template<class T> struct __is_trivially_copy_assignable<T, false> : false_type {};
        template<class T> struct is_trivially_copy_assignable : __is_trivially_copy_assignable<T> {};
#endif

#if __has_intrinsics_for(is_trivially_assignable)
        template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_trivially_move_assignable 
            : bool_constant<is_trivially_assignable<T, T&&>::value> {};
        template<class T> struct __is_trivially_move_assignable<T, false> : false_type {};
        template<class T> struct is_trivially_move_assignable : __is_trivially_move_assignable<T> {};
#endif

#if __has_intrinsics_for(is_trivially_destructible)
        template<class T> struct is_trivially_destructible
            : bool_constant<__is_trivially_destructible(T)> {};
#endif

        template<class T, class __Dummy, class ...Args> struct __is_nothrow_constructible_impl : false_type {};
        template<class T, class ...Args> struct __is_nothrow_constructible_impl<T, decltype(noexcept(::new T(declval<Args>()...))), Args...> : true_type {};
        template<class T, class ...Args> struct is_nothrow_constructible : __is_nothrow_constructible_impl<T, bool, Args...> {};

        template<class T> struct is_nothrow_default_constructible 
            : is_nothrow_constructible<T> {};

        template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_nothrow_copy_constructible 
            : bool_constant<is_nothrow_constructible<T, const T&>::value> {};
        template<class T> struct __is_nothrow_copy_constructible<T, false> : false_type {};
        template<class T> struct is_nothrow_copy_constructible : __is_nothrow_copy_constructible<T> {};

        template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_nothrow_move_constructible 
            : bool_constant<is_nothrow_constructible<T, T&&>::value> {};
        template<class T> struct __is_nothrow_move_constructible<T, false> : false_type {};
        template<class T> struct is_nothrow_move_constructible : __is_nothrow_move_constructible<T> {};

        template<class T, class U, class _Dummy> struct __is_nothrow_assignable_impl : false_type {};
        template<class T, class U> struct __is_nothrow_assignable_impl<T, U, decltype(noexcept(declval<T>() = declval<U>()))> : true_type {};
        template<class T, class U> struct is_nothrow_assignable : __is_nothrow_assignable_impl<T, U, bool> {};
        
        template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_nothrow_copy_assignable 
            : bool_constant<is_nothrow_assignable<T, const T&>::value> {};
        template<class T> struct __is_nothrow_copy_assignable<T, false> : false_type {};
        template<class T> struct is_nothrow_copy_assignable : __is_nothrow_copy_assignable<T> {};

        template<class T, bool Referenceable = is_referenceable<T>::value> struct __is_nothrow_move_assignable 
            : bool_constant<is_nothrow_assignable<T, T&&>::value> {};
        template<class T> struct __is_nothrow_move_assignable<T, false> : false_type {};
        template<class T> struct is_nothrow_move_assignable : __is_nothrow_move_assignable<T> {};

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

#if __has_intrinsics_for(has_virtual_destructor)
        template<class T> struct has_virtual_destructor
            : bool_constant<__has_virtual_destructor(T)> {};
#endif

#if __has_intrinsics_for(has_unique_object_representations)
        template<class T> struct has_unique_object_representations
            : bool_constant<__has_unique_object_representations(typename remove_cv<typename remove_all_extents<T>::type>::type)> {};
#endif

        template<size_t Size, class ...Types> struct __find_first_with_size {};
        template<size_t Size, class Type, class ...Types> struct __find_first_with_size<Size, Type, Types...>
            : conditional<Size == sizeof(Type), Type, typename __find_first_with_size<Size, Types...>::type> {};

        /* Add the reference qualifiers of From to To. */
        template<class From, class To> struct __add_reference_of { using type = To; };
        template<class T, class To> struct __add_reference_of<T&, To> { using type = To&; }; 
        template<class T, class To> struct __add_reference_of<T&&, To> { using type = To&&; }; 

        /* Add the cv qualifiers of From to To. */
        template<class From, class To> struct __add_cv_of { using type = To; };
        template<class T, class To> struct __add_cv_of<const T, To> { using type = const To; };
        template<class T, class To> struct __add_cv_of<volatile T, To> { using type = volatile To; };
        template<class T, class To> struct __add_cv_of<const volatile T, To> { using type = const volatile To; };

        /* Apply UnaryTemplate to T, and then apply the cv-qualifiers of T to the result type. */
        template<typename T, template<typename> typename UnaryTemplate> struct __preserve_cv : __add_cv_of<T, typename UnaryTemplate<T>::type> {};

#if __has_intrinsics_for(is_enum)
        template<class T> struct __make_signed { using type = typename __find_first_with_size<sizeof(T), signed char, signed short, signed int, signed long, signed long long>::type; };
        template<> struct __make_signed<unsigned char> { using type = signed char; };
        template<> struct __make_signed<unsigned short> { using type = signed short; };
        template<> struct __make_signed<unsigned int> { using type = signed int; };
        template<> struct __make_signed<unsigned long> { using type = signed long; };
        template<> struct __make_signed<unsigned long long> { using type = signed long long; };
        template<> struct __make_signed<signed char> { using type = signed char; };
        template<> struct __make_signed<signed short> { using type = signed short; };
        template<> struct __make_signed<signed int> { using type = signed int; };
        template<> struct __make_signed<signed long> { using type = signed long; };
        template<> struct __make_signed<signed long long> { using type = signed long long; };
        
        template<class T> requires (integral<T> || enumeration<T>) && (!is_same<typename remove_cv<T>::type, bool>::value)
        struct make_signed : __preserve_cv<T, __make_signed> {};
#endif

#if __has_intrinsics_for(is_enum)
        template<class T> struct __make_unsigned { using type = typename __find_first_with_size<sizeof(T), unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>::type; };
        template<> struct __make_unsigned<unsigned char> { using type = unsigned char; };
        template<> struct __make_unsigned<unsigned short> { using type = unsigned short; };
        template<> struct __make_unsigned<unsigned int> { using type = unsigned int; };
        template<> struct __make_unsigned<unsigned long> { using type = unsigned long; };
        template<> struct __make_unsigned<unsigned long long> { using type = unsigned long long; };
        template<> struct __make_unsigned<signed char> { using type = unsigned char; };
        template<> struct __make_unsigned<signed short> { using type = unsigned short; };
        template<> struct __make_unsigned<signed int> { using type = unsigned int; };
        template<> struct __make_unsigned<signed long> { using type = unsigned long; };
        template<> struct __make_unsigned<signed long long> { using type = unsigned long long; };

        template<class T> requires (integral<T> || enumeration<T>) && (!is_same<typename remove_cv<T>::type, bool>::value)
        struct make_unsigned : __preserve_cv<T, __make_unsigned> {};
#endif

        template<class T> struct remove_pointer { using type = T; };
        template<class T> struct remove_pointer<T*> { using type = T; };
        template<class T> struct remove_pointer<T* const> { using type = T; };
        template<class T> struct remove_pointer<T* volatile> { using type = T; };
        template<class T> struct remove_pointer<T* const volatile> { using type = T; };

        template<class T> 
        auto __add_pointer(int) -> typename enable_if<is_referenceable<T>::value, typename remove_reference<T>::type*>::type;
        template<class T> 
        auto __add_pointer(int) -> typename enable_if<is_void<typename remove_cv<T>::type>::value, T*>::type;
        template<class T> 
        auto __add_pointer(...) -> T;

        template<class T> struct add_pointer {
            using type = decltype(__add_pointer<T>(0));
        };

        template<class T> struct type_identity { using type = T; };

        template<size_t Len> requires (Len > 0) && (Len <= alignof(max_align_t))
        consteval size_t __default_alignment() noexcept {
            size_t alignment = 1;
            while (Len >= alignment) {
                alignment *= 2;
            }

            return alignment / 2;
        }

        template<size_t Align> requires (Align >= 1) && (Align <= alignof(max_align_t))
        consteval bool __is_alignment_of_some_type() noexcept {
            for (size_t alignment = 1; alignment <= alignof(max_align_t); alignment *= 2) {
                if (alignment == Align) {
                    return true;
                }
            }

            return false;
        }

        template<size_t Len, size_t Align = __default_alignment<Len>()> 
            requires (Len > 0) && (Align == __default_alignment<Len>() || __is_alignment_of_some_type<Align>())
        struct aligned_storage {
            struct type {
                alignas(Align) unsigned char data[Len];
            };
        };

        template<class T, T N> consteval T __max() noexcept {
            return N;
        }
        template<class T, T N1, T ...N> consteval T __max() noexcept {
            constexpr T remaining_max = __max<T, N...>();
            return remaining_max > N1 ? remaining_max : N1;
        }

        template<size_t Len, class ...Types> requires (sizeof...(Types) > 0) && (__is_complete<Types>::value && ...) && (is_object<Types>::value && ...)
        struct aligned_union {
            static constexpr size_t alignment_value = __max<size_t, (alignof(Types), ...)>();
            struct type {
                alignas(alignment_value) unsigned char value[__max<size_t, Len, (sizeof(Types), ...)>()];
            };
        };

        template<class T> struct remove_cvref : remove_cv<typename remove_reference<T>::type> {};

        template<class T> auto __decay(int) -> typename enable_if<is_array<T>::value, typename remove_extent<T>::type*>::type;
        template<class T> auto __decay(int) -> typename enable_if<is_function<T>::value, typename add_pointer<T>::type>::type;
        template<class T> auto __decay(...) -> typename remove_cv<T>::type;

        template<class T> struct decay {
            using type = decltype(__decay<typename remove_reference<T>::type>(0));
        };
        
        template<class T1> auto __common_type(...) -> decltype(__common_type<T1, T1>(0));
        template<class T1, class T2> auto __common_type(int) -> decltype(false ? declval<typename decay<T1>::type>() : declval<typename decay<T2>::type>());
        template<class T1, class T2> auto __common_type(...) -> decltype(false ? declval<const typename remove_reference<typename decay<T1>::type>::type&>() : declval<const typename remove_reference<typename decay<T2>::type>::type&>());
        template<class T1, class T2, class ...Tn> auto __common_type(...) -> decltype(__common_type<decltype(__common_type<T1, T2>(0)), Tn...>(0));
        template<class ...T> struct common_type { using type = typename decay<decltype(__common_type<T...>(0))>::type; };

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

        template<class B> auto __is_base_of_test(B*) -> true_type;
        template<class B> auto __is_base_of_test(...) -> false_type;

        template<class Base, class Derived, class = void> struct __is_base_of_impl : true_type {};
        template<class Base, class Derived> struct __is_base_of_impl<Base, Derived, void_t<decltype(__is_base_of_test<Base>(declval<Derived*>()))>> : decltype(__is_base_of_test<Base>(declval<Derived*>())) {};

        template<class Base, class Derived> struct is_base_of
            : bool_constant<is_class<Base>::value && is_class<Derived>::value && __is_base_of_impl<typename remove_cv<Base>::type, typename remove_cv<Derived>::type>::value> {};

#if __has_intrinsics_for(is_layout_compatible)
        template<class T, class U> struct is_layout_compatible
            : bool_constant<__is_layout_compatible(T, U)> {};
#endif

#if __has_intrinsics_for(is_pointer_interconvertible_base_of)
        template<class Base, class Derived> struct is_pointer_interconvertible_base_of
            : bool_constant<__is_pointer_interconvertible_base_of(Base, Derived)> {};
#endif

        template<class T> constexpr T&& forward(typename remove_reference<T>::type& t) noexcept {
            return static_cast<T&&>(t);
        }
        template<class T> constexpr T&& forward(typename remove_reference<T>::type&& t) noexcept {
            return static_cast<T&&>(t);
        }

        template<class T> struct __is_reference_wrapper : false_type {};
        template<class T> struct __is_reference_wrapper<::yilib::reference_wrapper<T>> : true_type {};

        // 20.14.4.1.1
        template<class MemberPtr, class Base, class T1, class ...Args> 
        requires is_member_function_pointer<MemberPtr Base::*>::value && is_base_of<Base, typename decay<T1>::type>::value
        constexpr auto __INVOKE(int*, MemberPtr Base::* fn, T1&& t1, Args&& ...args)
            noexcept(noexcept((forward<T1>(t1).*fn)(forward<Args>(args)...))) -> decltype((forward<T1>(t1).*fn)(forward<Args>(args)...)) {
                return (forward<T1>(t1).*fn)(forward<Args>(args)...);
        }

        // 20.14.4.1.2
        template<class MemberPtr, class Base, class T1, class ...Args> 
        requires is_member_function_pointer<MemberPtr Base::*>::value && __is_reference_wrapper<typename decay<T1>::type>::value
        constexpr auto __INVOKE(int*, MemberPtr Base::* fn, T1&& t1, Args&& ...args)
            noexcept(noexcept((t1.get().*fn)(forward<Args>(args)...))) -> decltype((t1.get().*fn)(forward<Args>(args)...)) {
                return (t1.get().*fn)(forward<Args>(args)...);
        }

        // 20.14.4.1.3
        template<class MemberPtr, class Base, class T1, class ...Args> 
        requires is_member_function_pointer<MemberPtr Base::*>::value
        constexpr auto __INVOKE(void*, MemberPtr Base::* fn, T1&& t1, Args&& ...args)
            noexcept(noexcept(((*forward<T1>(t1)).*fn)(forward<Args>(args)...))) -> decltype(((*forward<T1>(t1)).*fn)(forward<Args>(args)...)) {
                return ((*forward<T1>(t1)).*fn)(forward<Args>(args)...);
        }

        template<class MemberPtr, class Base, class T1>
        requires is_member_object_pointer<MemberPtr Base::*>::value && is_base_of<Base, typename decay<T1>::type>::value
        constexpr auto __INVOKE(int*, MemberPtr Base::* fn, T1&& t1)
            noexcept(noexcept(forward<T1>(t1).*fn)) -> decltype(forward<T1>(t1).*fn) {
                return forward<T1>(t1).*fn;
        }

        template<class MemberPtr, class Base, class T1>
        requires is_member_object_pointer<MemberPtr Base::*>::value && __is_reference_wrapper<typename decay<T1>::type>::value
        constexpr auto __INVOKE(int*, MemberPtr Base::* fn, T1&& t1)
            noexcept(noexcept(t1.get().*fn)) -> decltype(t1.get().*fn) {
                return t1.get().*fn;
        }

        template<class MemberPtr, class Base, class T1>
        requires is_member_object_pointer<MemberPtr Base::*>::value
        constexpr auto __INVOKE(void*, MemberPtr Base::* fn, T1&& t1)
            noexcept(noexcept((*forward<T1>(t1)).*fn)) -> decltype((*forward<T1>(t1)).*fn) {
                return (*forward<T1>(t1)).*fn;
        }

        template<class Fn, class ...Args>
        constexpr auto __INVOKE(void*, Fn&& fn, Args&& ...args) 
            noexcept(noexcept((forward<Fn>(fn))(forward<Args>(args)...))) -> decltype((forward<Fn>(fn))(forward<Args>(args)...)) {
            return (forward<Fn>(fn))(forward<Args>(args)...);
        }

        template<class Void, class ...Args> requires is_void<Void>::value
        struct __is_invocable : false_type {};
        template<class ...Args> struct __is_invocable<void_t<decltype(__INVOKE(static_cast<int*>(nullptr), declval<Args>()...))>, Args...> : true_type {};

        template<class Fn, class ...Args> struct is_invocable : __is_invocable<void, Fn, Args...> {};

        template<class R, class ...Args>
        struct __is_invocable_r {
            using Return = decltype(__INVOKE(static_cast<int*>(nullptr), declval<Args>()...));
            using type = typename conditional<
                is_void<R>::value,
                true_type,
                is_convertible<Return, R>
            >::type;
        };

        template<class R, class Fn, class ...Args> struct is_invocable_r : __is_invocable_r<R, Fn, Args...>::type {};

        template<class Fn, class ...Args>
        constexpr auto __INVOKE_NOTHROW(int*, Fn&& fn, Args&& ...args) noexcept -> decltype(auto) {
            return __INVOKE(static_cast<int*>(nullptr), forward<Fn>(fn), forward<Args>(args)...);
        }

        template<class Void, class ...Args> requires is_void<Void>::value
        struct __is_nothrow_invocable : false_type {};
        template<class ...Args> struct __is_nothrow_invocable<void_t<decltype(__INVOKE_NOTHROW(static_cast<int*>(nullptr), declval<Args>()...))>, Args...> : true_type {};

        template<class Fn, class ...Args> struct is_nothrow_invocable : __is_nothrow_invocable<void, Fn, Args...> {};

        template<class R, class ...Args>
        struct __is_nothrow_invocable_r {
            using Return = decltype(__INVOKE_NOTHROW(static_cast<int*>(nullptr), declval<Args>()...));
            using type = typename conditional<
                is_void<R>::value,
                true_type,
                is_convertible<Return, R>
            >::type;
        };

        template<class R, class Fn, class ...Args> struct is_nothrow_invocable_r : __is_nothrow_invocable_r<R, Fn, Args...>::type {};

        template<class Void, class ...Args> struct __invoke_result {};
        template<class ...Args> struct __invoke_result<void_t<decltype(__INVOKE(static_cast<int*>(nullptr), declval<Args>()...))>, Args...> {
            using type = decltype(__INVOKE(static_cast<int*>(nullptr), declval<Args>()...));
        };

        template<class Fn, class ...Args> struct invoke_result : __invoke_result<void, Fn, Args...> {};

#if __has_intrinsics_for(underlying_type) && __has_intrinsics_for(is_enum)
        template<class T, bool B> struct __underlying_type_impl {};
        template<class T> struct __underlying_type_impl<T, true> { using type = __underlying_type(T); };

        template<class T> struct underlying_type : __underlying_type_impl<T, is_enum<T>::value> {};
#endif

        template<class T> struct unwrap_reference {};
        template<class T> struct unwrap_reference<::yilib::reference_wrapper<T>> { using type = T&; };

        template<class T> struct unwrap_ref_decay : unwrap_reference<typename decay<T>::type> {};

#if __has_intrinsics_for(builtin_is_constant_evaluated)
        constexpr bool is_constant_evaluated() noexcept {
            return __builtin_is_constant_evaluated();
        }
#endif

#if __has_intrinsics_for(is_pointer_interconvertible_with_class)
        template<class S, class M>
        constexpr bool is_pointer_interconvertible_with_class(M S::* mp) noexcept {
            return __is_pointer_interconvertible_with_class(mp);
        }
#endif

#if __has_intrinsics_for(is_corresponding_member)
        template<class S1, class S2, class M1, class M2>
        constexpr bool is_corresponding_member(M1 S1::* mp, M2 S2::* mq) noexcept {
            return __is_corresponding_member(mp, mq);
        }
#endif

        template<class T, class U, template<class> class TQual, template<class> class UQual> struct basic_common_reference{};

        /* Given two types T1, T2, apply the union of the volatile qualifier of T1 and T2 to T. */
        template<class T1, class T2, class T> struct __volatile_union :
            conditional<is_volatile<T1>::value || is_volatile<T2>::value, volatile T, T> {};

        /* Given two types T1, T2, apply the union of the const qualifier of T1 and T2 to T. */
        template<class T1, class T2, class T> struct __const_union :
            conditional<is_const<T1>::value || is_const<T2>::value, const T, T> {};

        /* Given two types T1, T2, apply the union of the cv qualifiers of T1 and T2 to T. */
        template<class T1, class T2, class T> struct __cv_union :
            __volatile_union<T1, T2, typename __const_union<T1, T2, T>::type> {};

        /* Returns the simple common reference type of the given reference types T1, T2. The definition for simple common reference type can be found
         * in Note 3, Section 20.15.8.6 of the standard. */
        /*
        // 3.5 -- If A and B are both lvalue reference types ...
        template<class T1, class T2> requires is_reference<T1>::value && is_reference<T2>::value
        auto __simple_common_reference_type(int) -> 
            typename enable_if<is_lvalue_reference<T1>::value && is_lvalue_reference<T2>::value, 
                decltype(false ? declval<typename __cv_union<T1, T2, T1>::type>() : declval<typename __cv_union<T1, T2, T2>::type>())>::type; */
        struct __no_type {};

        template<class T1, class T2> requires is_reference<T1>::value && is_reference<T2>::value
        constexpr auto __simple_common_reference_type() noexcept -> decltype(auto) {
            if constexpr (is_lvalue_reference<T1>::value && is_lvalue_reference<T2>::value) {
                using C = decltype(false ? declval<typename __cv_union<T1, T2, T1>::type>() : declval<typename __cv_union<T1, T2, T2>::type>());
                if constexpr (is_reference<C>::value) {
                    return __val<C>();
                } else {
                    return __no_type();
                }
            } else if constexpr (is_rvalue_reference<T1>::value && is_rvalue_reference<T2>::value) {
                using C = decltype(__simple_common_reference_type<T1&, T2&>())&&;
                if constexpr (is_convertible<T1, C>::value && is_convertible<T2, C>::value) {
                    return __val<C>();
                } else {
                    return __no_type();
                }
            } else if constexpr (is_lvalue_reference<T1>::value) {
                using A = typename remove_reference<T1>::type;
                using B = typename remove_reference<T2>::type;
                using D = decltype(__simple_common_reference_type<A&, B const&>());
                if constexpr (is_convertible<B&&, D>::value) {
                    return __val<D>();
                } else {
                    return __no_type();
                }
            } else {
                return __simple_common_reference_type<T2, T1>();
            }
        }

        template<class T> T __val();

        template<class ...T>
        constexpr auto __common_reference_impl(...) noexcept -> __no_type;
        template<>
        constexpr auto __common_reference_impl(...) noexcept -> __no_type;
        template<class T>
        constexpr auto __common_reference_impl(int) noexcept -> T;
        template<class T1, class T2>
        constexpr auto __common_reference_impl(int) noexcept -> decltype(auto) {
            if constexpr (is_reference<T1>::value && is_reference<T2>::value) {
                using C = decltype(__simple_common_reference_type<T1, T2>());
                return __val<C>();
            } else if constexpr (auto val = false ? __val<T1>() : __val<T2>(); true) {
                return __val<decltype(val)>();
            } else if constexpr (auto val = declval<common_type<T1, T2>::type>(); true) {
                return __val<decltype(val)>();
            } else {
                return __no_type();
            }
        }
        template<class T1, class T2, class ...T>
        constexpr auto __common_reference_impl(...) noexcept
            -> decltype(__common_reference<decltype(__common_reference<T1, T2>(0)), T...>(0));

        struct __empty_struct {};

        template<class ...T> struct common_reference 
            : conditional<is_same<__no_type, decltype(__common_reference_impl<T...>(0))>::value, __empty_struct, type_identity<decltype(__common_reference_impl<T...>(0))>>::type {};
    }
}