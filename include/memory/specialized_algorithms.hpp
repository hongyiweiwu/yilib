#pragma once

#include "concepts.hpp"
#include "memory/pointer_util.hpp"
#include "memory/construct_destroy.hpp"
#include "iterator.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "ranges.hpp"
#include "algorithm.hpp"

// TODO: Implement parallel algorithms.
namespace std {
    /* 25.11.2 Special memory concepts */
    namespace __internal {
        template<class I>
        concept no_throw_input_iterator = input_iterator<I> && is_lvalue_reference_v<iter_reference_t<I>>
            && same_as<remove_cvref_t<iter_reference_t<I>>, iter_value_t<I>>;

        template<class S, class I>
        concept no_throw_sentinel_for = sentinel_for<S, I>;

        template<class R>
        concept no_throw_input_range = ranges::range<R> && no_throw_input_iterator<ranges::iterator_t<R>>
            && no_throw_sentinel_for<ranges::sentinel_t<R>, ranges::iterator_t<R>>;

        template<class I>
        concept no_throw_forward_iterator = no_throw_input_iterator<I> && forward_iterator<I> && no_throw_sentinel_for<I, I>;

        template<class R>
        concept no_throw_forward_range = no_throw_input_range<R> && no_throw_forward_iterator<ranges::iterator_t<R>>;

        /* Casts a pointer to any time to a `void*`. */
        template<class T>
        static constexpr void* voidify(T* const ptr) noexcept {
            return const_cast<void*>(static_cast<const volatile void*>(ptr));
        }
    }

    /* 25.11.3 uninitialized_default_construct */
    template<__internal::legacy_forward_iterator ForwardIt>
    void uninitialized_default_construct(ForwardIt first, ForwardIt last) {
        ForwardIt current = first;
        try {
            for (; current != last; current++) {
                ::new (__internal::voidify(addressof(*current))) typename iterator_traits<ForwardIt>::value_type;
            }
        } catch (...) {
            destroy(first, current);
            throw;
        }
    }

    template<__internal::legacy_forward_iterator ForwardIt, class Size>
    ForwardIt uninitialized_default_construct_n(ForwardIt first, Size n) {
        return uninitialized_default_construct(counted_iterator(first, n), default_sentinel).base();
    }

    namespace ranges {
        namespace __internal {
            using ::std::__internal::no_throw_forward_iterator;
            using ::std::__internal::no_throw_input_iterator;
            using ::std::__internal::no_throw_sentinel_for;
            using ::std::__internal::no_throw_forward_range;
            using ::std::__internal::no_throw_input_range;
            using ::std::__internal::voidify;
        }

        struct __uninitialized_default_construct_fn {
        public:
            template<__internal::no_throw_forward_iterator I, __internal::no_throw_sentinel_for<I> S>
            requires default_initializable<iter_value_t<I>>
            I operator()(I first, S last) const {
                using value_type = remove_reference_t<iter_reference_t<I>>;
                if constexpr (is_trivially_default_constructible_v<value_type>) {
                    return ranges::next(first, last);
                }

                I current = first;
                try {
                    for (; current != last; current++) {
                        ::new (__internal::voidify(addressof(*current))) value_type;
                    }
                    return current;
                } catch (...) {
                    for (; first != current; first++) {
                        destroy_at(addressof(*first));
                    }
                    throw;
                }
            }

            template<__internal::no_throw_forward_range R>
            requires default_initializable<range_value_t<R>>
            borrowed_iterator_t<R> operator()(R&& r) const {
                return (*this)(ranges::begin(forward<R>(r)), ranges::end(forward<R>(r)));
            }
        };

        inline namespace __fn_objects {
            inline constexpr __uninitialized_default_construct_fn uninitialized_default_construct;
        }

        struct __uninitialized_default_construct_n_fn {
        public:
            template<__internal::no_throw_forward_iterator I>
            requires default_initializable<iter_value_t<I>>
            I operator()(I first, iter_difference_t<I> n) const {
                return uninitialized_default_construct(counted_iterator(first, n), default_sentinel).base();
            }
        };

        inline namespace __fn_objects {
            inline constexpr __uninitialized_default_construct_n_fn uninitialized_default_construct_n;
        }
    }

    /* 25.11.4 uninitialized_value_construct */
    template<__internal::legacy_forward_iterator ForwardIt>
    void uninitialized_value_construct(ForwardIt first, ForwardIt last) {
        ForwardIt current;
        try {
            for (; current != last; current++) {
                ::new (__internal::voidify(addressof(*current))) typename iterator_traits<ForwardIt>::value_type();
            }
        } catch (...) {
            destroy(first, current);
            throw;
        }
    }

    template<__internal::legacy_forward_iterator ForwardIt, class Size>
    ForwardIt uninitialized_value_construct_n(ForwardIt first, Size n) {
        return uninitialized_value_construct(counted_iterator(first, n), default_sentinel).base();
    }

    namespace ranges {
        struct __uninitialized_value_construct_fn {
        public:
            template<__internal::no_throw_forward_iterator I, __internal::no_throw_sentinel_for<I> S>
            requires default_initializable<iter_value_t<I>>
            I operator()(I first, S last) const {
                using value_type = remove_reference_t<iter_reference_t<I>>;
                I current = first;
                try {
                    for (; current != last; current++) {
                        ::new (__internal::voidify(addressof(*current))) value_type();
                    }
                    return current;
                } catch (...) {
                    for (; first != current; first++) {
                        destroy_at(addressof(*first));
                    }
                    throw;
                }
            }

            template<__internal::no_throw_forward_range R>
            requires default_initializable<range_value_t<R>>
            borrowed_iterator_t<R> operator()(R&& r) const {
                return (*this)(ranges::begin(forward<R>(r)), ranges::end(forward<R>(r)));
            }
        };

        inline namespace __fn_objects {
            inline constexpr __uninitialized_value_construct_fn uninitialized_value_construct;
        }

        struct __uninitialized_value_construct_n_fn {
        public:
            template<__internal::no_throw_forward_iterator I>
            requires default_initializable<iter_value_t<I>>
            I operator()(I first, iter_difference_t<I> n) const {
                return uninitialized_value_construct(counted_iterator(first, n), default_sentinel).base();
            }
        };

        inline namespace __fn_objects {
            inline constexpr __uninitialized_value_construct_n_fn uninitialized_value_construct_n;
        }
    }

    /* 25.11.5 uninitialized_copy */
    template<__internal::legacy_input_iterator InputIt, __internal::legacy_forward_iterator ForwardIt>
    ForwardIt uninitialized_copy(InputIt first, InputIt last, ForwardIt result) {
        ForwardIt result_first = result;
        try {
            for (; first != last; result++, first++) {
                ::new (__internal::voidify(*result)) typename iterator_traits<ForwardIt>::value_type(*first);
            }
            return result;
        } catch (...) {
            destroy(result_first, result);
            throw;
        }
    }

    template<__internal::legacy_input_iterator InputIt, class Size, __internal::legacy_forward_iterator ForwardIt>
    ForwardIt uninitialized_copy_n(InputIt first, Size n, ForwardIt result) {
        return uninitialized_copy(counted_iterator(first, n), default_sentinel, result).base();
    }

    namespace ranges {
        template<class I, class O>
        using uninitialized_copy_result = in_out_result<I, O>;

        template<class I, class O>
        using uninitialized_copy_n_result = in_out_result<I, O>;

        struct __uninitialized_copy_fn {
        public:
            template<input_iterator I, sentinel_for<I> S1, __internal::no_throw_forward_iterator O, __internal::no_throw_sentinel_for<O> S2>
            requires constructible_from<iter_value_t<O>, iter_reference_t<I>>
            uninitialized_copy_result<I, O> operator()(I ifirst, S1 ilast, O ofirst, S2 olast) const {
                O ocurrent = ofirst;
                try {
                    for (; ifirst != ilast && ocurrent != olast; ocurrent++, ifirst++) {
                        ::new (__internal::voidify(addressof(*ocurrent))) remove_reference_t<iter_reference_t<O>>(*ifirst);
                    }
                    return {move(ifirst), ocurrent};
                } catch (...) {
                    for (; ofirst != ocurrent; ofirst++) {
                        destroy_at(addressof(*ofirst));
                    }
                    throw;
                }
            }

            template<input_range IR, __internal::no_throw_forward_range OR>
            requires constructible_from<range_value_t<OR>, range_reference_t<IR>>
            uninitialized_copy_result<borrowed_iterator_t<IR>, borrowed_iterator_t<OR>> operator()(IR&& in_range, OR&& out_range) const {
                return (*this)(ranges::begin(forward<IR>(in_range)), ranges::end(forward<IR>(in_range)), ranges::begin(forward<OR>(out_range)), ranges::end(forward<OR>(out_range)));
            }
        };

        inline namespace __fn_objects {
            inline constexpr __uninitialized_copy_fn uninitialized_copy;
        }

        struct __uninitialized_copy_n_fn {
        public:
            template<input_iterator I, __internal::no_throw_forward_iterator O, __internal::no_throw_sentinel_for<O> S>
            requires constructible_from<iter_value_t<O>, iter_reference_t<I>>
            uninitialized_copy_n_result<I, O> operator()(I ifirst, iter_difference_t<I> n, O ofirst, S olast) const {
                auto t = uninitialized_copy(counted_iterator(ifirst, n), default_sentinel, ofirst, olast);
                return {move(t.in).base(), t.out};
            }
        };

        inline namespace __fn_objects {
            inline constexpr __uninitialized_copy_n_fn uninitialized_copy_n;
        }
    }

    /* 25.11.6 uninitialized_move */
    template<__internal::legacy_input_iterator InputIt, __internal::legacy_forward_iterator ForwardIt>
    ForwardIt uninitialized_move(InputIt first, InputIt last, ForwardIt result) {
        ForwardIt result_first = result;
        try {
            for (; first != last; result++, first++) {
                ::new (__internal::voidify(*result)) typename iterator_traits<ForwardIt>::value_type(*first);
            }
            return result;
        } catch (...) {
            destroy(result_first, result);
            throw;
        }
    }

    template<__internal::legacy_input_iterator InputIt, class Size, __internal::legacy_forward_iterator ForwardIt>
    pair<InputIt, ForwardIt> uninitialized_move_n(InputIt first, Size n, ForwardIt result) {
        ForwardIt result_first = result;
        try {
            for (; n > 0; result++, first++, n--) {
                ::new (__internal::voidify(*result)) typename iterator_traits<ForwardIt>::value_type(*first);
            }
            return {first, result};
        } catch (...) {
            destroy(result_first, result);
            throw;
        }
    }

    namespace ranges {
        template<class I, class O>
        using uninitialized_move_result = in_out_result<I, O>;

        template<class I, class O>
        using uninitialized_move_n_result = in_out_result<I, O>;

        struct __uninitialized_move_fn {
        public:
            template<input_iterator I, sentinel_for<I> S1, __internal::no_throw_forward_iterator O, __internal::no_throw_sentinel_for<O> S2>
            requires constructible_from<iter_value_t<O>, iter_rvalue_reference_t<I>>
            uninitialized_move_result<I, O> operator()(I ifirst, S1 ilast, O ofirst, S2 olast) const {
                O ocurrent = ofirst;
                try {
                    for (; ifirst != ilast && ocurrent != olast; ocurrent++, ifirst++) {
                        ::new (__internal::voidify(addressof(*ocurrent))) remove_reference_t<iter_reference_t<O>>(ranges::iter_move(ifirst));
                    }
                    return {move(ifirst), ocurrent};
                } catch (...) {
                    for (; ofirst != ocurrent; ofirst++) {
                        destroy_at(addressof(*ofirst));
                    }
                    throw;
                }
            }

            template<input_range IR, __internal::no_throw_forward_range OR>
            requires constructible_from<range_value_t<OR>, range_rvalue_reference_t<IR>>
            uninitialized_move_result<borrowed_iterator_t<IR>, borrowed_iterator_t<OR>> operator()(IR&& in_range, OR&& out_range) const {
                return (*this)(ranges::begin(forward<IR>(in_range)), ranges::end(forward<IR>(in_range)), ranges::begin(forward<OR>(out_range)), ranges::end(forward<OR>(out_range)));
            }
        };

        inline namespace __fn_objects {
            inline constexpr __uninitialized_move_fn uninitialized_move;
        }

        struct __uninitialized_move_n_fn {
        public:
            template<input_iterator I, __internal::no_throw_forward_iterator O, __internal::no_throw_sentinel_for<O> S>
            requires constructible_from<iter_value_t<O>, iter_rvalue_reference_t<I>>
            uninitialized_move_n_result<I, O> operator()(I ifirst, iter_difference_t<I> n, O ofirst, S olast) const {
                auto t = uninitialized_move(counted_iterator(ifirst, n), default_sentinel, ofirst, olast);
                return {move(t.in).base(), t.out};
            }
        };

        inline namespace __fn_objects {
            inline constexpr __uninitialized_move_n_fn uninitialized_move_n;
        }
    }

    /* 25.11.7 uninitialized_fill */
    template<__internal::legacy_forward_iterator ForwardIt, class T>
    void uninitialized_fill(ForwardIt first, ForwardIt last, const T& x) {
        ForwardIt current = first;
        try {
            for (; current != last; current++) {
                ::new (__internal::voidify(*current)) typename iterator_traits<ForwardIt>::value_type(x);
            }
        } catch (...) {
            destroy(first, current);
            throw;
        }
    }

    template<__internal::legacy_forward_iterator ForwardIt, class Size, class T>
    ForwardIt uninitialized_fill_n(ForwardIt first, Size n, const T& x) {
        return uninitialized_fill(counted_iterator(first, n), default_sentinel, x).base();
    }

    namespace ranges {
        struct __uninitialized_fill_fn {
        public:
            template<__internal::no_throw_forward_iterator I, __internal::no_throw_sentinel_for<I> S, class T>
            requires constructible_from<iter_value_t<I>, const T&>
            I operator()(I first, S last, const T& x) const {
                I current = first;
                try {
                    for (; current != last; current++) {
                        ::new (__internal::voidify(*current)) remove_reference_t<iter_reference_t<I>>(x);
                    }
                } catch (...) {
                    for (; first != current; first++) {
                        destroy_at(addressof(*first));
                    }
                    throw;
                }
            }

            template<__internal::no_throw_forward_range R, class T>
            requires constructible_from<ranges::range_value_t<R>, const T&>
            ranges::borrowed_iterator_t<R> operator()(R&& r, const T& x) const {
                return (*this)(ranges::begin(forward<R>(r)), ranges::end(forward<R>(r)), x);
            }
        };

        inline namespace __fn_objects {
            inline constexpr __uninitialized_fill_fn uninitialized_fill;
        }

        struct __uninitialized_fill_n_fn {
        public:
            template<__internal::no_throw_forward_range I, class T>
            requires constructible_from<iter_value_t<I>, const T&>
            I operator()(I first, iter_difference_t<I> n, const T& x) const {
                return uninitialized_fill(counted_iterator(first, n), default_sentinel, x);
            }
        };

        inline namespace __fn_objects {
            inline constexpr __uninitialized_fill_n_fn uninitialized_fill_n;
        }
    }

    /* 25.11.9 destroy */
    template<__internal::no_throw_forward_iterator ForwardIt>
    constexpr void destroy(ForwardIt first, ForwardIt last) {
        for (; first != last; first++) {
            destroy_at(addressof(*first));
        }
    }

    namespace ranges {
        struct __destroy_fn {
        public:
            template<__internal::no_throw_input_iterator I, __internal::no_throw_sentinel_for<I> S>
            requires destructible<iter_value_t<I>>
            constexpr I operator()(I first, S last) noexcept {
                for (; first != last; first++) {
                    destroy_at(addressof(*first));
                }
                return first;
            }

            template<__internal::no_throw_input_range R>
            requires destructible<range_value_t<R>>
            constexpr borrowed_iterator_t<R> operator()(R&& r) noexcept {
                return (*this)(ranges::begin(forward<R>(r)), ranges::end(forward<R>(r)));
            }
        };

        inline namespace __fn_objects {
            inline constexpr __destroy_fn destroy;
        }
    }

    template<__internal::no_throw_forward_iterator ForwardIt, class Size>
    constexpr ForwardIt destroy_n(ForwardIt first, Size n) {
        for (; n > 0; first++, n--) {
            destroy_at(addressof(*first));
        }
        return first;
    }

    namespace ranges {
        struct __destroy_n_fn {
        public:
            template<__internal::no_throw_input_iterator I>
            requires destructible<iter_value_t<I>>
            constexpr I destroy_n(I first, iter_difference_t<I> n) noexcept {
                return destroy(counted_iterator(first, n), default_sentinel).base();
            }
        };

        inline namespace __fn_objects {
            inline constexpr __destroy_n_fn destroy_n;
        }
    }
}