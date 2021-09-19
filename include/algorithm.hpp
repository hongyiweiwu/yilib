#pragma once

#include "concepts.hpp"
#include "type_traits.hpp"
#include "initializer_list.hpp"
#include "iterator.hpp"
#include "compare.hpp"

namespace std {
    namespace ranges {
        /* 25.5 Algorithm result types */
        template<class I, class O>
        struct in_out_result {
            [[no_unique_address]] I in;
            [[no_unique_address]] O out;

            template<class I2, class O2>
            requires convertible_to<const I&, I2> && convertible_to<const O&, O2>
            constexpr operator in_out_result<I2, O2>() const & {
                return {in, out};
            }

            template<class I2, class O2>
            requires convertible_to<I, I2> && convertible_to<O, O2>
            constexpr operator in_out_result<I2, O2>() && {
                return {move(in), move(out)};
            }
        };
    }

    /* 25.6.5 Find */
    template<__internal::legacy_input_iterator InputIterator, class T>
    constexpr InputIterator find(InputIterator first, InputIterator last, const T& value) {
        return find_if(first, last, [&](const auto& elem) { return elem == value; });
    }

    template<__internal::legacy_input_iterator InputIterator, class Predicate>
    constexpr InputIterator find_if(InputIterator first, InputIterator last, Predicate pred) {
        for (; first != last; first++) {
            if (pred(*first)) {
                return first;
            }
        }

        return last;
    }

    /* 25.7.8 Remove */
    template<__internal::legacy_forward_iterator ForwardIterator, class T>
    requires is_move_assignable_v<typename ForwardIterator::reference>
    constexpr ForwardIterator remove(ForwardIterator first, ForwardIterator last, const T& value) {
        return remove_if(first, last, [&](const auto& elem) { return value == elem; } );
    }

    template<__internal::legacy_forward_iterator ForwardIterator, class Pred>
    requires is_move_assignable_v<typename ForwardIterator::reference>
    constexpr ForwardIterator remove_if(ForwardIterator first, ForwardIterator last, Pred pred) {
        ForwardIterator overwrite_it = find_if(first, last, pred);
        if (overwrite_it == last) {
            return overwrite_it;
        }

        for (ForwardIterator write_it = next(overwrite_it); write_it != last; write_it++) {
            if (!pred(*write_it)) {
                *overwrite_it = move(*write_it);
                overwrite_it++;
            }
        }

        return next(overwrite_it);
    }

    /* 25.7.11 Rotate */
    template<__internal::legacy_forward_iterator ForwardIterator>
    requires requires (ForwardIterator it) {
        requires is_swappable_v<decltype(*it)>;
        requires is_move_assignable_v<decltype(*it)>;
        requires is_move_constructible_v<decltype(*it)>;
    }
    constexpr ForwardIterator rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last) {
        // TODO: Implement.
        return first;
    }

    /* 25.8.9 Minimum and maximum */
    template<class T> 
    requires requires (const T& a, const T& b) { { a < b } -> convertible_to<bool>; }
    constexpr const T& min(const T& a, const T& b) { 
        return a <= b ? a : b; 
    }

    template<class T, class Compare>
    constexpr const T& min(const T& a, const T& b, Compare cmp) { 
        return (cmp(a, b) || a == b) ? a : b; 
    }

    template<class T> 
    requires is_copy_constructible_v<T> && requires (const T& a, const T& b) { { a < b } -> convertible_to<bool>; }
    constexpr T min(initializer_list<T> r) {
        constexpr auto compare_r = [](const T* begin, const T* end, auto& compare_r ) {
            if (begin + 1 == end) {
                return *begin;
            } else {
                return min(*begin, compare_r(begin + 1, end, compare_r));
            }
        };

        return compare_r(r.begin(), r.end(), compare_r);
    }

    template<class T, class Compare> 
    requires is_copy_constructible_v<T>
    constexpr T min(initializer_list<T> r, Compare cmp) {
        constexpr auto compare_r = [cmp](const T* begin, const T* end, auto& compare_r) {
            if (begin + 1 == end) {
                return *begin;
            } else {
                return min(*begin, compare_r(begin + 1, end, compare_r), cmp);
            }
        };

        return compare_r(r.begin(), r.end(), compare_r);
    }

    template<class T> 
    requires requires (const T& a, const T& b) { { a < b } -> convertible_to<bool>; }
    constexpr const T& max(const T& a, const T& b) { 
        return a >= b ? a : b; 
    }

    template<class T, class Compare>
    constexpr const T& max(const T& a, const T& b, Compare cmp) { 
        return (!cmp(a, b) || a == b) ? a : b; 
    }

    template<class T> 
    requires is_copy_constructible_v<T> && requires (const T& a, const T& b) { { a < b } -> convertible_to<bool>; }
    constexpr T max(initializer_list<T> r) {
        constexpr auto compare_r = [](const T* begin, const T* end, auto& compare_r) -> T {
            if (begin + 1 == end) {
                return *begin;
            } else {
                return max(*begin, compare_r(begin + 1, end, compare_r));
            }
        };

        return compare_r(r.begin(), r.end(), compare_r);
    }

    template<class T, class Compare> 
    requires is_copy_constructible_v<T>
    constexpr T max(initializer_list<T> r, Compare cmp) {
        constexpr auto compare_r = [cmp](const T* begin, const T* end, auto& compare_r) {
            if (begin + 1 == end) {
                return *begin;
            } else {
                return max(*begin, compare_r(begin + 1, end, compare_r), cmp);
            }
        };

        return compare_r(r.begin(), r.end(), compare_r);
    }

    /* 25.8.11 Lexicographical comparison */
    template<__internal::legacy_input_iterator InputIterator1, __internal::legacy_input_iterator InputIterator2>
    constexpr auto lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2) -> decltype(*first1 <=> *first2) {
        return lexicographical_compare(first1, last1, first2, last2, compare_three_way());
    }

    template<__internal::legacy_input_iterator InputIterator1, __internal::legacy_input_iterator InputIterator2, class Compare>
    constexpr auto lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, Compare comp) -> decltype(comp(*first1, *first2)) 
    requires is_same_v<strong_ordering, decltype(comp(*first1, *last1))> || is_same_v<partial_ordering, decltype(comp(*first1, *last1))>
        || is_same_v<weak_ordering, decltype(comp(*first1, *last1))> {
        bool first_ended = first1 == last1;
        bool second_ended = first2 == last2;

        while (!first_ended && !second_ended) {
            if (const auto res = comp(*first1, *first2); res != 0) {
                return res;
            }

            first1++;
            first2++;
            first_ended = first1 == last1;
            second_ended = first2 == last2;
        }

        return !first_ended ? strong_ordering::greater : !second_ended ? strong_ordering::less : strong_ordering::equal;
    }
}