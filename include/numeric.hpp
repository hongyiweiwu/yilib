#pragma once

#include "type_traits.hpp"
#include "iterator.hpp"
#include "functional.hpp"
#include "concepts.hpp"

// TODO: Implement parallel algorithms.
namespace std {
    template<__internal::legacy_input_iterator InputIterator, class T>
    requires is_copy_constructible_v<T> && is_copy_assignable_v<T>
    constexpr T accumulate(InputIterator first, InputIterator last, T init) {
        return accumulate(first, last, init, plus<>());
    }

    template<__internal::legacy_input_iterator InputIterator, class T, class BinaryOperation>
    requires is_copy_constructible_v<T> && is_copy_assignable_v<T>
    constexpr T accumulate(InputIterator first, InputIterator last, T init, BinaryOperation binary_op) {
        T acc = move(init);
        for (; first != last; first++) {
            acc = binary_op(move(acc), *first);
        }
        return acc;
    }

    template<__internal::legacy_input_iterator InputIterator>
    constexpr typename iterator_traits<InputIterator>::value_type reduce(InputIterator first, InputIterator last) {
        return reduce(first, last, typename iterator_traits<InputIterator>::value_type{});
    }

    template<__internal::legacy_input_iterator InputIterator, class T>
    constexpr typename iterator_traits<InputIterator>::value_type reduce(InputIterator first, InputIterator last, T init) {
        return reduce(first, last, init, plus<>());
    }

    template<__internal::legacy_input_iterator InputIterator, move_constructible T, class BinaryOperation>
    constexpr typename iterator_traits<InputIterator>::value_type reduce(InputIterator first, InputIterator last, T init, BinaryOperation binary_op)
    requires requires {
        { binary_op(init, *first) } -> convertible_to<T>;
        { binary_op(*first, init) } -> convertible_to<T>;
        { binary_op(init, init) } -> convertible_to<T>;
        { binary_op(*first, *first) } -> convertible_to<T>;
    } {
        return accumulate(first, last, init, binary_op);
    }

    template<__internal::legacy_input_iterator InputIterator1, __internal::legacy_input_iterator InputIterator2, class T>
    requires is_copy_constructible_v<T> && is_copy_assignable_v<T>
    constexpr T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init) {
        return inner_product(first1, last1, first2, init, plus<>(), multiplies<>());
    }

    template<__internal::legacy_input_iterator InputIterator1, __internal::legacy_input_iterator InputIterator2, class T, class BinaryOperation1, class BinaryOperation2>
    requires is_copy_constructible_v<T> && is_copy_assignable_v<T>
    constexpr T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init, BinaryOperation1 binary_op1, BinaryOperation2 binary_op2) {
        T acc = move(init);

        for (; first1 != last1; first1++, first2++) {
            acc = binary_op1(move(acc), binary_op2(*first1, *first2));
        }

        return acc;
    }

    template<__internal::legacy_input_iterator InputIterator1, __internal::legacy_input_iterator InputIterator2, class T>
    constexpr T transform_reduce(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init) {
        return transform_reduce(first1, last1, first2, init, plus<>(), multiplies<>());
    }

    template<__internal::legacy_input_iterator InputIterator1, __internal::legacy_input_iterator InputIterator2, move_constructible T, class BinaryOperation1, class BinaryOperation2>
    constexpr T transform_reduce(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init, BinaryOperation1 reduce, BinaryOperation2 transform) 
    requires requires {
        { reduce(init, init) } -> convertible_to<T>;
        { reduce(init, transform(*first1, *first2)) } -> convertible_to<T>;
        { reduce(transform(*first1, *first2), init) } -> convertible_to<T>;
        { reduce(transform(*first1, *first2), transform(*first1, *first2)) } -> convertible_to<T>;
    } {
        T acc = move(init);
        for (; first1 != last1; first1++, first2++) {
            acc = reduce(move(acc), transform(*first1, *first2));
        }
        return acc;
    }

    template<__internal::legacy_input_iterator InputIterator, move_constructible T, class BinaryOperation, class UnaryOperation>
    constexpr T transform_reduce(InputIterator first, InputIterator last, T init, BinaryOperation reduce, UnaryOperation transform)
    requires requires {
        { reduce(init, transform(*first)) } -> convertible_to<T>;
        { reduce(transform(*first), init) } -> convertible_to<T>;
        { reduce(init, init) } -> convertible_to<T>;
        { reduce(transform(*first), transform(*first)) } -> convertible_to<T>;
    } {
        T acc = move(init);
        for (; first != last; first++) {
            acc = reduce(move(acc), transform(*first));
        }
        return acc;
    }

    template<__internal::legacy_input_iterator InputIterator, __internal::legacy_output_iterator OutputIterator>
    constexpr OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result) {
        return partial_sum(first, last, result, plus<>());
    }

    template<__internal::legacy_input_iterator InputIterator, __internal::legacy_output_iterator OutputIterator, class BinaryOperation>
    constexpr OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result, BinaryOperation binary_op) {
        if (first == last) {
            return result;
        }

        typename iterator_traits<InputIterator>::value_type sum = *first;
        *result = sum;
        result++;
        first++;

        for (; first != last; first++, result++) {
            sum = binary_op(move(sum), *first);
            *result = sum;
        }

        return result;
    }

    template<__internal::legacy_input_iterator InputIterator, __internal::legacy_output_iterator OutputIterator, move_constructible T>
    constexpr OutputIterator exclusive_scan(InputIterator first, InputIterator last, OutputIterator result, T init) {
        return exclusive_scan(first, last, result, init, plus<>());
    }

    template<__internal::legacy_input_iterator InputIterator, __internal::legacy_output_iterator OutputIterator, move_constructible T, class BinaryOperation>
    constexpr OutputIterator exclusive_scan(InputIterator first, InputIterator last, OutputIterator result, T init, BinaryOperation binary_op)
    requires requires {
        { binary_op(init, *first) } -> convertible_to<T>;
        { binary_op(init, init) } -> convertible_to<T>;
        { binary_op(*first, init) } -> convertible_to<T>;
        { binary_op(*first, *first) } -> convertible_to<T>;
    } {
        return transform_exclusive_scan(first, last, result, init, binary_op, identity());
    }

    template<__internal::legacy_input_iterator InputIterator, __internal::legacy_output_iterator OutputIterator>
    constexpr OutputIterator inclusive_scan(InputIterator first, InputIterator last, OutputIterator result) {
        return inclusive_scan(first, last, result, plus<>());
    }

    template<__internal::legacy_input_iterator InputIterator, __internal::legacy_output_iterator OutputIterator, class BinaryOperation>
    constexpr OutputIterator inclusive_scan(InputIterator first, InputIterator last, OutputIterator result, BinaryOperation binary_op)
    requires is_move_constructible_v<typename iterator_traits<InputIterator>::value_type> && requires {
        { binary_op(*first, *first) } -> convertible_to<typename iterator_traits<InputIterator>::value_type>;
    } {
        return transform_inclusive_scan(first, last, result, binary_op, identity());
    }

    template<__internal::legacy_input_iterator InputIterator, __internal::legacy_output_iterator OutputIterator, class BinaryOperation, move_constructible T>
    constexpr OutputIterator inclusive_scan(InputIterator first, InputIterator last, OutputIterator result, BinaryOperation binary_op, T init) 
    requires requires {
        { binary_op(init, init) } -> convertible_to<T>;
        { binary_op(init, *first) } -> convertible_to<T>;
        { binary_op(*first, *first) } -> convertible_to<T>;
    } {
        return transform_inclusive_scan(first, last, result, binary_op, identity(), init);
    }

    template<__internal::legacy_input_iterator InputIterator, __internal::legacy_output_iterator OutputIterator, move_constructible T, class BinaryOperation, class UnaryOperation>
    constexpr OutputIterator transform_exclusive_scan(InputIterator first, InputIterator last, OutputIterator result, T init, BinaryOperation binary_op, UnaryOperation unary_op)
    requires requires {
        { binary_op(init, unary_op(*first)) } -> convertible_to<T>;
        { binary_op(init, init) } -> convertible_to<T>;
        { binary_op(unary_op(*first), unary_op(*first)) } -> convertible_to<T>;
    } {
        T acc = move(init);

        for (; first != last; first++, result++) {
            *result = acc;
            acc = binary_op(move(acc), unary_op(*first));
        }

        return result;
    }

    template<__internal::legacy_input_iterator InputIterator, __internal::legacy_output_iterator OutputIterator, class BinaryOperation, class UnaryOperation>
    constexpr OutputIterator transform_inclusive_scan(InputIterator first, InputIterator last, OutputIterator result,  BinaryOperation binary_op, UnaryOperation unary_op) 
    requires is_move_constructible_v<typename iterator_traits<InputIterator>::value_type> && requires {
        { binary_op(unary_op(*first), unary_op(*first)) } -> convertible_to<typename iterator_traits<InputIterator>::value_type>;
    } {
        if (first == last) {
            return result;
        }

        typename iterator_traits<InputIterator>::value_type acc = unary_op(*first);
        *result = acc;
        first++;
        result++;

        for (; first != last; first++, result++) {
            acc = binary_op(move(acc), unary_op(*first));
            *result = acc;
        }

        return result;
    }

    template<__internal::legacy_input_iterator InputIterator, __internal::legacy_output_iterator OutputIterator, class BinaryOperation, class UnaryOperation, move_constructible T>
    constexpr OutputIterator transform_inclusive_scan(InputIterator first, InputIterator last, OutputIterator result,  BinaryOperation binary_op, UnaryOperation unary_op, T init)
    requires requires {
        { binary_op(init, unary_op(*first)) } -> convertible_to<T>;
        { binary_op(init, init) } -> convertible_to<T>;
        { binary_op(unary_op(*first), unary_op(*first)) } -> convertible_to<T>;
    } {
        T acc = move(init);
        for (; first != last; first++, result++) {
            acc = binary_op(move(acc), unary_op(*first));
            *result = acc;
        }

        return result;
    }

    template<__internal::legacy_input_iterator InputIterator, __internal::legacy_output_iterator OutputIterator>
    constexpr OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result) {
        return adjacent_difference(first, last, result, minus<>());
    }

    template<__internal::legacy_input_iterator InputIterator, __internal::legacy_output_iterator OutputIterator, class BinaryOperation>
    constexpr OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result, BinaryOperation binary_op) 
    requires is_move_assignable_v<typename iterator_traits<InputIterator>::value_type> && is_constructible_v<typename iterator_traits<InputIterator>::value_type, decltype(*first)> {
        if (first == last) {
            return result;
        }

        typename iterator_traits<InputIterator>::value_type acc = *first;
        *result = acc;
        first++;
        result++;

        for (; first != last; first++, result++) {
            typename iterator_traits<InputIterator>::value_type val = *first;
            *result = binary_op(val, move(acc));
            acc = move(val);
        }

        return result;
    }

    template<class ForwardIterator, class T>
    requires is_convertible_v<T, typename iterator_traits<ForwardIterator>::value_type> && requires (T val) { ++val; }
    constexpr void iota(ForwardIterator first, ForwardIterator last, T value) {
        while (first != last) {
            *first = value;
            value++;
            first++;
        }
    }

    template<integral M, integral N>
    requires (!is_same_v<remove_cv_t<M>, bool>) && (!is_same_v<remove_cv_t<N>, bool>)
    constexpr common_type_t<M, N> gcd(M m, N n) noexcept {
        constexpr auto abs = []<class T>(T i) noexcept {
            return i < T(0) ? -i : i;
        };

        return n == 0 ? abs(m) : gcd(n, abs(m) % abs(n));
    }

    template<integral M, integral N>
    requires (!is_same_v<remove_cv_t<M>, bool>) && (!is_same_v<remove_cv_t<N>, bool>)
    constexpr common_type_t<M, N> lcm(M m, N n) noexcept {
        constexpr auto abs = []<class T>(T i) noexcept {
            return i < T(0) ? -i : i;
        };

        return m == 0 || n == 0 ? 0 : (abs(m) / gcd(m, n)) * abs(n);
    }

    template<class T>
    requires is_arithmetic_v<T> && (!is_same_v<remove_cv_t<T>, bool>)
    constexpr T midpoint(T a, T b) noexcept {
        if constexpr (is_integral_v<T>) {
            using U = make_unsigned_t<T>;
        } else {
            return __builtin_isnormal(a) && __builtin_isnormal(b) ? (a / 2 + b / 2) : ((a + b) / 2);
        }
    }

    template<class T>
    requires is_object_v<T> && __internal::is_complete<T>::value
    constexpr T* midpoint(T* a, T* b) {
        return __builtin_isnormal(a) && __builtin_isnormal(b) ? (a / 2 + b / 2) : ((a + b) / 2);
    }
}