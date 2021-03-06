#pragma once

#include "cstddef.hpp"
#include "utility.hpp"
#include "type_traits.hpp"
#include "compare.hpp"
#include "util/utility_traits.hpp"

#include "__tuple.hpp"

namespace std {
    struct allocator_arg_t {
        explicit allocator_arg_t() = default;
    };

    namespace __internal {
        template<unsigned int I, class T>
        class __tuple_leaf {
        protected:
            [[no_unique_address]] T val;    // Enables empty class optimization for empty T.
        public:
            constexpr __tuple_leaf() = default;
            constexpr __tuple_leaf(const __tuple_leaf&) = default;
            constexpr __tuple_leaf(__tuple_leaf&&) = default;

            template<class U>
            constexpr __tuple_leaf(U&& u) : val(forward<U>(u)) {}

            template<class Alloc, class U>
            constexpr __tuple_leaf(allocator_arg_t, Alloc alloc) : val(make_obj_using_allocator(alloc)) {}

            template<class Alloc>
            constexpr __tuple_leaf(allocator_arg_t, Alloc alloc, const __tuple_leaf& other)
                : val(make_obj_using_allocator(alloc, other.val)) {}

            template<class Alloc>
            constexpr __tuple_leaf(allocator_arg_t, Alloc alloc, __tuple_leaf&& other)
                : val(make_obj_using_allocator(alloc, move(other).val)) {}

            template<class Alloc, class U>
            constexpr __tuple_leaf(allocator_arg_t, Alloc alloc, U&& u) : val(make_obj_using_allocator(alloc, forward<U>(u))) {}

            constexpr T& get_val() & noexcept {
                return val;
            }

            constexpr T&& get_val() && noexcept {
                return move(val);
            }

            constexpr const T& get_val() const& noexcept {
                return val;
            }

            constexpr const T&& get_val() const&& noexcept {
                return move(val);
            }

            constexpr void swap(__tuple_leaf& rhs) noexcept(is_nothrow_swappable_v<T>) {
                val.swap(rhs.val);
            }
        };

        template<class IndexSeq, class ...T>
        class __tuple_leaf_wrapper; // This template should never be used -- only its specialization is valid.

        // This class is completely unguarded by concepts, and do not contain any explicit restrictions. All such guards/restrictions are implemented
        // in higher-up implementations (i.e. std::tuple).
        template<std::size_t ...I, class ...T>
        class __tuple_leaf_wrapper<index_sequence<I...>, T...> : public __tuple_leaf<I, T>... {
        public:
            constexpr __tuple_leaf_wrapper() = default;
            constexpr __tuple_leaf_wrapper(const __tuple_leaf_wrapper&) = default;
            constexpr __tuple_leaf_wrapper(__tuple_leaf_wrapper&&) = default;

            constexpr __tuple_leaf_wrapper(const T& ...t)
            requires (sizeof...(T) > 0) : __tuple_leaf<I, T>(t)... {}

            template<class ...U>
            requires (sizeof...(T) > 0)
            constexpr __tuple_leaf_wrapper(U&& ...u) : __tuple_leaf<I, T>(forward<U>(u))... {}

            template<class ...U>
            requires (sizeof...(T) != 1)
                || ((!is_convertible_v<const __tuple_leaf_wrapper<U>&, T> && !is_constructible_v<T, const __tuple_leaf_wrapper<U>&> && !is_same_v<T, U>) && ...)
            constexpr __tuple_leaf_wrapper(const __tuple_leaf_wrapper<U...>& u) : __tuple_leaf<I, T>(get<I, T>(u))... {}

            template<class ...U>
            requires (sizeof...(T) != 1)
                || ((!is_convertible_v<__tuple_leaf_wrapper<U>, T> && !is_constructible_v<T, __tuple_leaf_wrapper<U>> && !is_same_v<T, U>) && ...)
            constexpr __tuple_leaf_wrapper(__tuple_leaf_wrapper<U...>&& u) : __tuple_leaf<I, T>(forward<U>(get<I, T>(u)))... {}

            template<class Alloc>
            constexpr __tuple_leaf_wrapper(allocator_arg_t, Alloc alloc) : __tuple_leaf<I, T>(allocator_arg_t{}, alloc)... {}

            template<class Alloc>
            constexpr __tuple_leaf_wrapper(allocator_arg_t, Alloc alloc, const __tuple_leaf_wrapper& other)
                : __tuple_leaf<I, T>(allocator_arg_t{}, alloc, other)... {}

            template<class Alloc>
            constexpr __tuple_leaf_wrapper(allocator_arg_t, Alloc alloc, __tuple_leaf_wrapper&& other)
                : __tuple_leaf<I, T>(allocator_arg_t{}, alloc, move(other))... {}

            template<class Alloc>
            requires (sizeof...(T) > 0)
            constexpr __tuple_leaf_wrapper(allocator_arg_t, Alloc alloc, const T& ...t)
                : __tuple_leaf<I, T>(allocator_arg_t{}, alloc, t)... {}

            template<class Alloc, class ...U>
            requires (sizeof...(T) > 0)
            constexpr __tuple_leaf_wrapper(allocator_arg_t, Alloc alloc, U&& ...u)
                : __tuple_leaf<I, T>(allocator_arg_t{}, alloc, forward<U>(u))... {}

            template<class Alloc, class ...U>
            requires (sizeof...(T) != 1)
                || ((!is_convertible_v<const __tuple_leaf_wrapper<U>&, T> && !is_constructible_v<T, const __tuple_leaf_wrapper<U>&> && !is_same_v<T, U>) && ...)
            constexpr __tuple_leaf_wrapper(allocator_arg_t, Alloc alloc, const __tuple_leaf_wrapper<U...>& u)
                : __tuple_leaf<I, T>(allocator_arg_t{}, alloc, get<I, T>(u))... {}

            template<class Alloc, class ...U>
            requires (sizeof...(T) != 1)
                || ((!is_convertible_v<__tuple_leaf_wrapper<U>, T> && !is_constructible_v<T, __tuple_leaf_wrapper<U>> && !is_same_v<T, U>) && ...)
            constexpr __tuple_leaf_wrapper(allocator_arg_t, Alloc alloc, __tuple_leaf_wrapper<U...>&& u)
                : __tuple_leaf<I, T>(allocator_arg_t{}, alloc, forward<U>(get<I, T>(u)))... {}

            constexpr void swap(__tuple_leaf_wrapper& rhs) noexcept((is_nothrow_swappable_v<T> && ...)) {
                (__tuple_leaf<I, T>::swap(rhs), ...);
            }
        };
    }

    /* 20.5.3 Class template tuple */
    template<class ...T>
    class tuple : public __internal::__tuple_leaf_wrapper<make_index_sequence<sizeof...(T)>, T...> {
    private:
        using __leaf_wrapper_t = __internal::__tuple_leaf_wrapper<make_index_sequence<sizeof...(T)>, T...>;

        template<std::size_t I>
        using __leaf_t = __internal::__tuple_leaf<I, typename __internal::pick_ith_type<I, T...>::type>;

        /* Models a type T that has an implicit default constructor. This check relies on the fact that copy-list-initialization of
         * an variable doesn't use explicit constructors. Of all the copy-list-initializations, we use the only one available as an
         * expression, which is to pass an empty initializer list into a function parameter of type T. */
        template<class U>
        static constexpr bool is_implicit_default_constructible = requires (void (&fn)(U)) { fn({}); };
    public:
        constexpr explicit((!is_implicit_default_constructible<T> || ...))
        tuple()
        requires (is_default_constructible_v<T> && ...) = default;

        constexpr explicit(!conjunction_v<is_convertible<const T&, T>...>)
        tuple(const T& ...t)
        requires (sizeof...(T) > 1) && (is_copy_constructible_v<T> && ...)
            : __leaf_wrapper_t(t...) {}

        template<class ...U>
        requires (sizeof...(T) == sizeof...(U)) && (sizeof...(T) >= 1) && (is_constructible_v<T, U> && ...)
        constexpr explicit(!conjunction_v<is_convertible<U, T>...>)
        tuple(U&& ...u) : __leaf_wrapper_t(forward<U>(u)...) {}

        tuple(const tuple& u)
        requires (is_copy_constructible_v<T> && ...) = default;

        tuple(tuple&& u)
        requires (is_move_constructible_v<T> && ...) = default;

        template<class ...U>
        requires (sizeof...(T) == sizeof...(U)) && (is_constructible_v<T, const U&> && ...)
            && (sizeof...(T) != 1 || ((!is_convertible_v<const tuple<U>&, T> && !is_constructible_v<T, const tuple<U>&> && !is_same_v<T, U>) && ...))
        constexpr explicit(!conjunction_v<is_convertible<const U&, T>...>)
        tuple(const tuple<U...>& u) : __leaf_wrapper_t(u) {}

        template<class ...U>
        requires (sizeof...(T) == sizeof...(U)) && (is_constructible_v<T, U> && ...)
            && (sizeof...(T) != 1 || ((!is_convertible_v<tuple<U>, T> && !is_constructible_v<T, tuple<U>> && !is_same_v<T, U>) && ...))
        constexpr explicit(!conjunction_v<is_convertible<U, T>...>)
        tuple(tuple<U...>&& u) : __leaf_wrapper_t(move(u)) {}

    private:
        template<class U1, class U2>
        static consteval bool __is_pair_copy_constructor_explicit() noexcept {
            if constexpr (sizeof...(T) == 2) {
                return !is_convertible_v<const U1&, typename __internal::pick_ith_type<0, T...>::type> && !is_convertible_v<const U2&, typename __internal::pick_ith_type<1, T...>::type>;
            } else {
                return false;
            }
        }

    public:
        template<class U1, class U2>
        requires (sizeof...(T) == 2)
            && requires { { bool_constant<is_constructible_v<typename __internal::pick_ith_type<0, T...>::type, const U1&>>{} } -> same_as<true_type>; }
            && requires { { bool_constant<is_constructible_v<typename __internal::pick_ith_type<1, T...>::type, const U2&>>{} } -> same_as<true_type>; }
        constexpr explicit(__is_pair_copy_constructor_explicit<U1, U2>())
        tuple(const pair<U1, U2>& u)
            : __leaf_wrapper_t(u.first, u.second) {}

    private:
        template<class U1, class U2>
        static consteval bool __is_pair_move_constructor_explicit() noexcept {
            if constexpr (sizeof...(T) == 2) {
                return !is_convertible_v<U1, typename __internal::pick_ith_type<0, T...>::type> && !is_convertible_v<U2, typename __internal::pick_ith_type<1, T...>::type>;
            } else {
                return false;
            }
        }
    public:
        template<class U1, class U2>
        requires (sizeof...(T) == 2)
            && requires { { bool_constant<is_constructible_v<typename __internal::pick_ith_type<0, T...>::type, U1>>{} } -> same_as<true_type>; }
            && requires { { bool_constant<is_constructible_v<typename __internal::pick_ith_type<1, T...>::type, U2>>{} } -> same_as<true_type>; }
        constexpr explicit(__is_pair_move_constructor_explicit<U1, U2>())
        tuple(pair<U1, U2>&& u) : __leaf_wrapper_t(forward<U1>(u.first), forward<U2>(u.second)) {}

        template<class Alloc>
        requires (is_default_constructible_v<T> && ...)
        constexpr explicit((!is_implicit_default_constructible<T> || ...))
        tuple(allocator_arg_t, Alloc a) : __leaf_wrapper_t(allocator_arg_t{}, a) {}

        template<class Alloc>
        requires (sizeof...(T) > 1) && (is_copy_constructible_v<T> && ...)
        constexpr explicit(!conjunction_v<is_convertible<const T&, T>...>)
        tuple(allocator_arg_t, Alloc a, const T& ...t) : __leaf_wrapper_t(allocator_arg_t{}, a, t...) {}

        template<class Alloc, class ...U>
        requires (sizeof...(T) == sizeof...(U)) && (sizeof...(T) >= 1) && (is_constructible_v<T, U> && ...)
        constexpr explicit(!conjunction_v<is_convertible<U, T>...>)
        tuple(allocator_arg_t, Alloc a, U&& ...u) : __leaf_wrapper_t(allocator_arg_t{}, a, forward<U>(u)...) {}

        template<class Alloc>
        requires (is_copy_constructible_v<T> && ...)
        tuple(allocator_arg_t, Alloc a, const tuple& u) : __leaf_wrapper_t(allocator_arg_t{}, a, u) {}

        template<class Alloc>
        requires (is_move_constructible_v<T> && ...)
        tuple(allocator_arg_t, Alloc a, tuple&& u) : __leaf_wrapper_t(allocator_arg_t{}, a, move(u)) {}

        template<class Alloc, class ...U>
        requires (sizeof...(T) == sizeof...(U)) && (is_constructible_v<T, const U&> && ...)
            && (sizeof...(T) != 1 || ((!is_convertible_v<const tuple<U>&, T> && !is_constructible_v<T, const tuple<U>&> && !is_same_v<T, U>) && ...))
        constexpr explicit(!conjunction_v<is_convertible<const U&, T>...>)
        tuple(allocator_arg_t, Alloc a, const tuple<U...>& u) : __leaf_wrapper_t(allocator_arg_t{}, a, u) {}

        template<class Alloc, class ...U>
        requires (sizeof...(T) == sizeof...(U)) && (is_constructible_v<T, U> && ...)
            && (sizeof...(T) != 1 || ((!is_convertible_v<tuple<U>, T> && !is_constructible_v<T, tuple<U>> && !is_same_v<T, U>) && ...))
        constexpr explicit(!conjunction_v<is_convertible<U, T>...>)
        tuple(allocator_arg_t, Alloc a, tuple<U...>&& u) : __leaf_wrapper_t(allocator_arg_t{}, a, move(u)) {}

        template<class Alloc, class U1, class U2>
        requires (sizeof...(T) == 2)
            && requires { { bool_constant<is_constructible_v<typename __internal::pick_ith_type<0, T...>::type, const U1&>>{} } -> same_as<true_type>; }
            && requires { { bool_constant<is_constructible_v<typename __internal::pick_ith_type<1, T...>::type, const U2&>>{} } -> same_as<true_type>; }
        constexpr explicit(__is_pair_copy_constructor_explicit<U1, U2>())
        tuple(allocator_arg_t, Alloc a, const pair<U1, U2>& u) : __leaf_wrapper_t(allocator_arg_t{}, a, u.first, u.second) {}

        template<class Alloc, class U1, class U2>
        requires (sizeof...(T) == 2)
            && requires { { bool_constant<is_constructible_v<typename __internal::pick_ith_type<0, T...>::type, U1>>{} } -> same_as<true_type>; }
            && requires { { bool_constant<is_constructible_v<typename __internal::pick_ith_type<1, T...>::type, U2>>{} } -> same_as<true_type>; }
        constexpr explicit(__is_pair_move_constructor_explicit<U1, U2>())
        tuple(allocator_arg_t, Alloc a, pair<U1, U2>&& u) : __leaf_wrapper_t(allocator_arg_t{}, a, move(u).first, move(u).second) {}

    public:
        constexpr tuple& operator=(const tuple& tpl)
        requires (is_copy_assignable_v<T> && ...) {
            constexpr auto helper = []<std::size_t ...I>(tuple& self, const tuple& tpl) {
                ((self.__leaf_t<I>::get_val() = tpl.__leaf_t<I>::get_val()), ...);
            };

            helper(*this, tpl);
            return *this;
        }

        constexpr tuple& operator=(tuple&& tpl)
        noexcept((is_nothrow_move_assignable_v<T> && ...))
        requires (is_move_assignable_v<T> && ...) {
            constexpr auto helper = []<std::size_t ...I>(tuple& self, tuple&& tpl) {
                ((self.__leaf_t<I>::get_val() = forward<T>(tpl.__leaf_t<I>::get_val())), ...);
            };

            helper(*this, move(tpl));
            return *this;
        }

        template<class ...U>
        requires (sizeof...(T) == sizeof...(U)) && (is_assignable_v<T&, const U&> && ...)
        constexpr tuple& operator=(const tuple<U...>& tpl) {
            constexpr auto helper = []<std::size_t ...I>(tuple& self, const tuple<U...>& tpl) {
                ((self.__leaf_t<I>::get_val() = tpl.__leaf_t<I>::get_val()), ...);
            };

            helper(*this, tpl);
            return *this;
        }

        template<class ...U>
        requires (sizeof...(T) == sizeof...(U)) && (is_assignable_v<T&, U> && ...)
        constexpr tuple& operator=(tuple<U...>&& tpl) {
            constexpr auto helper = []<std::size_t ...I>(tuple& self, tuple&& tpl) {
                ((self.__leaf_t<I>::get_val() = forward<U>(tpl.__leaf_t<I>::get_val())), ...);
            };

            helper(*this, tpl);
            return *this;
        }

        template<class U1, class U2>
        requires (sizeof...(T) == 2)
            && requires { { bool_constant<is_assignable_v<typename __internal::pick_ith_type<0, T...>::type&, const U1&>>{} } -> same_as<true_type>; }
            && requires { { bool_constant<is_assignable_v<typename __internal::pick_ith_type<1, T...>::type&, const U2&>>{} } -> same_as<true_type>; }
        constexpr tuple& operator=(const pair<U1, U2>& p) {
            __leaf_t<0>::get_val() = p.first;
            __leaf_t<1>::get_val() = p.second;
            return *this;
        }

        template<class U1, class U2>
        requires (sizeof...(T) == 2)
            && requires { { bool_constant<is_assignable_v<typename __internal::pick_ith_type<0, T...>::type&, U1>>{} } -> same_as<true_type>; }
            && requires { { bool_constant<is_assignable_v<typename __internal::pick_ith_type<1, T...>::type&, U2>>{} } -> same_as<true_type>; }
        constexpr tuple& operator=(pair<U1, U2>&& p) {
            __leaf_t<0>::get_val() = forward<U1>(p.first);
            __leaf_t<1>::get_val() = forward<U2>(p.second);
            return *this;
        }

        constexpr void swap(tuple& rhs)
        noexcept((is_nothrow_swappable_v<T> && ...))
        requires (is_swappable_v<T> && ...) {
            __leaf_wrapper_t::swap(rhs);
        }
    };

    template<class ...U> tuple(U...) -> tuple<U...>;
    template<class T1, class T2> tuple(pair<T1, T2>) -> tuple<T1, T2>;

    /* 20.5.4 Tuple creation functions */
    inline constexpr struct {
        template<class T>
        auto operator=(T&&) const -> decltype(*this) {
            return *this;
        }
    } ignore;

    template<class ...T>
    constexpr tuple<unwrap_ref_decay_t<T>...> make_tuple(T&& ...t) {
        return tuple<unwrap_ref_decay_t<T>...>(forward<T>(t)...);
    }

    template<class ...T>
    constexpr tuple<T&&...> forward_as_tuple(T&& ...t) noexcept {
        return tuple<T&&...>(forward<T>(t)...);
    }

    template<class ...T>
    constexpr tuple<T&...> tie(T& ...t) noexcept {
        return tuple<T&...>(t...);
    }

    namespace __internal {
        /* Given a list of index sequences, flatten them into one index sequence. */
        template<class ...Sequences>
        struct __merge_index_sequences;
        // Here, Result is the flattened part of the index sequence, Next is the numbers inside the next index_sequence, and Sequences is the remaining
        // index sequences.
        template<std::size_t ...Result, std::size_t ...Next, class ...Sequences>
        struct __merge_index_sequences<index_sequence<Result...>, index_sequence<Next...>, Sequences...> {
            using type = typename __merge_index_sequences<index_sequence<Result..., Next...>, Sequences...>::type;
        };
        template<std::size_t ...Result, std::size_t ...Next>
        struct __merge_index_sequences<index_sequence<Result...>, index_sequence<Next...>> {
            using type = index_sequence<Result..., Next...>;
        };

        /* Generate an index sequence consisted of N values equal to C. */
        template<std::size_t C, std::size_t N, class Sequence = index_sequence<>>
        struct __fill_index_sequence;
        template<std::size_t C, std::size_t ...Seq>
        struct __fill_index_sequence<C, 0, index_sequence<Seq...>> {
            using type = index_sequence<Seq...>;
        };
        template<std::size_t C, std::size_t N, std::size_t ...Seq>
        struct __fill_index_sequence<C, N, index_sequence<Seq...>> {
            using type = typename __fill_index_sequence<C, N - 1, index_sequence<Seq..., C>>::type;
        };

        template<std::size_t I, class Tuple, class ...Tuples>
        constexpr auto __pick_ith_tuple(Tuple&& tpl, Tuples&& ...tpls) noexcept -> decltype(auto) {
            if constexpr (I == 0) {
                return forward<Tuple>(tpl);
            } else {
                return __pick_ith_tuple<I - 1>(forward<Tuples>(tpls)...);
            }
        }

        template<std::size_t I, class ...Types>
        requires (I < sizeof...(Types))
        constexpr typename pick_ith_type<I, Types...>::type& __tuple_get(tuple<Types...>& t) noexcept {
            return t.__tuple_leaf<I, typename pick_ith_type<I, Types...>::type>::get_val();
        }

        template<std::size_t I, class ...Types>
        requires (I < sizeof...(Types))
        constexpr typename pick_ith_type<I, Types...>::type&& __tuple_get(tuple<Types...>&& t) noexcept {
            return move(t).__tuple_leaf<I, typename pick_ith_type<I, Types...>::type>::get_val();
        }

        template<std::size_t I, class ...Types>
        requires (I < sizeof...(Types))
        constexpr const typename pick_ith_type<I, Types...>::type& __tuple_get(const tuple<Types...>& t) noexcept {
            return t.__tuple_leaf<I, typename pick_ith_type<I, Types...>::type>::get_val();
        }

        template<std::size_t I, class ...Types>
        requires (I < sizeof...(Types))
        constexpr const typename pick_ith_type<I, Types...>::type&& __tuple_get(const tuple<Types...>&& t) noexcept {
            return move(t).__tuple_leaf<I, typename pick_ith_type<I, Types...>::type>::get_val();
        }

        /* Checks if a tuple (without reference) satisfies the requirement of the tuple_cat function. */
        template<class T, class U = remove_cvref_t<T>, bool IsLValue = is_lvalue_reference_v<T>>
        struct __tuple_cat_satisfiable;
        template<class T, bool IsLValue, class ...Types>
        struct __tuple_cat_satisfiable<T, tuple<Types...>, IsLValue>
            : bool_constant<IsLValue
                ? (is_constructible_v<Types, typename __add_cv_of<T, Types>::type&> && ...)
                : (is_constructible_v<Types, typename __add_cv_of<T, Types>::type&&> && ...)> {};
    }

    template<class ...Tuples>
    requires (__internal::__tuple_cat_satisfiable<Tuples>::value && ...)
    constexpr auto tuple_cat(Tuples&& ...tpls) {
        // This lambda enables us to get a variadic template parameter I. I has the same length as Tuples and corresponds to the length of each tuple
        // in Tuples.
        constexpr auto match_with_tuple_size = []<std::size_t ...I>(index_sequence<I...>, Tuples&& ...tpls) {
            // An index sequence constructed by repeating the index of each tuple in Tuples for (size of that tuple) times.
            using Is = typename __internal::__merge_index_sequences<typename __internal::__fill_index_sequence<I, tuple_size_v<Tuples>>::type...>::type;
            // An index sequence constructed by concatenating an iota sequence with the size of the length of each tuple together.
            using Js = typename __internal::__merge_index_sequences<make_index_sequence<tuple_size_v<Tuples>>...>::type;

            constexpr auto match_with_inner_outer_index = []<std::size_t ...Is, std::size_t ...Js>(index_sequence<Is...>, index_sequence<Js...>, Tuples&& ...tpls) {
                // Here the expansion I, J will get the Jth element of the Ith tuple.
                return make_tuple(
                    __internal::__tuple_get<Js>(
                        __internal::__pick_ith_tuple<Is>(forward<Tuples>(tpls)...)
                    )...
                );
            };

            return match_with_inner_outer_index(Is{}, Js{}, forward<Tuples>(tpls)...);
        };

        return match_with_tuple_size(make_index_sequence<sizeof...(Tuples)>{}, forward<Tuples>(tpls)...);
    }

    /* 20.5.5 Calling a function with a tuple of arguments */
    template<class F, class Tuple>
    constexpr decltype(auto) apply(F&& f, Tuple&& t) {
        constexpr auto apply_impl = []<std::size_t ...I>(F&& f, Tuple&& t, index_sequence<I...>) {
            return __internal::__INVOKE(static_cast<int*>(nullptr), forward<F>(f), __tuple_get<I>(forward<Tuple>(t))...);
        };

        return apply_impl(forward<F>(f), forward<Tuple>(t), make_index_sequence<tuple_size_v<remove_reference_t<Tuple>>>{});
    }

    template<class T, class Tuple>
    constexpr T make_from_tuple(Tuple &&t) {
        constexpr auto make_from_tuple_impl = []<std::size_t ...I>(Tuple&& t, index_sequence<I...>) {
            return T(__tuple_get<I>(forward<Tuple>(t))...);
        };

        return make_from_tuple_impl(forward<Tuple>(t), make_index_sequence<tuple_size_v<remove_reference_t<Tuple>>>{});
    }

    /* 20.5.6 Tuple helper classes */
    template<std::size_t I, class ...Types>
    requires (I < sizeof...(Types))
    struct tuple_element<I, tuple<Types...>> {
        using type = typename __internal::pick_ith_type<I, Types...>::type;
    };

    /* 20.5.7 Element access */
    template<std::size_t I, class ...Types>
    requires (I < sizeof...(Types))
    constexpr tuple_element_t<I, tuple<Types...>>& get(tuple<Types...>& t) noexcept {
        return __internal::__tuple_get<I, Types...>(t);
    }

    template<std::size_t I, class ...Types>
    requires (I < sizeof...(Types))
    constexpr tuple_element_t<I, tuple<Types...>>&& get(tuple<Types...>&& t) noexcept {
        return __internal::__tuple_get<I, Types...>(move(t));
    }

    template<std::size_t I, class ...Types>
    requires (I < sizeof...(Types))
    constexpr const tuple_element_t<I, tuple<Types...>>& get(const tuple<Types...>& t) noexcept {
        return __internal::__tuple_get<I, Types...>(t);
    }

    template<std::size_t I, class ...Types>
    requires (I < sizeof...(Types))
    constexpr const tuple_element_t<I, tuple<Types...>>&& get(const tuple<Types...>&& t) noexcept {
        return __internal::__tuple_get<I, Types...>(move(t));
    }

    template<class T, class ...Types>
    requires __internal::appears_exactly_i_times<1, T, Types...>::value
    constexpr T& get(tuple<Types...>& t) noexcept {
        return t.__internal::template __tuple_leaf<__internal::get_index_of<0, T, Types...>::value, T>::get_val();
    }

    template<class T, class ...Types>
    requires __internal::appears_exactly_i_times<1, T, Types...>::value
    constexpr T&& get(tuple<Types...>&& t) noexcept {
        return move(t).__internal::template __tuple_leaf<__internal::get_index_of<0, T, Types...>::value, T>::get_val();
    }

    template<class T, class ...Types>
    requires __internal::appears_exactly_i_times<1, T, Types...>::value
    constexpr const T& get(const tuple<Types...>& t) noexcept {
        return t.__internal::template __tuple_leaf<__internal::get_index_of<0, T, Types...>::value, T>::get_val();
    }

    template<class T, class ...Types>
    requires __internal::appears_exactly_i_times<1, T, Types...>::value
    constexpr const T&& get(const tuple<Types...>&& t) noexcept {
        return move(t).__internal::template __tuple_leaf<__internal::get_index_of<0, T, Types...>::value, T>::get_val();
    }

    /* 20.5.8 Relational operators */
    template<class ...T, class ...U>
    requires (sizeof...(T) == sizeof...(U))
    constexpr bool operator==(const tuple<T...>& t, const tuple<U...>& u) {
        constexpr auto helper = []<std::size_t ...I>(const tuple<T...>& t, const tuple<U...>& u) -> bool {
            static_assert(requires {{ ((get<I>(t) == get<I>(u)) && ...) } -> convertible_to<bool>; });
            return ((get<I>(t) == get<I>(u)) && ...);
        };

        return helper(t, u);
    }

    template<class ...T, class ...U>
    constexpr common_comparison_category_t<__internal::synth_three_way_result<T, U>...>
    operator<=>(const tuple<T...>& t, const tuple<U...>& u) {
        if constexpr (sizeof...(T) != sizeof...(U)) {
            return sizeof...(T) <=> sizeof...(U);
        }

        // This returns the three-way comparing result recursively assuming both tuples have the same length.
        constexpr auto helper = []<std::size_t I>(const tuple<T...>& t, const tuple<U...>& u, const auto& next) {
            if constexpr (I == sizeof...(T)) {
                return strong_ordering::equal;
            } else if (auto c = __internal::synth_three_way(get<I>(t), get<I>(u)); c != 0) {
                return c;
            } else {
                return next<I + 1>(t, u, next);
            }
        };

        return helper<0>(t, u, helper);
    }

    /* 20.5.9 Tuple traits */
    // Forward declaration. Originally declared in "memory.hpp".
    template<class T, class Alloc>
    struct uses_allocator;

    template<class ...Types, class Alloc>
    struct uses_allocator<tuple<Types...>, Alloc> : true_type {};

    /* 20.5.10 Tuple specialized algorithms */
    template<class ...T>
    requires (is_swappable_v<T> && ...)
    constexpr void swap(tuple<T...>& x, tuple<T...>& y) noexcept(noexcept(x.swap(y))) {
        x.swap(y);
    }
}