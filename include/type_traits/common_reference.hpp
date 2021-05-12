#pragma once

#include "type_traits/is_type.hpp"
#include "type_traits/reference_manip.hpp"
#include "utility/declval.hpp"
#include "type_traits/is_operation.hpp"
#include "type_traits/decay.hpp"
#include "type_traits/relations.hpp"

namespace yilib::__internal {
        template<class T1> auto __common_type(...) -> decltype(__common_type<T1, T1>(0));
        template<class T1, class T2> auto __common_type(int) -> decltype(false ? declval<typename decay<T1>::type>() : declval<typename decay<T2>::type>());
        template<class T1, class T2> auto __common_type(...) -> decltype(false ? declval<const typename remove_reference<typename decay<T1>::type>::type&>() : declval<const typename remove_reference<typename decay<T2>::type>::type&>());
        template<class T1, class T2, class ...Tn> auto __common_type(...) -> decltype(__common_type<decltype(__common_type<T1, T2>(0)), Tn...>(0));
        template<class ...T> struct common_type { using type = typename decay<decltype(__common_type<T...>(0))>::type; };

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

        struct __no_type {};

        template<class T> T __val();

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