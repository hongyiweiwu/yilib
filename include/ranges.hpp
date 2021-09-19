#pragma once

#include "type_traits.hpp"
#include "iterator.hpp"
#include "type_traits/is_type.hpp"
#include "util/utility_traits.hpp"
#include "limits.hpp"
#include "memory.hpp"

namespace std::ranges {
    // NOTE: Range access functions are implemented in "iterator.hpp".

    /* 24.4.2 Ranges */
    template<class T>
    concept range = requires (T& t) {
        ranges::begin(t);
        ranges::end(t);
    };

    template<class T>
    concept borrowed_range = range<T> && (is_lvalue_reference_v<T> || enable_borrowed_range<remove_cvref_t<T>>);

    template<class T>
    using iterator_t = decltype(ranges::begin(declval<T&>()));

    template<range R>
    using sentinel_t = decltype(ranges::end(declval<R&>()));

    template<range R>
    using range_difference_t = iter_difference_t<iterator_t<R>>;

    template<range R>
    requires requires (R& r) { ranges::size(r); }
    using range_size_t = decltype(ranges::size(declval<R&>()));

    template<range R>
    using range_value_t = iter_value_t<iterator_t<R>>;

    template<range R>
    using range_reference_t = iter_reference_t<iterator_t<R>>;

    template<range R>
    using range_rvalue_reference_t = iter_rvalue_reference_t<iterator_t<R>>;

    /* 24.4.3 Sized ranges */
    template<class T>
    concept sized_range = range<T> && requires (T& t) { ranges::size(t); };

    /* 24.4.4 Views */
    struct view_base {};

    template<class T>
    inline constexpr bool enable_view = derived_from<T, view_base>;

    template<class T>
    concept view = range<T> && movable<T> && default_initializable<T> && enable_view<T>;

    /* 24.4.5 Other range refinements. */
    template<class R, class T>
    concept output_range = range<R> && output_iterator<iterator_t<R>, T>;

    template<class T>
    concept input_range = range<T> && input_iterator<iterator_t<T>>;

    template<class T>
    concept forward_range = input_range<T> && forward_iterator<iterator_t<T>>;

    template<class T>
    concept bidirectional_range = forward_range<T> && bidirectional_iterator<iterator_t<T>>;

    template<class T>
    concept random_access_range = bidirectional_range<T> && random_access_iterator<iterator_t<T>>;

    template<class T>
    concept contiguous_range = random_access_range<T> && contiguous_iterator<iterator_t<T>> && requires (T& t) {
        { ranges::data(t) } -> same_as<add_pointer_t<range_reference_t<T>>>;
    };

    template<class T>
    concept common_range = range<T> && same_as<iterator_t<T>, sentinel_t<T>>;

    template<class T>
    concept viewable_range = range<T> && (borrowed_range<T> || view<remove_cvref_t<T>>);

    namespace __internal {
        template<class R>
        concept simple_view = view<R> && range<const R> && same_as<iterator_t<R>, iterator_t<const R>>
            && same_as<sentinel_t<R>, sentinel_t<const R>>;

        template<class I>
        concept has_arrow = input_iterator<I> && (is_pointer_v<I> || requires (I i) { i.operator->(); });

        template<class T, class U>
        concept not_same_as = !same_as<remove_cvref_t<T>, remove_cvref_t<U>>;
    }

    template<class D>
    requires is_class_v<D> && same_as<D, remove_cv_t<D>>
    class view_interface : public view_base {
    private:
        constexpr D& derived() noexcept {
            return static_cast<D&>(*this);
        }

        constexpr const D& derived() const noexcept {
            return static_cast<const D&>(*this);
        }
    public:
        constexpr bool empty()
        requires forward_range<D> {
            return ranges::begin(derived()) == ranges::end(derived());
        }

        constexpr bool empty() const
        requires forward_range<const D> {
            return ranges::begin(derived()) == ranges::end(derived());
        }

        constexpr explicit operator bool()
        requires requires { ranges::empty(derived()); } {
            return !ranges::empty(derived());
        }

        constexpr explicit operator bool() const
        requires requires { ranges::empty(derived()); } {
            return !ranges::empty(derived());
        }

        constexpr auto data()
        requires contiguous_iterator<iterator_t<D>> {
            return to_address(ranges::begin(derived()));
        }

        constexpr auto data() const
        requires range<const D> && contiguous_iterator<iterator_t<const D>> {
            return to_address(ranges::begin(derived()));
        }

        constexpr auto size()
        requires forward_range<D> && sized_sentinel_for<sentinel_t<D>, iterator_t<D>> {
            return ranges::end(derived()) - ranges::begin(derived());
        }

        constexpr auto size() const
        requires forward_range<const D> && sized_sentinel_for<sentinel_t<const D>, iterator_t<const D>> {
            return ranges::end(derived()) - ranges::begin(derived());
        }

        constexpr decltype(auto) front()
        requires forward_range<D> {
            return *ranges::begin(derived());
        }

        constexpr decltype(auto) front() const
        requires forward_range<const D> {
            return *ranges::begin(derived());
        }

        constexpr decltype(auto) back()
        requires bidirectional_range<D> && common_range<D> {
            return *ranges::prev(ranges::end(derived()));
        }

        constexpr decltype(auto) back() const
        requires bidirectional_range<const D> && common_range<const D> {
            return *ranges::prev(ranges::end(derived()));
        }

        template<random_access_range R = D>
        constexpr decltype(auto) operator[](range_difference_t<R> n) {
            return ranges::begin(derived())[n];
        }

        template<random_access_range R = D>
        constexpr decltype(auto) operator[](range_difference_t<R> n) const {
            return ranges::begin(derived())[n];
        }
    };

    /* 24.5.4 Sub-ranges */
    enum class subrange_kind : bool {
        unsized, sized
    };

    namespace __internal {
        template<input_or_output_iterator I, bool StoreSize>
        struct __subrange_base {};

        template<input_or_output_iterator I>
        struct __subrange_base<I, true> {
        protected:
            make_unsigned_t<iter_difference_t<I>> s = 0;
        };

        template<class T>
        concept pair_like = !is_reference_v<T> && requires (T t) {
            typename tuple_size<T>::type;
            requires derived_from<tuple_size<T>, integral_constant<std::size_t, 2>>;
            typename tuple_element_t<0, remove_const_t<T>>;
            typename tuple_element_t<1, remove_const_t<T>>;
            { get<0>(t) } -> convertible_to<const tuple_element_t<0, T>&>;
            { get<1>(t) } -> convertible_to<const tuple_element_t<1, T>&>;
        };
    }

    template<input_or_output_iterator I, sentinel_for<I> S = I, subrange_kind K = sized_sentinel_for<S, I> ? subrange_kind::sized : subrange_kind::unsized>
    requires (K == subrange_kind::sized) || (!sized_sentinel_for<S, I>)
    class subrange : public view_interface<subrange<I, S, K>>, public __internal::__subrange_base<I, K == subrange_kind::sized && !sized_sentinel_for<S, I>> {
    private:
        template<class From, class To>
        static constexpr bool convertible_to_non_slicing = convertible_to<From, To> &&
            !(is_pointer_v<decay_t<From>> && is_pointer_v<decay_t<To>> && __internal::not_same_as<remove_pointer_t<decay_t<From>>, remove_pointer_t<decay_t<To>>>);

        template<class T, class U, class V>
        static constexpr bool pair_like_convertible_from = !range<T> && __internal::pair_like<T>
            && constructible_from<T, U, V> && convertible_to_non_slicing<U, tuple_element_t<0, T>>
            && convertible_to_non_slicing<V, tuple_element_t<1, T>>;

        static constexpr bool store_size = K == subrange_kind::sized && !sized_sentinel_for<S, I>;

        I head = I();
        S tail = S();
    public:
        subrange() = default;

        template<class T>
        requires convertible_to_non_slicing<T, I> && (!store_size)
        constexpr subrange(T i, S s) : view_interface<subrange<I, S, K>>(), __internal::__subrange_base<I, store_size>(), head(move(i)), tail(s) {}

        template<class T>
        requires convertible_to_non_slicing<T, I> && (K == subrange_kind::sized)
        constexpr subrange(T i, S s, make_unsigned_t<iter_difference_t<I>> n)
            : view_interface<subrange<I, S, K>>(), __internal::__subrange_base<I, store_size>(), head(move(i)), tail(s) {
            if constexpr (store_size) {
                s = n;
            }
        }

        template<__internal::not_same_as<subrange> R>
        requires borrowed_range<R> && convertible_to_non_slicing<iterator_t<R>, I> && convertible_to<sentinel_t<R>, S> && (!store_size)
        constexpr subrange(R&& r) : subrange(ranges::begin(r), ranges::end(r)) {}

        template<__internal::not_same_as<subrange> R>
        requires borrowed_range<R> && convertible_to_non_slicing<iterator_t<R>, I> && convertible_to<sentinel_t<R>, S> && store_size && sized_range<R>
        constexpr subrange(R&& r) : subrange(r, ranges::size(r)) {}

        template<borrowed_range R>
        requires convertible_to_non_slicing<iterator_t<R>, I> && convertible_to<sentinel_t<R>, S> && (K == subrange_kind::sized)
        constexpr subrange(R&& r, make_unsigned_t<iter_difference_t<I>> n) : subrange(ranges::begin(r), ranges::end(r), n) {}

        template<__internal::not_same_as<subrange> PairLike>
        requires pair_like_convertible_from<PairLike, const I&, const S&>
        constexpr operator PairLike() const {
            return PairLike(head, tail);
        }

        constexpr I begin() const
        requires copyable<I> {
            return head;
        }

        [[nodiscard]] constexpr I begin()
        requires (!copyable<I>) {
            return move(head);
        }

        constexpr S end() const {
            return tail;
        }

        constexpr bool empty() const {
            return head == tail;
        }

        constexpr make_unsigned_t<iter_difference_t<I>> size() const
        requires (K == subrange_kind::sized) {
            if constexpr (store_size) {
                return this->s;
            } else {
                return static_cast<make_signed_t<iter_difference_t<I>>>(tail - head);
            }
        }

        [[nodiscard]] constexpr subrange next(iter_difference_t<I> n = 1) const &
        requires forward_iterator<I> {
            subrange tmp = *this;
            tmp.advance(n);
            return tmp;
        }

        [[nodiscard]] constexpr subrange next(iter_difference_t<I> n = 1) && {
            advance(n);
            return move(*this);
        }

        [[nodiscard]] constexpr subrange prev(iter_difference_t<I> n = 1) const
        requires bidirectional_iterator<I> {
            subrange tmp = *this;
            tmp.advance(-n);
            return tmp;
        }

        constexpr subrange& advance(iter_difference_t<I> n) {
            if constexpr (store_size) {
                const iter_difference_t<I> d = n - ranges::advance(head, n, tail);
                if (d >= 0) {
                    this->s -= static_cast<make_unsigned_t<iter_difference_t<I>>>(d);
                } else {
                    this->s += static_cast<make_unsigned_t<iter_difference_t<I>>>(-d);
                }
            } else {
                ranges::advance(head, n, tail);
            }

            return *this;
        }
    };

    template<input_or_output_iterator I, sentinel_for<I> S>
    subrange(I, S) -> subrange<I, S>;

    template<input_or_output_iterator I, sentinel_for<I> S>
    subrange(I, S, make_unsigned_t<iter_difference_t<I>>) -> subrange<I, S, subrange_kind::sized>;

    template<class P>
    requires (!range<P>) && __internal::pair_like<P> && sentinel_for<tuple_element_t<1, P>, tuple_element_t<0, P>>
    subrange(P) -> subrange<tuple_element_t<0, P>, tuple_element_t<1, P>>;

    template<class P>
    requires (!range<P>) && __internal::pair_like<P> && sentinel_for<tuple_element_t<1, P>, tuple_element_t<0, P>>
    subrange(P, make_unsigned_t<iter_difference_t<tuple_element_t<0, P>>>) -> subrange<tuple_element_t<0, P>, tuple_element_t<1, P>, subrange_kind::sized>;

    template<borrowed_range R>
    subrange(R&&) -> subrange<iterator_t<R>, sentinel_t<R>, 
        (sized_range<R> || sized_sentinel_for<sentinel_t<R>, iterator_t<R>>) ? subrange_kind::sized : subrange_kind::unsized>;

    template<borrowed_range R>
    subrange(R&&, make_unsigned_t<range_difference_t<R>>) -> subrange<iterator_t<R>, sentinel_t<R>, subrange_kind::sized>;

    template<std::size_t N, class I, class S, subrange_kind K>
    requires (N < 2)
    constexpr auto get(const subrange<I, S, K>& r) {
        if constexpr (N == 0) {
            return r.begin();
        } else {
            return r.end();
        }
    }

    template<std::size_t N, class I, class S, subrange_kind K>
    requires (N < 2)
    constexpr auto get(subrange<I, S, K>& r) {
        if constexpr (N == 0) {
            return r.begin();
        } else {
            return r.end();
        }
    }

    template<input_or_output_iterator I, sentinel_for<I> S, subrange_kind K>
    inline constexpr bool enable_borrowed_range<subrange<I, S, K>> = true;
}

namespace std {
    using ranges::get;
}

namespace std::ranges {
    /* 24.5.5 Dangling iterator handling */
    struct dangling {
        constexpr dangling() noexcept = default;

        template<class ...Args>
        constexpr dangling(Args&&...) noexcept {}
    };

    template<range R>
    using borrowed_iterator_t = conditional_t<borrowed_range<R>, iterator_t<R>, dangling>;

    template<range R>
    using borrowed_subrange_t = conditional_t<borrowed_range<R>, subrange<iterator_t<R>>, dangling>;
}