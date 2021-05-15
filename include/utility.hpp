#pragma once

#include "utility/typecast.hpp"
#include "utility/declval.hpp"

#include "compare.hpp"
#include "initializer_list.hpp"
#include "type_traits.hpp"
#include "cstddef.hpp"
#include "limits.hpp"
#include "tuple.hpp"

namespace std {
    /* 20.2.2 swap */
    template<class T> requires is_move_constructible_v<T> && is_move_assignable_v<T>
    constexpr void swap(T& a, T& b) noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_move_assignable_v<T>) {
        T c = __internal::move(a);
        a = __internal::move(b);
        b = __internal::move(c);
    }

    template<class T, size_t N> requires is_swappable_v<T>
    constexpr void swap(T (&a)[N], T (&b)[N]) noexcept(is_nothrow_swappable_v<T>) {
        for (auto i = 0; i < N; i++) {
            swap(a[i], b[i]);
        }
    }

    /* 20.2.3 exchange */
    template<class T, class U = T> constexpr T exchange(T& obj, U&& new_val) {
        T old = __internal::move(obj);
        obj = __internal::forward(new_val);
        return old;
    }

    /* 20.2.4 Forward/move */
    using __internal::forward;
    using __internal::move;

    /* 20.2.5 Function template as_const */
    template<class T> constexpr add_const_t<T>& as_const(T& t) noexcept {
        return t;
    }

    /* 20.2.6 Function template declval */
    using __internal::declval;

    /* 20.2.7 Integer comparison functions */
    template<class T, class U> requires is_integral_v<T> && is_integral_v<U>
    constexpr bool cmp_equal(T t, U u) noexcept {
        using UT = make_unsigned_t<T>;
        using UU = make_unsigned_t<U>;
        if constexpr (is_signed_v<T> == is_signed_v<U>) {
            return t == u;
        } else if constexpr (is_signed_v<T>) {
            return t < 0 ? false : UT(t) == u;
        } else {
            return u < 0 ? false : t == UU(u);
        }
    }
    template<class T, class U> requires is_integral_v<T> && is_integral_v<U>
    constexpr bool cmp_not_equal(T t, U u) noexcept {
        return !cmp_equal(t, u);
    }

    template<class T, class U> requires is_integral_v<T> && is_integral_v<U>
    constexpr bool cmp_less(T t, U u) noexcept {
        using UT = make_unsigned_t<T>;
        using UU = make_unsigned_t<U>;
        if constexpr (is_signed_v<T> == is_signed_v<U>) {
            return t < u;
        } else if constexpr (is_signed_v<T>) {
            return t < 0 ? true : UT(t) < u;
        } else {
            return u < 0 ? false : t < UU(u);
        }
    }

    template<class T, class U> requires is_integral_v<T> && is_integral_v<U>
    constexpr bool cmp_greater(T t, U u) noexcept {
        return cmp_less(u, t);
    }

    template<class T, class U> requires is_integral_v<T> && is_integral_v<U>
    constexpr bool cmp_less_equal(T t, U u) noexcept {
        return !cmp_greater(t, u);
    }

    template<class T, class U> requires is_integral_v<T> && is_integral_v<U>
    constexpr bool cmp_greater_equal(T t, U u) noexcept {
        return !cmp_less(t, u);
    }

    template<class R, class T> requires is_integral_v<R> && is_integral_v<T>
    constexpr bool in_range(T t) noexcept {
        return cmp_greater_equal(t, numeric_limits<R>::min()) && cmp_less_equal(t, numeric_limits<R>::max());
    }

    /* 20.3 Compile-time integer sequences */
    template<class T, T ...I> requires is_integral_v<T>
    struct integer_sequence {
        using value_type = T;
        static constexpr size_t size() noexcept { return sizeof...(I); }
    };

    template<size_t ...I> using index_sequence = integer_sequence<size_t, I...>;

    namespace __internal {
        template<class T, T N, T Curr, T ...Remaining> constexpr auto __make_integer_sequence_test() {
            if constexpr (N == 0) {
                return integer_sequence<T>{};
            } else if constexpr (Curr == 0) {
                return integer_sequence<T, Curr, Remaining...>{};
            } else {
                return __make_integer_sequence_test<T, N, Curr - 1, Curr, Remaining...>();
            }
        }

        template<class T, class IntConstant> requires (IntConstant::value >= 0)
        struct __make_integer_sequence {
            using type = decltype(__internal::__make_integer_sequence_test<T, IntConstant::value, IntConstant::value - 1>());
        };
    }

    template<class T, T N> requires is_integral_v<T> && (N >= 0)
    using make_integer_sequence = 
        typename __internal::__make_integer_sequence<T, integral_constant<T, N>>::type;
    template<size_t N> using make_index_sequence = make_integer_sequence<size_t, N>;

    template<class ...T> using index_sequence_for = make_index_sequence<sizeof...(T)>;

    /* 20.4 Class template pair */
    namespace __internal {
        /* Models a type T that has an implicit default constructor. This check relies on the fact that copy-list-initialization of
         * an variable doesn't use explicit constructors. Of all the copy-list-initializations, we use the only one available as an
         * expression, which is to pass an empty initializer list into a function parameter of type T. */
        template<class T> concept __is_implicit_default_constructible = requires (void (&fn)(T)) { fn({}); };
    }

    struct piecewise_construct_t {
        explicit piecewise_construct_t() = default;
    };

    template<class T1, class T2> struct pair {
        using first_type = T1;
        using second_type = T2;

        T1 first;
        T2 second;

        pair(const pair&) = default;
        pair(pair&&) = default;

        constexpr explicit(!__internal::__is_implicit_default_constructible<T1> || !__internal::__is_implicit_default_constructible<T2>) 
        pair() requires is_default_constructible_v<T1> && is_default_constructible_v<T2>
            : first(), second() {}

        constexpr explicit(!is_convertible_v<const T1&, T1> || !is_convertible_v<const T2&, T2>)
        pair(const T1& x, const T2& y) requires is_copy_constructible_v<T1> && is_copy_constructible_v<T2>
            : first(x), second(y) {}

        template<class U1, class U2> requires is_constructible_v<T1, U1> && is_constructible_v<T2, U2>
        constexpr explicit(!is_convertible_v<U1, T1> || !is_convertible_v<U2, T2>)
        pair(U1&& x, U2&& y) : first(forward<U1>(x)), second(forward<U2>(y)) {}

        template<class U1, class U2> requires is_constructible_v<T1, const T1&> && is_constructible_v<T2, const T2&>
        constexpr explicit(!is_convertible_v<const U1&, T1> || !is_convertible_v<const U2&, T2>)
        pair(const pair<U1, U2>& p) : first(p.first), second(p.second) {}

        template<class U1, class U2> requires is_constructible_v<T1, U1> && is_constructible_v<T2, U2>
        constexpr explicit(!is_convertible_v<U1, T1> || !is_convertible_v<U2, T2>) 
        pair(pair<U1, U2>&& p) : first(forward<U1>(p.first)), second(forward<U2>(p.second)) {}

        // TODO: Implement!
        template<class ...Args1, class ...Args2> requires is_constructible_v<T1, Args1...> && is_constructible_v<T2, Args2...>
        constexpr pair(piecewise_construct_t, tuple<Args1...> first_args, tuple<Args2...> second_args)  
            : T1(), T2() {}

        constexpr pair& operator=(const pair& p) requires is_copy_assignable_v<T1> && is_copy_assignable_v<T2> {
            first = p.first;
            second = p.second;
            return *this;
        }

        template<class U1, class U2> requires is_assignable_v<T1&, const U1&> && is_assignable_v<T2&, const U2&>
        constexpr pair& operator=(const pair<U1, U2>& p) {
            first = p.first;
            second = p.second;
            return *this;
        }

        constexpr pair& operator=(pair&& p) noexcept(noexcept(is_nothrow_move_assignable_v<T1> && is_nothrow_move_assignable_v<T2>))
            requires is_move_assignable_v<T1> && is_move_assignable_v<T2> {
            first = forward<T1>(p.first);
            second = forward<T2>(p.second);
            return *this;
        }

        template<class U1, class U2> requires is_assignable_v<T1&, U1> && is_assignable_v<T2&, U2>
        constexpr pair& operator=(pair<U1, U2>&& p) {
            first = forward<U1>(p.first);
            second = forward<U2>(p.second);
            return *this;
        }

        constexpr void swap(pair& p) noexcept(noexcept(is_nothrow_swappable_v<T1> && is_nothrow_swappable_v<T2>))
            requires is_swappable_v<T1> && is_swappable_v<T2> {
            swap(first, p.first);
            swap(second, p.second);
        }
    };

    template<class T1, class T2> pair(T1, T2) -> pair<T1, T2>;

    inline constexpr piecewise_construct_t piecewise_construct{};

    template<class T1, class T2> constexpr bool operator==(const pair<T1, T2>& x, const pair<T1, T2>& y) {
        return x.first == y.first && x.second == y.second;
    }

    template<class T1, class T2> constexpr common_comparison_category_t<__internal::synth_three_way_result<T1>, __internal::synth_three_way_result<T2>>
        operator<=>(const pair<T1, T2>& x, const pair<T1, T2>& y) {
            if (auto c = __internal::synth_three_way(x.first, y.first); c != 0) return c;
            return __internal::synth_three_way(x.second, y.second);
    }

    template<class T1, class T2> requires is_swappable_v<T1> && is_swappable_v<T2>
    constexpr void swap(pair<T1, T2>& x, pair<T1, T2>& y) noexcept(noexcept(x.swap(y))) {
        return x.swap(y);
    }

    template<class T1, class T2> constexpr pair<unwrap_ref_decay_t<T1>, unwrap_ref_decay_t<T2>> make_pair(T1&& x, T2&& y) {
        return pair<unwrap_ref_decay_t<T1>, unwrap_ref_decay_t<T2>>(forward<T1>(x), forward<T2>(y));
    }

    template<class T1, class T2> struct tuple_size<pair<T1, T2>> : integral_constant<size_t, 2> {};
    template<size_t I, class T1, class T2> requires (I < 2)
    struct tuple_element<I, pair<T1, T2>> {
        using type = conditional_t<I == 0, T1, T2>;
    };

    template<size_t I, class T1, class T2> requires (I < 2)
    constexpr tuple_element_t<I, pair<T1, T2>>& get(pair<T1, T2>& p) noexcept {
        if constexpr (I == 0) { return p.first; }
        else { return p.second; }
    }

    template<size_t I, class T1, class T2> requires (I < 2)
    constexpr const tuple_element_t<I, pair<T1, T2>>& get(const pair<T1, T2>& p) noexcept {
        if constexpr (I == 0) { return p.first; }
        else { return p.second; }
    }

    template<size_t I, class T1, class T2> requires (I < 2)
    constexpr tuple_element_t<I, pair<T1, T2>>&& get(pair<T1, T2>&& p) noexcept {
        if constexpr (I == 0) { return p.first; }
        else { return p.second; }
    }

    template<size_t I, class T1, class T2> requires (I < 2)
    constexpr const tuple_element_t<I, pair<T1, T2>>&& get(const pair<T1, T2>&& p) noexcept {
        if constexpr (I == 0) { return p.first; }
        else { return p.second; }
    }

    template<class T1, class T2> requires (!is_same_v<T1, T2>)
    constexpr T1& get(pair<T1, T2>& p) noexcept {
        return p.first;
    }

    template<class T1, class T2> requires (!is_same_v<T1, T2>)
    constexpr const T1& get(const pair<T1, T2>& p) noexcept {
        return p.first;
    }

    template<class T1, class T2> requires (!is_same_v<T1, T2>)
    constexpr T1&& get(pair<T1, T2>&& p) noexcept {
        return p.first;
    }

    template<class T1, class T2> requires (!is_same_v<T1, T2>)
    constexpr const T1&& get(const pair<T1, T2>&& p) noexcept {
        return p.first;
    }

    template<class T1, class T2> requires (!is_same_v<T1, T2>)
    constexpr T2& get(pair<T1, T2>& p) noexcept {
        return p.second;
    }

    template<class T1, class T2> requires (!is_same_v<T1, T2>)
    constexpr const T2& get(const pair<T1, T2>& p) noexcept {
        return p.second;
    }

    template<class T1, class T2> requires (!is_same_v<T1, T2>)
    constexpr T2&& get(pair<T1, T2>&& p) noexcept {
        return p.second;
    }

    template<class T1, class T2> requires (!is_same_v<T1, T2>)
    constexpr const T2&& get(const pair<T1, T2>&& p) noexcept {
        return p.second;
    }

    struct in_place_t {
        explicit in_place_t() = default;
    };
    inline constexpr in_place_t in_place{};

    template<class T> struct in_place_type_t {
        explicit in_place_type_t() = default;
    };
    template<class T> inline constexpr in_place_type_t<T> in_place_type{};

    template<size_t I> struct in_place_index_t {
        explicit in_place_index_t() = default;
    };
    template<size_t I> inline constexpr in_place_index_t<I> in_place_index{};
}
