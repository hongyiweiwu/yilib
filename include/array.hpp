#pragma once

#include "compare.hpp"
#include "initializer_list.hpp"
#include "cstddef.hpp"
#include "iterator.hpp"
#include "utility.hpp"
#include "type_traits.hpp"
#include "__tuple.hpp"

namespace std {
    /* 22.3.7 Class template array */
    template<class T, std::size_t N>
    struct array {
        T __elems[N];

        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using iterator = T*;
        using const_iterator = const T*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        constexpr void fill(const T& u) {
            for (T& x: __elems) {
                x = u;
            }
        }

        constexpr void swap(array& other) noexcept(is_nothrow_swappable_v<T>) {
            using std::swap;
            for (std::size_t i = 0; i < N; i++) {
                swap(__elems[i], other.__elems[i]);
            }
        }

        constexpr iterator begin() noexcept {
            return __elems;
        }

        constexpr const_iterator begin() const noexcept {
            return __elems;
        }

        constexpr iterator end() noexcept {
            return __elems + N;
        }

        constexpr const_iterator end() const noexcept {
            return __elems + N;
        }

        constexpr reverse_iterator rbegin() noexcept {
            return reverse_iterator(__elems + N);
        }

        constexpr const_reverse_iterator rbegin() const noexcept {
            return reverse_iterator(__elems + N);
        }

        constexpr reverse_iterator rend() noexcept {
            return reverse_iterator(__elems);
        }

        constexpr const_reverse_iterator rend() const noexcept {
            return reverse_iterator(__elems);
        }

        constexpr const_iterator cbegin() const noexcept {
            return begin();
        }

        constexpr const_iterator cend() const noexcept {
            return end();
        }

        constexpr const_reverse_iterator crbegin() const noexcept {
            return rbegin();
        }

        constexpr const_reverse_iterator crend() const noexcept {
            return rend();
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return N == 0;
        }

        constexpr size_type size() const noexcept {
            return N;
        }

        constexpr size_type max_size() const noexcept {
            return N;
        }

        constexpr reference operator[](size_type n) {
            return __elems[n];
        }

        constexpr const_reference operator[](size_type n) const {
            return __elems[n];
        }

        constexpr reference at(size_type n) {
            return operator[](n);
        }

        constexpr const_reference at(size_type n) const {
            return operator[](n);
        }

        constexpr reference front() {
            return at(0);
        }

        constexpr const_reference front() const {
            return at(0);
        }

        constexpr reference back() {
            return at(N - 1);
        }

        constexpr const_reference back() const {
            return at(N - 1);
        }

        constexpr T* data() noexcept {
            return __elems;
        }

        constexpr const T* data() const noexcept {
            return __elems;
        }
    };

    template<class T, class ...U> requires (is_same_v<T, U> && ...)
    array(T, U...) -> array<T, 1 + sizeof...(U)>;

    template<class T>
    struct array<T, 0> {
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using iterator = T*;
        using const_iterator = const T*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        constexpr void fill(const T&) {}

        constexpr void swap(array&) noexcept(is_nothrow_swappable_v<T>) {}

        constexpr iterator begin() noexcept {
            return nullptr;
        }

        constexpr const_iterator begin() const noexcept {
            return nullptr;
        }

        constexpr iterator end() noexcept {
            return nullptr;
        }

        constexpr const_iterator end() const noexcept {
            return nullptr;
        }

        constexpr reverse_iterator rbegin() noexcept {
            return reverse_iterator(nullptr);
        }

        constexpr const_reverse_iterator rbegin() const noexcept {
            return reverse_iterator(nullptr);
        }

        constexpr reverse_iterator rend() noexcept {
            return reverse_iterator(nullptr);
        }

        constexpr const_reverse_iterator rend() const noexcept {
            return reverse_iterator(nullptr);
        }

        constexpr const_iterator cbegin() const noexcept {
            return begin();
        }

        constexpr const_iterator cend() const noexcept {
            return end();
        }

        constexpr const_reverse_iterator crbegin() const noexcept {
            return rbegin();
        }

        constexpr const_reverse_iterator crend() const noexcept {
            return rend();
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return true;
        }

        constexpr size_type size() const noexcept {
            return 0;
        }

        constexpr size_type max_size() const noexcept {
            return 0;
        }

        constexpr reference operator[](size_type n) {
            return 0;
        }

        constexpr const_reference operator[](size_type n) const {
            return 0;
        }

        constexpr reference at(size_type n) {
            return operator[](n);
        }

        constexpr const_reference at(size_type n) const {
            return operator[](n);
        }

        constexpr reference front() {
            return at(0);
        }

        constexpr const_reference front() const {
            return at(0);
        }

        constexpr reference back() {
            return at(0);
        }

        constexpr const_reference back() const {
            return at(0);
        }

        constexpr T* data() noexcept {
            return nullptr;
        }

        constexpr const T* data() const noexcept {
            return nullptr;
        }
    };

    template<class T, std::size_t N>
    constexpr bool operator==(const array<T, N>& x, const array<T, N>& y) {
        if constexpr (N != 0) {
            for (std::size_t i = 0; i < N; i++) {
                if (x[i] != y[i]) {
                    return false;
                }
            }
        }
        
        return true;
    }

    template<class T, std::size_t N>
    constexpr __internal::synth_three_way_result<T> operator<=>(const array<T, N>& x, const array<T, N>& y) {
        if constexpr (N != 0) {
            for (std::size_t i = 0; i < N; i++) {
                if (const auto cmp = __internal::synth_three_way(x[i], y[i]); cmp != 0) {
                    return cmp;
                }
            }
        }

        return strong_ordering::equal;
    }

    /* 22.3.7.4 Specialized algorithms */
    template<class T, std::size_t N> requires (N == 0) || is_swappable_v<T>
    constexpr void swap(array<T, N>& x, array<T, N>& y) noexcept(noexcept(x.swap(y))) {
        x.swap(y);
    }


    /* 22.3.7.6 Array creation functions */
    template<class T, std::size_t N> requires (!is_array_v<T>) && is_copy_constructible_v<T>
    constexpr array<remove_cv_t<T>, N> to_array(T (&a)[N]) {
        constexpr auto helper = []<std::size_t ...I>(T (&a)[N], index_sequence<I...>) {
            return array<remove_cv_t<T>, N>{{ a[I]... }};
        };

        return helper(a);
    }

    template<class T, std::size_t N> requires (!is_array_v<T>) && is_move_constructible_v<T>
    constexpr array<remove_cv_t<T>, N> to_array(T (&&a)[N]) {
        constexpr auto helper = []<std::size_t ...I>(T (&&a)[N], index_sequence<I...>) {
            return array<remove_cv_t<T>, N>{{ move(a[I])... }};
        };

        return helper(move(a));
    }

    template<class T, std::size_t N>
    struct tuple_size<array<T, N>> : integral_constant<std::size_t, N> {};

    template<std::size_t I, class T, std::size_t N> requires (I < N)
    struct tuple_element<I, array<T, N>> {
        using type = T;
    };

    template<std::size_t I, class T, std::size_t N> requires (I < N)
    constexpr T& get(array<T, N>& arr) noexcept {
        return arr[I];
    }

    template<std::size_t I, class T, std::size_t N> requires (I < N)
    constexpr T&& get(array<T, N>&& arr) noexcept {
        return move(arr[I]);
    }

    template<std::size_t I, class T, std::size_t N> requires (I < N)
    constexpr const T& get(const array<T, N>& arr) noexcept {
        return arr[I];
    }

    template<std::size_t I, class T, std::size_t N> requires (I < N)
    constexpr const T&& get(const array<T, N>&& arr) noexcept {
        return move(arr[I]);
    }
}