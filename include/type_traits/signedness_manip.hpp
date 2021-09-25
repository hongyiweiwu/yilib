#pragma once

#include "util/macros.hpp"
#include "cstddef.hpp"
#include "type_traits/base.hpp"
#include "type_traits/is_type.hpp"
#include "type_traits/relations.hpp"
#include "type_traits/cv_manip.hpp"

namespace std::__internal {
    /* Find the first element in `Types` with size greater or equal to the given `Size`. */
    template<std::size_t Size, class ...Types>
    struct find_first_with_size {};

    template<std::size_t Size, class Type, class ...Types>
    struct find_first_with_size<Size, Type, Types...>
        : conditional<Size <= sizeof(Type), Type, typename find_first_with_size<Size, Types...>::type> {};

    /* Add the cv qualifiers of From to To. */
    template<class From, class To> struct __add_cv_of { using type = To; };
    template<class T, class To> struct __add_cv_of<const T, To> { using type = const To; };
    template<class T, class To> struct __add_cv_of<volatile T, To> { using type = volatile To; };
    template<class T, class To> struct __add_cv_of<const volatile T, To> { using type = const volatile To; };

    /* Apply UnaryTemplate to T, and then apply the cv-qualifiers of T to the result type. */
    template<typename T, template<typename> typename UnaryTemplate> struct __preserve_cv : __add_cv_of<T, typename UnaryTemplate<T>::type> {};

#if __has_intrinsics_for(is_enum)
    template<class T> struct __make_signed { using type = typename find_first_with_size<sizeof(T), signed char, signed short, signed int, signed long, signed long long>::type; };
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
    
    template<class T> requires (is_integral<T>::value || is_enum<T>::value) && (!is_same<typename remove_cv<T>::type, bool>::value)
    struct make_signed : __preserve_cv<T, __make_signed> {};
#endif

#if __has_intrinsics_for(is_enum)
    template<class T> struct __make_unsigned { using type = typename find_first_with_size<sizeof(T), unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>::type; };
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

    template<class T> requires (is_integral<T>::value || is_enum<T>::value) && (!is_same<typename remove_cv<T>::type, bool>::value)
    struct make_unsigned : __preserve_cv<T, __make_unsigned> {};
#endif
}