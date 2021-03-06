#pragma once

#include "iosfwd.hpp"
#include "type_traits.hpp"
#include "cstddef.hpp"
#include "concepts.hpp"
#include "utility.hpp"
#include "functional.hpp"
#include "compare.hpp"
#include "initializer_list.hpp"
#include "new.hpp"

namespace std {
    namespace __internal {
        template<class T>
        using with_reference = T&;

        template<class T>
        concept can_reference = requires { typename with_reference<T>; };

        template<class T>
        concept dereferenceable = requires (T& t) { { *t } -> can_reference; };
    }

    // Forward declaration, implemented below.
    template<class>
    struct iterator_traits;

    /* 23.3.2.1 Incrementable traits */
    template<class>
    struct incrementable_traits {};

    template<class T>
    requires is_object_v<T>
    struct incrementable_traits<T*> {
        using difference_type = std::ptrdiff_t;
    };

    template<class I>
    struct incrementable_traits<const I> : incrementable_traits<I> {};

    template<class T>
    requires requires { typename T::difference_type; }
    struct incrementable_traits<T> {
        using difference_type = typename T::difference_type;
    };

    template<class T>
    requires (!requires { typename T::difference_type; } && requires (const T& a, const T& b) { { a - b} -> integral; })
    struct incrementable_traits<T> {
        using difference_type = make_signed_t<decltype(declval<T>() - declval<T>())>;
    };

    template<class T>
    using iter_difference_t = conditional_t<
        requires { typename iterator_traits<remove_cvref_t<T>>::__primary_template; },
        typename iterator_traits<remove_cvref_t<T>>::difference_type,
        typename incrementable_traits<remove_cvref_t<T>>::difference_type
    >;

    /* 23.3.2.2 Indirectly readable traits */
    template<class>
    struct indirectly_readable_traits {};

    template<class T>
    requires is_object_v<T>
    struct indirectly_readable_traits<T*> {
        using value_type = remove_cv_t<T>;
    };

    template<class I>
    requires is_array_v<I>
    struct indirectly_readable_traits<I> {
        using value_type = remove_cv_t<remove_extent_t<I>>;
    };

    template<class I>
    struct indirectly_readable_traits<const I> : indirectly_readable_traits<I> {};

    template<class T>
    requires requires { typename T::value_type; } && is_object_v<typename T::value_type>
    struct indirectly_readable_traits<T> {
        using value_type = remove_cv_t<typename T::value_type>;
    };

    template<class T>
    requires requires { typename T::element_type; } && is_object_v<typename T::element_type>
    struct indirectly_readable_traits<T> {
        using value_type = remove_cv_t<typename T::element_type>;
    };

    template<class T>
    using iter_value_t = conditional_t<
        requires { typename iterator_traits<remove_cvref_t<T>>::__primary_template; },
        typename iterator_traits<remove_cvref_t<T>>::value_type,
        typename indirectly_readable_traits<remove_cvref_t<T>>::value_type
    >;

    /* 23.3.2.3 Iterator traits */
    template<__internal::dereferenceable T>
    using iter_reference_t = decltype(*declval<T&>());

    namespace __internal {
        /* Models a LegacyIterator for definition of iterator_traits. This is slightly different than the named requirement with the same name. */
        template<class I>
        concept __iterator_traits_legacy_iterator = copyable<I> && requires (I i) {
            { *i } -> can_reference;
            { ++i } -> same_as<I&>;
            { *i++ } -> can_reference;
        };

        /* Models a LegacyInputIterator for definition of iterator_traits. This is slightly different than the named requirement with the same name. */
        template<class I>
        concept __iterator_traits_legacy_input_iterator = __iterator_traits_legacy_iterator<I> && equality_comparable<I> && requires (I i) {
            typename incrementable_traits<I>::difference_type;
            typename indirectly_readable_traits<I>::value_type;
            typename common_reference_t<iter_reference_t<I>&&, typename indirectly_readable_traits<I>::value_type&>;
            typename common_reference_t<decltype(*i++)&&, typename indirectly_readable_traits<I>::value_type&>;
            requires signed_integral<typename incrementable_traits<I>::difference_type>;
        };

        /* Models a LegacyForwardIterator for definition of iterator_traits. This is slightly different than the named requirement with the same name. */
        template<class I>
        concept __iterator_traits_legacy_forward_iterator = __iterator_traits_legacy_input_iterator<I> && constructible_from<I>
            && is_lvalue_reference_v<iter_reference_t<I>>
            && same_as<remove_cvref_t<iter_reference_t<I>>, typename indirectly_readable_traits<I>::value_type>
            && requires (I i) {
                { i++ } -> convertible_to<const I&>;
                { *i++ } -> same_as<iter_reference_t<I>>;
            };

        /* Models a LegacyBidirectionalIterator for definition of iterator_traits. This is slightly different than the named requirement with the same name. */
        template<class I>
        concept __iterator_traits_legacy_bidirectional_iterator = __iterator_traits_legacy_forward_iterator<I> && requires (I i) {
            { --i } -> same_as<I&>;
            { i-- } -> convertible_to<const I&>;
            { *i-- } -> same_as<iter_reference_t<I>>;
        };

        /* Models a LegacyRandomAccessIterator for definition of iterator_traits. This is slightly different than the named requirement with the same name. */
        template<class I>
        concept __iterator_traits_legacy_random_access_iterator = __iterator_traits_legacy_bidirectional_iterator<I> && totally_ordered<I>
            && requires (I i, typename incrementable_traits<I>::difference_type n) {
                { i += n } -> same_as<I&>;
                { i -= n } -> same_as<I&>;
                { i + n } -> same_as<I>;
                { i - n } -> same_as<I>;
                { n + i } -> same_as<I&>;
                { i - i } -> same_as<decltype(n)>;
                { i[n] } -> convertible_to<iter_reference_t<I>>;
            };
    }

    // Forward declaration. Declared below.
    struct input_iterator_tag;
    struct output_iterator_tag;
    struct forward_iterator_tag;
    struct bidirectional_iterator_tag;
    struct random_access_iterator_tag;
    struct contiguous_iterator_tag;

    template<class I>
    struct iterator_traits {};

    template<class I>
    requires requires {
        typename I::difference_type;
        typename I::value_type;
        typename I::pointer;
        typename I::reference;
        typename I::iterator_category;
    }
    struct iterator_traits<I> {
        using difference_type = typename I::difference_type;
        using value_type = typename I::difference_type;
        using pointer = typename I::pointer;
        using reference = typename I::reference;
        using iterator_category = typename I::iterator_category;
        using __primary_template = void;
    };

    template<class I>
    requires requires {
        typename I::difference_type;
        typename I::value_type;
        typename I::reference;
        typename I::iterator_category;
    }
    struct iterator_traits<I> {
        using difference_type = typename I::difference_type;
        using value_type = typename I::difference_type;
        using pointer = void;
        using reference = typename I::reference;
        using iterator_category = typename I::iterator_category;
        using __primary_template = void;
    };

    template<class I>
    requires (!requires {
        typename I::difference_type;
        typename I::value_type;
        typename I::reference;
        typename I::iterator_category;
    }) && __internal::__iterator_traits_legacy_input_iterator<I>
    struct iterator_traits<I> {
    private:
        static constexpr auto get_pointer_type() noexcept {
            if constexpr (requires { typename I::pointer; }) {
                return declval<I::pointer>();
            } else if constexpr (requires (I& i) { i.operator->(); }) {
                return declval<decltype(declval<I&>().operator->())>();
            }
        }

        static constexpr auto get_reference_type() noexcept {
            if constexpr (requires { typename I::reference; }) {
                return declval<I::reference>();
            } else {
                return declval<iter_reference_t<I>>();
            }
        }

        static constexpr auto get_iterator_category() noexcept {
            if constexpr (requires { typename I::iterator_category; }) {
                return declval<I::iterator_category>();
            } else if constexpr (__internal::__iterator_traits_legacy_random_access_iterator<I>) {
                return declval<random_access_iterator_tag>();
            } else if constexpr (__internal::__iterator_traits_legacy_bidirectional_iterator<I>) {
                return declval<bidirectional_iterator_tag>();
            } else if constexpr (__internal::__iterator_traits_legacy_forward_iterator<I>) {
                return declval<forward_iterator_tag>();
            } else {
                return declval<input_iterator_tag>();
            }
        }
    public:
        using difference_type = typename incrementable_traits<I>::difference_type;
        using value_type = typename indirectly_readable_traits<I>::value_type;
        using pointer = decltype(get_pointer_type());
        using reference = decltype(get_reference_type());
        using iterator_category = decltype(get_iterator_category());
        using __primary_template = void;
    };

    template<class I>
    requires (!requires {
        typename I::difference_type;
        typename I::value_type;
        typename I::reference;
        typename I::iterator_category;
    }) && (!__internal::__iterator_traits_legacy_input_iterator<I>) && __internal::__iterator_traits_legacy_iterator<I>
    struct iterator_traits<I> {
    private:
        static constexpr auto get_difference_type() noexcept {
            if constexpr (requires { typename I::difference_type; }) {
                return declval<I::difference_type>();
            }
        }
    public:
        using difference_type = decltype(get_difference_type());
        using value_type = void;
        using pointer = void;
        using reference = void;
        using iterator_category = output_iterator_tag;
        using __primary_template = void;
    };

    template<class T>
    requires is_object_v<T>
    struct iterator_traits<T*> {
        using iterator_concept = contiguous_iterator_tag;
        using iterator_category = random_access_iterator_tag;
        using value_type = remove_cv_t<T>;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
    };

    namespace ranges {
        /* 23.3.3.1 ranges::iter_move */
        namespace __iter_move_internal {
            void iter_move();

            struct iter_move_fn {
            private:
                template<class E>
                static constexpr bool use_adl_iter_move = (is_class_v<E> || is_enum_v<E>) && requires (E&& e) { iter_move(forward<E>(e)); };
            public:
                template<class E>
                requires use_adl_iter_move<E>
                constexpr decltype(auto) operator()(E&& e) const noexcept(noexcept(iter_move(forward<E>(e)))) {
                    return iter_move(forward<E>(e));
                }

                template<class E>
                requires (!use_adl_iter_move<E>) && requires (E&& e) { *forward<E>(e); }
                constexpr decltype(auto) operator()(E&& e) const noexcept(noexcept(*forward<E>(e))) {
                    if constexpr (is_lvalue_reference_v<decltype(*forward<E>(e))>) {
                        return move(*forward<E>(e));
                    } else {
                        return *forward<E>(e);
                    }
                }
            };
        }

        inline namespace __fn_objects {
            inline constexpr ::std::ranges::__iter_move_internal::iter_move_fn iter_move;
        }

        /* 23.3.3.2 ranges::iter_swap */
        namespace __iter_swap_internal {
            template<class X, class Y>
            constexpr iter_value_t<X> iter_exchange_move(X&& x, Y&& y)
            noexcept(noexcept(iter_value_t<X>(iter_move(x))) && noexcept(*x = iter_move(y))) {
                iter_value_t<X> old_value(iter_move(x));
                *x = iter_move(y);
                return old_value;
            }

            template<class I1, class I2>
            void iter_swap(I1, I2) = delete;

            struct iter_swap_fn {
            private:
                template<class X, class Y>
                static constexpr bool use_adl_iter_swap() noexcept {
                    return (is_class_v<X> || is_enum_v<X>) && (is_class_v<Y> || is_enum_v<Y>)
                        && requires (X&& x, Y&& y) { iter_swap(forward<X>(x), forward<Y>(y)); };
                }

                // Forward declaration. Implemented below.
                template<class X, class Y>
                static constexpr bool use_ranges_swap() noexcept;

                // Forward declaration. Implemented below.
                template<class X, class Y>
                static constexpr bool use_iter_exchange_move() noexcept;
            public:
                template<class X, class Y>
                requires (use_adl_iter_swap<X, Y>())
                constexpr void operator()(X&& x, Y&& y) const noexcept(noexcept(iter_swap(forward<X>(x), forward<Y>(y)))) {
                    iter_swap(forward<X>(x), forward<Y>(y));
                }

                template<class X, class Y>
                requires (use_ranges_swap<X, Y>())
                constexpr void operator()(X&& x, Y&& y) const noexcept(noexcept(ranges::swap(*forward<X>(x), *forward<Y>(y)))) {
                    ranges::swap(forward<X>(x), forward<Y>(y));
                }

                template<class X, class Y>
                requires (use_iter_exchange_move<X, Y>())
                constexpr void operator()(X&& x, Y&& y) const noexcept(noexcept(*forward<X>(x) = iter_exchange_move(forward<Y>(y), forward<X>(x)))) {
                    *forward<X>(x) = iter_exchange_move(forward<Y>(y), forward<X>(x));
                }
            };
        }

        inline namespace __fn_objects {
            inline constexpr ::std::ranges::__iter_swap_internal::iter_swap_fn iter_swap;
        }
    }

    template<__internal::dereferenceable T>
    requires requires (T& t) {
        { ranges::iter_move(t) } -> __internal::can_reference;
    }
    using iter_rvalue_reference_t = decltype(ranges::iter_move(declval<T&>()));

    /* 23.3.4 Iterator concepts */
    namespace __internal {
        template<class I>
        using __iter_traits_t = conditional_t<!requires { typename iterator_traits<I>::__primary_template; }, I, iterator_traits<I>>;

        template<class I>
        struct __iter_concept {
        private:
            static constexpr auto get_type() noexcept {
                if constexpr (requires { typename __iter_traits_t<I>::iterator_concept; }) {
                    return declval<typename __iter_traits_t<I>::iterator_concept>();
                } else if constexpr (requires { typename __iter_traits_t<I>::iterator_category; }) {
                    return declval<typename __iter_traits_t<I>::iterator_category>();
                } else if constexpr (!requires { typename iterator_traits<I>::__primary_template; }) {
                    return declval<random_access_iterator_tag>();
                }
            }

        public:
            using type = decltype(get_type());
        };

        template<class I>
        using __iter_concept_t = typename __iter_concept<I>::type;
    }

    template<class In>
    concept indirectly_readable = requires (const remove_cvref_t<In> in) {
        typename iter_value_t<remove_cvref_t<In>>;
        typename iter_reference_t<remove_cvref_t<In>>;
        typename iter_rvalue_reference_t<remove_cvref_t<In>>;
        { *in } -> same_as<iter_reference_t<remove_cvref_t<In>>>;
        { ranges::iter_move(in) } -> same_as<iter_rvalue_reference_t<remove_cvref_t<In>>>;
    } && common_reference_with<iter_reference_t<remove_cvref_t<In>>&&, iter_value_t<remove_cvref_t<In>>&>
        && common_reference_with<iter_reference_t<remove_cvref_t<In>>&&, iter_rvalue_reference_t<remove_cvref_t<In>>&&>
        && common_reference_with<iter_rvalue_reference_t<remove_cvref_t<In>>&&, const iter_value_t<remove_cvref_t<In>>&>;

    template<indirectly_readable T>
    using iter_common_reference_t = common_reference_t<iter_reference_t<T>, iter_value_t<T>&>;

    template<class Out, class T>
    concept indirectly_writable = requires (Out&& o, T&& t) {
        *o = forward<T>(t);
        *forward<Out>(o) = forward<T>(t);
        const_cast<const iter_reference_t<Out>&&>(*o) = forward<T>(t);
        const_cast<const iter_reference_t<Out>&&>(*forward<Out>(o)) = forward<T>(t);
    };

    template<class I>
    concept weakly_incrementable = default_initializable<I> && movable<I>
        && requires (I i) {
            typename iter_difference_t<I>;
            requires signed_integral<iter_difference_t<I>>;
            { ++i } -> same_as<I&>;
            i++;
        };

    template<class I>
    concept incrementable = regular<I> && weakly_incrementable<I>
        && requires (I i) { { i++ } -> same_as<I>; };

    template<class I>
    concept input_or_output_iterator = requires (I i) {
        { *i } -> __internal::can_reference;
    } && weakly_incrementable<I>;

    template<class S, class I>
    concept sentinel_for = semiregular<S> && input_or_output_iterator<I>
        && __internal::weakly_equality_comparable_with<S, I>;

    template<class S, class I>
    inline constexpr bool disable_sized_sentinel_for = false;

    template<class S, class I>
    concept sized_sentinel_for = sentinel_for<S, I> && !disable_sized_sentinel_for<remove_cv_t<S>, remove_cv_t<I>>
        && requires (const I& i, const S& s) {
            { s - i } -> same_as<iter_difference_t<I>>;
            { i - s } -> same_as<iter_difference_t<I>>;
        };

    template<class I>
    concept input_iterator = input_or_output_iterator<I> && indirectly_readable<I>
        && requires { typename __internal::__iter_concept<I>; }
        && derived_from<__internal::__iter_concept_t<I>, input_iterator_tag>;

    template<class I, class T>
    concept output_iterator = input_or_output_iterator<I> && indirectly_writable<I, T>
        && requires (I i, T&& t) { *i++ = forward<T>(t); };

    template<class I>
    concept forward_iterator = input_iterator<I> && derived_from<__internal::__iter_concept_t<I>, forward_iterator_tag>
        && incrementable<I> && sentinel_for<I, I>;

    template<class I>
    concept bidirectional_iterator = forward_iterator<I> && derived_from<__internal::__iter_concept_t<I>, bidirectional_iterator_tag>
        && requires (I i) {
            { --i } -> same_as<I&>;
            { i-- } -> same_as<I>;
        };

    template<class I>
    concept random_access_iterator = bidirectional_iterator<I> && derived_from<__internal::__iter_concept_t<I>, random_access_iterator_tag>
        && totally_ordered<I> && sized_sentinel_for<I, I>
        && requires (I i, const I j, const iter_difference_t<I> n) {
            { i += n } -> same_as<I&>;
            { j + n } -> same_as<I>;
            { n + j } -> same_as<I>;
            { i -= n } -> same_as<I&>;
            { j - n } -> same_as<I>;
            { j[n] } -> same_as<iter_reference_t<I>>;
        };

    template<class I>
    concept contiguous_iterator = random_access_iterator<I> && derived_from<__internal::__iter_concept_t<I>, contiguous_iterator_tag>
        && is_lvalue_reference_v<iter_reference_t<I>> && same_as<iter_value_t<I>, remove_cvref_t<iter_reference_t<I>>>
        && requires (const I& i) {
            { to_address(i) } -> same_as<add_pointer_t<iter_reference_t<I>>>;
        };

    /* 23.3.6 Indirect callable requirements */
    template<class F, class I>
    concept indirectly_unary_invocable = indirectly_readable<I> && copy_constructible<F> && invocable<F&, iter_value_t<I>&>
        && invocable<F&, iter_reference_t<I>> && invocable<F&, iter_common_reference_t<I>>
        && common_reference_with<invoke_result_t<F&, iter_value_t<I>&>, invoke_result_t<F&, iter_reference_t<I>>>;

    template<class F, class I>
    concept indirectly_regular_unary_invocable = indirectly_readable<I> && copy_constructible<F> && regular_invocable<F&, iter_value_t<I>&>
        && regular_invocable<F&, iter_reference_t<I>> && regular_invocable<F&, iter_common_reference_t<I>>
        && common_reference_with<invoke_result_t<F&, iter_value_t<I>&>, invoke_result_t<F&, iter_reference_t<I>>>;

    template<class F, class I>
    concept indirect_unary_predicate = indirectly_readable<I> && copy_constructible<F> && predicate<F&, iter_value_t<I>&>
        && predicate<F&, iter_reference_t<I>> && predicate<F&, iter_common_reference_t<I>>;

    template<class F, class I1, class I2 = I1>
    concept indirect_binary_predicate = indirectly_readable<I1> && indirectly_readable<I2> && copy_constructible<F>
        && predicate<F&, iter_value_t<I1>&, iter_value_t<I2>&>
        && predicate<F&, iter_value_t<I1>&, iter_reference_t<I2>>
        && predicate<F&, iter_reference_t<I1>, iter_value_t<I2>&>
        && predicate<F&, iter_reference_t<I1>, iter_reference_t<I2>>
        && predicate<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;

    template<class F, class I1, class I2 = I1>
    concept indirect_equivalence_relation = indirectly_readable<I1> && indirectly_readable<I2> && copy_constructible<F>
        && equivalence_relation<F&, iter_value_t<I1>&, iter_value_t<I2>&>
        && equivalence_relation<F&, iter_value_t<I1>&, iter_reference_t<I2>>
        && equivalence_relation<F&, iter_reference_t<I1>, iter_value_t<I2>&>
        && equivalence_relation<F&, iter_reference_t<I1>, iter_reference_t<I2>>
        && equivalence_relation<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;

    template<class F, class I1, class I2 = I1>
    concept indirect_strict_weak_order = indirectly_readable<I1> && indirectly_readable<I2> && copy_constructible<F>
        && strict_weak_order<F&, iter_value_t<I1>&, iter_value_t<I2>&>
        && strict_weak_order<F&, iter_value_t<I1>&, iter_reference_t<I2>>
        && strict_weak_order<F&, iter_reference_t<I1>, iter_value_t<I2>&>
        && strict_weak_order<F&, iter_reference_t<I1>, iter_reference_t<I2>>
        && strict_weak_order<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;

    template<class F, class ...Is>
    requires (indirectly_readable<Is> && ...) && invocable<F, iter_reference_t<Is>...>
    using indirect_result_t = invoke_result_t<F, iter_reference_t<Is>...>;

    template<indirectly_readable I, indirectly_regular_unary_invocable<I> Proj>
    struct projected {
        using value_type = remove_cvref_t<indirect_result_t<Proj&, I>>;
        indirect_result_t<Proj&, I> operator*() const;
    };

    template<weakly_incrementable I, class Proj>
    struct incrementable_traits<projected<I, Proj>> {
        using difference_type = iter_difference_t<I>;
    };

    /* 23.3.7 Common algorithm requirements */
    template<class In, class Out>
    concept indirectly_movable = indirectly_readable<In> && indirectly_writable<Out, iter_rvalue_reference_t<In>>;

    template<class In, class Out>
    concept indirectly_movable_storable = indirectly_movable<In, Out> && indirectly_writable<Out, iter_value_t<In>>
        && movable<iter_value_t<In>> && constructible_from<iter_value_t<In>, iter_rvalue_reference_t<In>>
        && assignable_from<iter_value_t<In>&, iter_rvalue_reference_t<In>>;

    template<class In, class Out>
    concept indirectly_copyable = indirectly_readable<In> && indirectly_writable<Out, iter_reference_t<In>>;

    template<class In, class Out>
    concept indirectly_copyable_storable = indirectly_copyable<In, Out> && indirectly_writable<Out, iter_value_t<In>&>
        && indirectly_writable<Out, const iter_value_t<In>&> && indirectly_writable<Out, iter_value_t<In>&&>
        && indirectly_writable<Out, const iter_value_t<In>&&> && copyable<iter_value_t<In>>
        && constructible_from<iter_value_t<In>, iter_reference_t<In>> && assignable_from<iter_value_t<In>&, iter_reference_t<In>>;

    template<class I1, class I2 = I1>
    concept indirectly_swappable = indirectly_readable<I1> && indirectly_readable<I2>
        && requires (const I1 i1, const I2 i2) {
            ranges::iter_swap(i1, i1);
            ranges::iter_swap(i1, i2);
            ranges::iter_swap(i2, i1);
            ranges::iter_swap(i2, i2);
        };

    template<class I1, class I2, class R, class P1 = identity, class P2 = identity>
    concept indirectly_comparable = indirect_binary_predicate<R, projected<I1, P1>, projected<I2, P2>>;

    template<class I>
    concept permutable = forward_iterator<I> && indirectly_movable_storable<I, I> && indirectly_swappable<I, I>;

    template<class I1, class I2, class Out, class R = ranges::less, class P1 = identity, class P2 = identity>
    concept mergeable = input_iterator<I1> && input_iterator<I2> && weakly_incrementable<Out>
        && indirectly_copyable<I1, Out> && indirectly_copyable<I2, Out> && indirect_strict_weak_order<R, projected<I1, P1>, projected<I2, P2>>;

    template<class I, class R = ranges::less, class P = identity>
    concept sortable = permutable<I> && indirect_strict_weak_order<R, projected<I, P>>;

    /* 23.4.2 Standard iterator tags */
    struct input_iterator_tag {};
    struct output_iterator_tag {};
    struct forward_iterator_tag : public input_iterator_tag {};
    struct bidirectional_iterator_tag : public forward_iterator_tag {};
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};
    struct contiguous_iterator_tag : public random_access_iterator_tag {};

    /* 23.4.3 Iterator operations */
    namespace __internal {
        template<class I>
        concept legacy_iterator = copy_constructible<I> && is_copy_assignable_v<I> && destructible<I> && swappable<I&>
            && requires (I& r) {
                typename iterator_traits<I>::value_type;
                typename iterator_traits<I>::difference_type;
                typename iterator_traits<I>::reference;
                typename iterator_traits<I>::pointer;
                typename iterator_traits<I>::iterator_category;
                *r;
                { ++r } -> same_as<I&>;
            };

        template<class I>
        concept legacy_input_iterator = legacy_iterator<I> && equality_comparable<I> && requires (I i, I j) {
            { i != j } -> convertible_to<bool>;
            { *i } -> same_as<typename iterator_traits<I>::reference>;
            { *i } -> convertible_to<typename iterator_traits<I>::value_type>;
            { ++i } -> same_as<I&>;
            (void) i++;
            { *i++ } -> convertible_to<typename iterator_traits<I>::value_type>;
        };

        template<class I>
        concept legacy_output_iterator = legacy_iterator<I> && (is_class_v<I> || is_pointer_v<I>) && requires (I& r) {
            { ++r } -> same_as<I&>;
            { r++ } -> convertible_to<const I&>;
        };

        template<class I>
        concept legacy_forward_iterator = legacy_input_iterator<I> && is_default_constructible_v<I>
            && ((is_same_v<typename iterator_traits<I>::reference, typename iterator_traits<I>::value_type&> && legacy_output_iterator<I>)
                || (is_same_v<typename iterator_traits<I>::reference, const typename iterator_traits<I>::value_type&> && !legacy_output_iterator<I>))
            && requires (I i) {
                { i++ } -> same_as<I>;
                { *i++ } -> same_as<typename iterator_traits<I>::reference>;
            };

        template<class I>
        concept legacy_bidirectional_iterator = legacy_forward_iterator<I> && requires (I a, I b) {
            { --a } -> same_as<I&>;
            { a-- } -> convertible_to<const I&>;
            { *a-- } -> same_as<typename iterator_traits<I>::reference>;
        };

        template<class I>
        concept legacy_random_access_iterator = legacy_bidirectional_iterator<I>
            && requires (I i, I a, I b, I& r, typename iterator_traits<I>::difference_type n) {
                { r += n } -> same_as<I&>;
                { a + n } -> same_as<I>;
                { n + a } -> same_as<I>;
                { r -= n } -> same_as<I&>;
                { i - n } -> same_as<I>;
                { b - a } -> same_as<typename iterator_traits<I>::difference_type>;
                { i[n] } -> convertible_to<typename iterator_traits<I>::reference>;
                { a < b } -> convertible_to<bool>;
                { a > b } -> convertible_to<bool>;
                { a <= b } -> convertible_to<bool>;
                { a >= b } -> convertible_to<bool>;
            };
    }

    template<__internal::legacy_input_iterator InputIterator, class Distance>
    constexpr void advance(InputIterator& i, Distance n) {
        if constexpr (is_same_v<typename iterator_traits<InputIterator>::iterator_category, random_access_iterator_tag>) {
            i += n;
        } else {
            if (n >= 0) {
                while (n--) {
                    i++;
                }
            } else {
                while (n++) {
                    i--;
                }
            }
        }
    }

    template<__internal::legacy_input_iterator InputIterator>
    constexpr typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, InputIterator last) {
        if constexpr (is_same_v<typename iterator_traits<InputIterator>::iterator_category, random_access_iterator_tag>) {
            return last - first;
        } else {
            typename iterator_traits<InputIterator>::difference_type i = 0;
            while (last != first) {
                first++;
                i++;
            }

            return i;
        }
    }

    template<__internal::legacy_input_iterator InputIterator>
    constexpr InputIterator next(InputIterator x, typename iterator_traits<InputIterator>::difference_type n = 1) {
        advance(x, n);
        return x;
    }

    template<__internal::legacy_input_iterator InputIterator>
    constexpr InputIterator prev(InputIterator x, typename iterator_traits<InputIterator>::difference_type n = 1) {
        advance(x, -n);
        return x;
    }

    namespace ranges {
        /* 24.3 Range access */
        template<class T>
        inline const bool enable_borrowed_range = false;

        namespace __begin_internal {
            void begin(auto&) = delete;
            void begin(const auto&) = delete;

            struct begin_fn {
            private:
                template<class T>
                static consteval bool is_noexcept() noexcept {
                    using unqualified_t = remove_cvref_t<T>;

                    if constexpr (is_array_v<unqualified_t> && __internal::is_complete<remove_all_extents_t<unqualified_t>>::value) {
                        return noexcept(declval<T>() + 0);
                    } else if constexpr (requires (T&& t) { { __internal::decay_copy(forward<T>(t).begin()) } -> input_or_output_iterator; }) {
                        return noexcept(__internal::decay_copy(declval<T>().begin()));
                    } else if constexpr ((is_class_v<unqualified_t> || is_enum_v<unqualified_t>)
                        && requires (T&& t) { { __internal::decay_copy(begin(forward<T>(t))) } -> input_or_output_iterator; }) {
                        return noexcept(__internal::decay_copy(begin(declval<T>())));
                    }

                    return true;
                }

            public:
                template<class T>
                requires is_lvalue_reference_v<T> || enable_borrowed_range<remove_cvref_t<T>>
                constexpr input_or_output_iterator auto operator()(T&& t) const noexcept(is_noexcept(declval<T>())) {
                    using unqualified_t = remove_cvref_t<T>;

                    if constexpr (is_array_v<unqualified_t>) {
                        if constexpr ( __internal::is_complete<remove_all_extents_t<unqualified_t>>::value) {
                            return forward<T>(t) + 0;
                        } else {
                            __internal::always_illformed<T>();
                        }
                    } else if constexpr (requires { { __internal::decay_copy(forward<T>(t).begin()) } -> input_or_output_iterator; }) {
                        return __internal::decay_copy(forward<T>(t).begin());
                    } else if constexpr ((is_class_v<unqualified_t> || is_enum_v<unqualified_t>)
                        && requires { { __internal::decay_copy(begin(forward<T>(t))) } -> input_or_output_iterator; }) {
                        return __internal::decay_copy(begin(forward<T>(t)));
                    } else {
                        __internal::always_illformed<T>();
                    }
                }
            };
        }

        inline namespace __fn_objects {
            inline constexpr __begin_internal::begin_fn begin;
        }

        namespace __end_internal {
            void end(auto&) = delete;
            void end(const auto&) = delete;

            struct end_fn {
            private:
                template<class T>
                using iterator_t = decltype(ranges::begin(declval<T&>()));

                template<class T>
                static consteval bool is_noexcept() noexcept {
                    using unqualified_t = remove_cvref_t<T>;
                    if constexpr (is_bounded_array_v<unqualified_t> && __internal::is_complete<remove_all_extents_t<unqualified_t>>::value) {
                        return noexcept(declval<T>() + extent_v<T>);
                    } else if constexpr (requires (T&& t) { { __internal::decay_copy(forward<T>(t).end()) } -> sentinel_for<iterator_t<T>>; }) {
                        return noexcept(__internal::decay_copy(declval<T>().end()));
                    } else if constexpr ((is_class_v<unqualified_t> || is_enum_v<unqualified_t>)
                        && requires (T&& t) { { __internal::decay_copy(end(forward<T>(t))) } -> sentinel_for<iterator_t<T>>; }) {
                        return noexcept(__internal::decay_copy(end(declval<T>())));
                    }

                    return true;
                }

            public:
                template<class T>
                requires is_lvalue_reference_v<T> || enable_borrowed_range<remove_cvref_t<T>>
                constexpr sentinel_for<iterator_t<T>> auto operator()(T&& t) const noexcept(is_noexcept<T>()) {
                    using unqualified_t = remove_cvref_t<T>;
                    if constexpr (is_bounded_array_v<unqualified_t>) {
                        if constexpr (__internal::is_complete<remove_all_extents_t<unqualified_t>>::value) {
                            return forward<T>(t) + extent_v<T>;
                        } else {
                            __internal::always_illformed<T>();
                        }
                    } else if constexpr (requires { { __internal::decay_copy(forward<T>(t).end()) } -> sentinel_for<iterator_t<T>>; }) {
                        return __internal::decay_copy(forward<T>(t).end());
                    } else if constexpr ((is_class_v<unqualified_t> || is_enum_v<unqualified_t>)
                        && requires { { __internal::decay_copy(end(forward<T>(t))) } -> sentinel_for<iterator_t<T>>; }) {
                        return __internal::decay_copy(end(forward<T>(t)));
                    } else {
                        __internal::always_illformed<T>();
                    }
                }
            };
        }

        inline namespace __fn_objects {
            inline constexpr __end_internal::end_fn end;
        }

        struct __cbegin_fn {
        private:
            template<class T>
            using CT = conditional_t<is_lvalue_reference_v<T>, const remove_reference_t<T>&, const T>;

        public:
            template<class T>
            constexpr input_or_output_iterator auto operator()(T&& t) const noexcept(noexcept(ranges::begin(static_cast<CT<T>&&>(t)))) {
                return ranges::begin(static_cast<CT<T>&&>(t));
            }
        };

        inline namespace __fn_objects {
            inline constexpr __cbegin_fn cbegin;
        }

        struct __cend_fn {
        private:
            template<class T>
            using CT = conditional_t<is_lvalue_reference_v<T>, const remove_reference_t<T>&, const T>;

            template<class T>
            using iterator_t = decltype(ranges::begin(declval<T&>()));

        public:
            template<class T>
            constexpr sentinel_for<iterator_t<T>> auto operator()(T&& t) const noexcept(noexcept(ranges::end(static_cast<CT<T>&&>(t)))) {
                return ranges::end(static_cast<CT<T>&&>(t));
            }
        };

        inline namespace __fn_objects {
            inline constexpr __cend_fn cend;
        }

        namespace __rbegin_internal {
            void rbegin(auto&) = delete;
            void rbegin(const auto&) = delete;

            struct rbegin_fn {
            private:
                template<class T>
                static consteval bool is_noexcept() noexcept {
                    using unqualified_t = remove_cvref_t<T>;
                    if constexpr (is_array_v<unqualified_t> && !__internal::is_complete<remove_all_extents_t<unqualified_t>>::value) {
                        return true;
                    } else if constexpr (requires (T&& t) { { __internal::decay_copy(forward<T>(t).rbegin()) } -> input_or_output_iterator; }) {
                        return noexcept(__internal::decay_copy(declval<T>().rbegin()));
                    } else if constexpr ((is_class_v<unqualified_t> || is_enum_v<unqualified_t>) && requires (T&& t) { { __internal::decay_copy(rbegin(forward<T>(t))) } -> input_or_output_iterator; }) {
                        return noexcept(__internal::decay_copy(rbegin(declval<T>())));
                    } else if constexpr (requires (T&& t) {
                        { ranges::begin(forward<T>(t)) } -> bidirectional_iterator;
                        { ranges::end(forward<T>(t)) } -> bidirectional_iterator;
                    } && is_same_v<decltype(ranges::begin(declval<T>())), decltype(ranges::end(declval<T>()))>) {
                        return noexcept(make_reverse_iterator(ranges::end(declval<T>())));
                    }

                    return true;
                }
            public:
                template<class T>
                requires is_lvalue_reference_v<T> || enable_borrowed_range<remove_cvref_t<T>>
                constexpr input_or_output_iterator auto operator()(T&& t) const noexcept(is_noexcept<T>()) {
                    using unqualified_t = remove_cvref_t<T>;
                    if constexpr (is_array_v<unqualified_t> && !__internal::is_complete<remove_all_extents_t<unqualified_t>>::value) {
                        __internal::always_illformed<T>();
                    } else if constexpr (requires { { __internal::decay_copy(forward<T>(t).rbegin()) } -> input_or_output_iterator; }) {
                        return __internal::decay_copy(forward<T>(t).rbegin());
                    } else if constexpr ((is_class_v<unqualified_t> || is_enum_v<unqualified_t>) && requires { { __internal::decay_copy(rbegin(forward<T>(t))) } -> input_or_output_iterator; }) {
                        return __internal::decay_copy(rbegin(forward<T>(t)));
                    } else if constexpr (requires {
                        { ranges::begin(forward<T>(t)) } -> bidirectional_iterator;
                        { ranges::end(forward<T>(t)) } -> bidirectional_iterator;
                    } && is_same_v<decltype(ranges::begin(forward<T>(t))), decltype(ranges::end(forward<T>(t)))>) {
                        return make_reverse_iterator(ranges::end(forward<T>(t)));
                    } else {
                        __internal::always_illformed<T>();
                    }
                }
            };
        }

        inline namespace __fn_objects {
            inline constexpr __rbegin_internal::rbegin_fn rbegin;
        }

        namespace __rend_internal {
            void rend(auto&) = delete;
            void rend(const auto&) = delete;

            struct rend_fn {
            private:
                template<class T>
                static consteval bool is_noexcept() noexcept {
                    using unqualified_t = remove_cvref_t<T>;
                    if constexpr (is_array_v<unqualified_t> && !__internal::is_complete<remove_all_extents_t<unqualified_t>>::value) {
                        return true;
                    } else if constexpr (requires (T&& t) { { __internal::decay_copy(forward<T>(t).rend()) } -> sentinel_for<decltype(ranges::rbegin(declval<T>()))>; }) {
                        return noexcept(__internal::decay_copy(declval<T>().rend()));
                    } else if constexpr ((is_class_v<unqualified_t> || is_enum_v<unqualified_t>) && requires (T&& t) { { __internal::decay_copy(rend(forward<T>(t))) } -> sentinel_for<decltype(ranges::rbegin(declval<T>()))>; }) {
                        return noexcept(__internal::decay_copy(rend(declval<T>())));
                    } else if constexpr (requires (T&& t) {
                        { ranges::begin(forward<T>(t)) } -> bidirectional_iterator;
                        { ranges::end(forward<T>(t)) } -> bidirectional_iterator;
                        { ranges::end(forward<T>(t)) } -> same_as<decltype(ranges::begin(forward<T>(t)))>;
                    }) {
                        return noexcept(make_reverse_iterator(ranges::begin(declval<T>())));
                    }

                    return true;
                }
            public:
                template<class T>
                requires is_lvalue_reference_v<T> || enable_borrowed_range<remove_cvref_t<T>>
                constexpr sentinel_for<decltype(ranges::rbegin(declval<T>()))> auto operator()(T&& t) const noexcept(is_noexcept<T>()) {
                    using unqualified_t = remove_cvref_t<T>;
                    if constexpr (is_array_v<unqualified_t> && !__internal::is_complete<remove_all_extents_t<unqualified_t>>::value) {
                        __internal::always_illformed<T>();
                    } else if constexpr (requires { { __internal::decay_copy(forward<T>(t).rend()) } -> sentinel_for<decltype(ranges::rbegin(forward<T>(t)))>; }) {
                        return __internal::decay_copy(forward<T>(t).rend());
                    } else if constexpr ((is_class_v<unqualified_t> || is_enum_v<unqualified_t>) && requires { { __internal::decay_copy(rend(forward<T>(t))) } -> sentinel_for<decltype(ranges::rbegin(declval<T>()))>; }) {
                        return __internal::decay_copy(rend(forward<T>(t)));
                    } else if constexpr (requires {
                        { ranges::begin(forward<T>(t)) } -> bidirectional_iterator;
                        { ranges::end(forward<T>(t)) } -> bidirectional_iterator;
                        { ranges::end(forward<T>(t)) } -> same_as<decltype(ranges::begin(forward<T>(t)))>;
                    }) {
                        return make_reverse_iterator(ranges::begin(forward<T>(t)));
                    } else {
                        __internal::always_illformed<T>();
                    }
                }
            };
        }

        inline namespace __fn_objects {
            inline constexpr __rend_internal::rend_fn rend;
        }

        struct __crbegin_fn {
        private:
            template<class T>
            using CT = conditional_t<is_lvalue_reference_v<T>, const remove_reference_t<T>&, const T>;

        public:
            template<class T>
            constexpr input_or_output_iterator auto operator()(T&& t) const noexcept(noexcept(ranges::rbegin(static_cast<CT<T>&&>(t)))) {
                return ranges::rbegin(static_cast<CT<T>&&>(t));
            }
        };

        inline namespace __fn_objects {
            inline constexpr __crbegin_fn crbegin;
        }

        struct __crend_fn {
        private:
            template<class T>
            using CT = conditional_t<is_lvalue_reference_v<T>, const remove_reference_t<T>&, const T>;

            template<class T>
            using iterator_t = decltype(ranges::begin(declval<T&>()));

        public:
            template<class T>
            constexpr sentinel_for<iterator_t<T>> auto operator()(T&& t) const noexcept(noexcept(ranges::rend(static_cast<CT<T>&&>(t)))) {
                return ranges::rend(static_cast<CT<T>&&>(t));
            }
        };

        inline namespace __fn_objects {
            inline constexpr __crend_fn crend;
        }

        template<class>
        inline constexpr bool disable_sized_range = false;

        namespace __size_internal {
            void size(auto&) = delete;
            void size(const auto&) = delete;

            struct size_fn {
            private:
                template<class T>
                static consteval bool is_noexcept() noexcept {
                    using unqualified_t = remove_cvref_t<T>;
                    if constexpr (is_unbounded_array_v<unqualified_t>) {
                        return true;
                    } else if constexpr (is_array_v<unqualified_t>) {
                        return __internal::decay_copy(extent_v<T>);
                    } else if constexpr (!disable_sized_range<unqualified_t> && requires (T&& t) { __internal::decay_copy(forward<T>(t).size()); }) {
                        return __internal::decay_copy(declval<T>().size());
                    } else if constexpr ((is_class_v<unqualified_t> || is_enum_v<unqualified_t>) && !disable_sized_range<unqualified_t> && requires (T&& t) {
                        __internal::decay_copy(size(forward<T>(t)));
                    }) {
                        return __internal::decay_copy(size(declval<T>()));
                    } else if constexpr (requires (T&& t) {
                        make_unsigned_t<decltype(ranges::end(forward<T>(t)) - ranges::begin(forward<T>(t)))>(ranges::end(forward<T>(t)) - ranges::begin(forward<T>(t)));
                        { ranges::begin(forward<T>(t)) } -> forward_iterator;
                        { ranges::end(forward<T>(t)) } -> sized_sentinel_for<decltype(ranges::begin(forward<T>(t)))>;
                    }) {
                        using unsigned_t = make_unsigned_t<decltype(ranges::end(declval<T>()) - ranges::begin(declval<T>()))>;
                        return unsigned_t(ranges::end(declval<T>()) - ranges::begin(declval<T>()));
                    } else {
                        return true;
                    }
                }
            public:
                template<class T>
                constexpr auto operator()(T&& t) const noexcept(is_noexcept<T>()) {
                    using unqualified_t = remove_cvref_t<T>;
                    if constexpr (is_unbounded_array_v<unqualified_t>) {
                        __internal::always_illformed<T>();
                    } else if constexpr (is_array_v<unqualified_t>) {
                        return __internal::decay_copy(extent_v<T>);
                    } else if constexpr (!disable_sized_range<unqualified_t> && requires { __internal::decay_copy(forward<T>(t).size()); }) {
                        return __internal::decay_copy(forward<T>(t).size());
                    } else if constexpr ((is_class_v<unqualified_t> || is_enum_v<unqualified_t>) && !disable_sized_range<unqualified_t> && requires {
                        __internal::decay_copy(size(forward<T>(t)));
                    }) {
                        return __internal::decay_copy(size(forward<T>(t)));
                    } else if constexpr (requires {
                        make_unsigned_t<decltype(ranges::end(forward<T>(t)) - ranges::begin(forward<T>(t)))>(ranges::end(forward<T>(t)) - ranges::begin(forward<T>(t)));
                        { ranges::begin(forward<T>(t)) } -> forward_iterator;
                        { ranges::end(forward<T>(t)) } -> sized_sentinel_for<decltype(ranges::begin(forward<T>(t)))>;
                    }) {
                        using unsigned_t = make_unsigned_t<decltype(ranges::end(forward<T>(t)) - ranges::begin(forward<T>(t)))>;
                        return unsigned_t(ranges::end(forward<T>(t)) - ranges::begin(forward<T>(t)));
                    } else {
                        __internal::always_illformed<T>();
                    }
                }
            };
        }

        inline namespace __fn_objects {
            inline constexpr __size_internal::size_fn size;
        }

        struct __ssize_fn {
        private:
            template<class T>
            using range_difference_t = iter_difference_t<decltype(ranges::begin(declval<T&>()))>;
        public:
            template<class T>
            constexpr auto operator()(T&& t) const noexcept(noexcept(ranges::size(forward<T>(t)))) {
                if constexpr (numeric_limits<range_difference_t<T>>::digits < numeric_limits<std::ptrdiff_t>::digits) {
                    return static_cast<std::ptrdiff_t>(ranges::size(forward<T>(t)));
                } else {
                    return static_cast<range_difference_t<T>>(ranges::size(forward<T>(t)));
                }
            }
        };

        inline namespace __fn_objects {
            inline constexpr __ssize_fn ssize;
        }

        struct __empty_fn {
        private:
            template<class T>
            static consteval bool is_noexcept() noexcept {
                using unqualified_t = remove_cvref_t<T>;
                if constexpr (is_unbounded_array_v<unqualified_t>) {
                    __internal::always_illformed<T>();
                } else if constexpr (requires (T&& t) { bool(forward<T>(t).empty()); }) {
                    return bool(declval<T>().empty());
                } else if constexpr (requires (T&& t) { ranges::size(forward<T>(t)) == 0; }) {
                    return ranges::size(declval<T>()) == 0;
                } else if constexpr (requires (T&& t) {
                    bool(ranges::begin(forward<T>(t)) == ranges::end(forward<T>(t)));
                    { ranges::begin(forward<T>(t)) } -> forward_iterator;
                }) {
                    return bool(ranges::begin(declval<T>()) == ranges::end(declval<T>()));
                } else {
                    __internal::always_illformed<T>();
                }
            }
        public:
            template<class T>
            constexpr bool operator()(T&& t) const noexcept(is_noexcept<T>()) {
                using unqualified_t = remove_cvref_t<T>;
                if constexpr (is_unbounded_array_v<unqualified_t>) {
                    __internal::always_illformed<T>();
                } else if constexpr (requires { bool(forward<T>(t).empty()); }) {
                    return bool(forward<T>(t).empty());
                } else if constexpr (requires { ranges::size(forward<T>(t)) == 0; }) {
                    return ranges::size(forward<T>(t)) == 0;
                } else if constexpr (requires {
                    bool(ranges::begin(forward<T>(t)) == ranges::end(forward<T>(t)));
                    { ranges::begin(forward<T>(t)) } -> forward_iterator;
                }) {
                    return bool(ranges::begin(forward<T>(t)) == ranges::end(forward<T>(t)));
                } else {
                    __internal::always_illformed<T>();
                }
            }
        };

        inline namespace __fn_objects {
            inline constexpr __empty_fn empty;
        }

        struct __data_fn {
        private:
            template<class T>
            static consteval bool is_noexcept() noexcept {
                using unqualified_t = remove_cvref_t<T>;
                if constexpr (is_array_v<unqualified_t> && !__internal::is_complete<remove_all_extents_t<unqualified_t>>::value) {
                    return true;
                } else if constexpr (requires (T&& t) {
                    __internal::decay_copy(forward<T>(t).data());
                    requires is_pointer_v<decltype(__internal::decay_copy(forward<T>(t).data()))>;
                }) {
                    return noexcept(__internal::decay_copy(declval<T>().data()));
                } else if constexpr (requires (T&& t) { { ranges::begin(forward<T>(t)) } -> contiguous_iterator; }) {
                    return noexcept(to_address(ranges::begin(declval<T>())));
                } else {
                    return true;
                }
            }

        public:
            template<class T>
            requires is_lvalue_reference_v<T> || enable_borrowed_range<remove_cvref_t<T>>
            constexpr auto operator()(T&& t) const noexcept(is_noexcept<T>()) {
                using unqualified_t = remove_cvref_t<T>;
                if constexpr (is_array_v<unqualified_t> && !__internal::is_complete<remove_all_extents_t<unqualified_t>>::value) {
                    __internal::always_illformed<T>();
                } else if constexpr (requires {
                    __internal::decay_copy(forward<T>(t).data());
                    requires is_pointer_v<decltype(__internal::decay_copy(forward<T>(t).data()))>;
                }) {
                    return __internal::decay_copy(forward<T>(t).data());
                } else if constexpr (requires { { ranges::begin(forward<T>(t)) } -> contiguous_iterator; }) {
                    return to_address(ranges::begin(forward<T>(t)));
                } else {
                    __internal::always_illformed<T>();
                }
            }
        };

        inline namespace __fn_objects {
            inline constexpr __data_fn data;
        }

        struct __cdata_fn {
        private:
            template<class T>
            using CT = conditional_t<is_lvalue_reference_v<T>, const remove_reference_t<T>&, const T>;
        public:
            template<class T>
            constexpr auto operator()(T&& t) const noexcept(noexcept(ranges::data(static_cast<CT<T>&&>(t)))) {
                return ranges::data(static_cast<CT<T>&&>(t));
            }
        };

        inline namespace __fn_objects {
            inline constexpr __cdata_fn cdata;
        }

        /* 23.4.4 Range iterator operations */
        struct __advance_fn {
            template<input_or_output_iterator I>
            constexpr void operator()(I& i, iter_difference_t<I> n) const {
                if constexpr (is_same_v<typename iterator_traits<I>::iterator_category, random_access_iterator_tag>) {
                    i += n;
                } else {
                    if (n >= 0) {
                        while (n--) {
                            i++;
                        }
                    } else {
                        while (n++) {
                            i--;
                        }
                    }
                }
            }

            template<input_or_output_iterator I, sentinel_for<I> S>
            constexpr void operator()(I& i, S bound) const {
                if constexpr (assignable_from<I&, S>) {
                    i = move(bound);
                } else if constexpr (sized_sentinel_for<S, I>) {
                    this->operator()(i, bound - i);
                } else {
                    while (i != bound) {
                        i++;
                    }
                }
            }

            template<input_or_output_iterator I, sentinel_for<I> S>
            constexpr iter_difference_t<I> operator()(I& i, iter_difference_t<I> n, S bound) const {
                if constexpr (sized_sentinel_for<S, I>) {
                    // std::abs is not constexpr.
                    constexpr auto abs = [](const iter_difference_t<I> x) {
                        return x < 0 ? -x : x;
                    };

                    if (const auto dist = abs(n) - abs(bound - i); dist >= 0) {
                        this->operator()(i, bound);
                        return dist;
                    } else {
                        this->operator()(i, n);
                        return 0;
                    }
                } else {
                    if (n >= 0) {
                        while (i != bound && n != 0) {
                            i++;
                            n--;
                        }
                    } else {
                        while (i != bound && n != 0) {
                            i--;
                            n++;
                        }
                    }

                    return n;
                }
            }
        };

        inline namespace __fn_objects {
            inline constexpr ::std::ranges::__advance_fn advance;
        }

        struct __distance_fn {
        private:
            template<class T>
            using range_difference_t = iter_difference_t<decltype(ranges::begin(declval<T&>()))>;
        public:
            template<input_or_output_iterator I, sentinel_for<I> S>
            constexpr iter_difference_t<I> operator()(I first, S last) const {
                if constexpr (sized_sentinel_for<S, I>) {
                    return last - first;
                } else {
                    iter_difference_t<I> i = 0;
                    while (first != last) {
                        first++;
                        i++;
                    }

                    return i;
                }
            }

            template<class R>
            requires requires (R& r) {
                ranges::begin(r);
                ranges::end(r);
            }
            constexpr range_difference_t<R> operator()(R&& r) const {
                if constexpr (requires (R& r) { ranges::size(r); }) {
                    return static_cast<range_difference_t<R>>(ranges::size(r));
                } else {
                    return this->operator()(ranges::begin(r), ranges::end(r));
                }
            }
        };

        inline namespace __fn_objects {
            inline constexpr ::std::ranges::__distance_fn distance;
        }

        struct __next_fn {
            template<input_or_output_iterator I>
            constexpr I operator()(I x) const {
                ++x;
                return x;
            }

            template<input_or_output_iterator I>
            constexpr I operator()(I x, iter_difference_t<I> n) const {
                ranges::advance(x, n);
                return x;
            }

            template<input_or_output_iterator I, sentinel_for<I> S>
            constexpr I operator()(I x, S bound) const {
                ranges::advance(x, bound);
                return x;
            }

            template<input_or_output_iterator I, sentinel_for<I> S>
            constexpr I operator()(I x, iter_difference_t<I> n, S bound) const {
                ranges::advance(x, n, bound);
                return x;
            }
        };

        inline namespace __fn_objects {
            inline constexpr ::std::ranges::__next_fn next;
        }

        struct prev_fn {
            template<input_or_output_iterator I>
            constexpr I operator()(I x) const {
                --x;
                return x;
            }

            template<input_or_output_iterator I>
            constexpr I operator()(I x, iter_difference_t<I> n) const {
                ranges::advance(x, -n);
                return x;
            }

            template<input_or_output_iterator I>
            constexpr I operator()(I x, iter_difference_t<I> n, I bound) const {
                ranges::advance(x, -n, bound);
                return x;
            }
        };

        inline namespace __fn_objects {
            inline constexpr ::std::ranges::prev_fn prev;
        }
    }

    /* 23.5.1 Reverse iterators */
    template<class Iterator>
    requires __internal::legacy_bidirectional_iterator<Iterator> || bidirectional_iterator<Iterator>
    class reverse_iterator {
    public:
        using iterator_type = Iterator;
        using iterator_concept = conditional_t<random_access_iterator<Iterator>, random_access_iterator_tag, bidirectional_iterator_tag>;
        using iterator_category = conditional_t<derived_from<typename iterator_traits<Iterator>::iterator_category, random_access_iterator_tag>,
            random_access_iterator_tag, typename iterator_traits<Iterator>::iterator_category>;
        using value_type = iter_value_t<Iterator>;
        using difference_type = iter_difference_t<Iterator>;
        using pointer = typename iterator_traits<Iterator>::pointer;
        using reference = iter_reference_t<Iterator>;

        constexpr reverse_iterator() : current() {}
        constexpr explicit reverse_iterator(Iterator x) : current(x) {}

        template<class U>
        constexpr reverse_iterator(const reverse_iterator<U>& u) : current(u.current) {}

        template<class U>
        constexpr reverse_iterator& operator=(const reverse_iterator<U>& u) {
            current = u.base();
            return *this;
        }

        constexpr Iterator base() const {
            return current;
        }

        constexpr reference operator*() const {
            Iterator tmp = current;
            return *--tmp;
        }

        constexpr pointer operator->() const
        requires is_pointer_v<Iterator> || requires (const Iterator i) { i.operator->(); } {
            if constexpr (is_pointer_v<Iterator>) {
                return prev(current);
            } else {
                return prev(current).operator->();
            }
        }

        constexpr reverse_iterator& operator++() {
            --current;
            return *this;
        }

        constexpr reverse_iterator operator++(int) {
            reverse_iterator tmp = *this;
            --current;
            return tmp;
        }

        constexpr reverse_iterator& operator--() {
            ++current;
            return *this;
        }

        constexpr reverse_iterator operator--(int) {
            reverse_iterator tmp = *this;
            ++current;
            return tmp;
        }

        constexpr reverse_iterator operator+(difference_type n) const
        requires __internal::legacy_random_access_iterator<Iterator> || random_access_iterator<Iterator> {
            return reverse_iterator(current - n);
        }

        constexpr reverse_iterator& operator+=(difference_type n)
        requires __internal::legacy_random_access_iterator<Iterator> || random_access_iterator<Iterator> {
            current -= n;
            return *this;
        }

        constexpr reverse_iterator operator-(difference_type n) const
        requires __internal::legacy_random_access_iterator<Iterator> || random_access_iterator<Iterator> {
            return reverse_iterator(current + n);
        }

        constexpr reverse_iterator& operator-=(difference_type n)
        requires __internal::legacy_random_access_iterator<Iterator> || random_access_iterator<Iterator> {
            current += n;
            return *this;
        }

        constexpr auto operator[](difference_type n) const -> decltype(this->current[-n - 1])
        requires __internal::legacy_random_access_iterator<Iterator> || random_access_iterator<Iterator> {
            return current[-n - 1];
        }

        friend constexpr iter_rvalue_reference_t<Iterator> iter_move(const reverse_iterator& i)
        noexcept(is_nothrow_copy_constructible_v<Iterator> && noexcept(ranges::iter_move(--declval<Iterator&>()))) {
            Iterator tmp = i.base();
            return ranges::iter_move(--tmp);
        }

        template<indirectly_swappable<Iterator> Iterator2>
        friend constexpr void iter_swap(const reverse_iterator& x, const reverse_iterator<Iterator2>& y)
        noexcept(is_nothrow_copy_constructible_v<Iterator> && is_nothrow_copy_constructible_v<Iterator2> && noexcept(ranges::iter_swap(--declval<Iterator&>(), --declval<Iterator2&>()))) {
            Iterator xtmp = x.base();
            Iterator ytmp = y.base();
            ranges::iter_swap(--xtmp, --ytmp);
        }

    protected:
        Iterator current;
    };

    template<class Iterator1, class Iterator2>
    requires requires (const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y) {
        { x.base() == y.base() } -> convertible_to<bool>;
    }
    constexpr bool operator==(const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y) {
        return x.base() == y.base();
    }

    template<class Iterator1, class Iterator2>
    requires requires (const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y) {
        { x.base() != y.base() } -> convertible_to<bool>;
    }
    constexpr bool operator!=(const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y) {
        return x.base() != y.base();
    }

    template<class Iterator1, class Iterator2>
    requires requires (const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y) {
        { x.base() < y.base() } -> convertible_to<bool>;
    }
    constexpr bool operator<(const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y) {
        return x.base() < y.base();
    }

    template<class Iterator1, class Iterator2>
    requires requires (const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y) {
        { x.base() > y.base() } -> convertible_to<bool>;
    }
    constexpr bool operator>(const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y) {
        return x.base() > y.base();
    }

    template<class Iterator1, class Iterator2>
    requires requires (const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y) {
        { x.base() <= y.base() } -> convertible_to<bool>;
    }
    constexpr bool operator<=(const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y) {
        return x.base() <= y.base();
    }

    template<class Iterator1, class Iterator2>
    requires requires (const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y) {
        { x.base() >= y.base() } -> convertible_to<bool>;
    }
    constexpr bool operator>=(const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y) {
        return x.base() >= y.base();
    }

    template<class Iterator1, three_way_comparable_with<Iterator1> Iterator2>
    constexpr compare_three_way_result_t<Iterator1, Iterator2> operator<=>(const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y) {
        return y.base() <=> x.base();
    }

    template<class Iterator1, class Iterator2>
    constexpr auto operator-(const reverse_iterator<Iterator1>& x, const reverse_iterator<Iterator2>& y) -> decltype(y.base() - x.base()) {
        return y.base() - x.base();
    }

    template<class Iterator>
    constexpr reverse_iterator<Iterator> operator+(iter_difference_t<Iterator> n, const reverse_iterator<Iterator>& x) {
        return reverse_iterator<Iterator>(x.base() - n);
    }

    template<class Iterator>
    constexpr reverse_iterator<Iterator> make_reverse_iterator(Iterator i) {
        return reverse_iterator<Iterator>(i);
    }

    template<class Iterator1, class Iterator2>
    requires (!sized_sentinel_for<Iterator1, Iterator2>)
    inline constexpr bool disable_sized_sentinel_for<reverse_iterator<Iterator1>, reverse_iterator<Iterator2>> = true;

    /* 23.5.2 Insert iterators */
    template<class Container>
    class back_insert_iterator {
    protected:
        Container* container = nullptr;

    public:
        using iterator_category = output_iterator_tag;
        using value_type = void;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = void;
        using container_type = Container;

        constexpr back_insert_iterator() noexcept = default;
        constexpr explicit back_insert_iterator(Container& x) : container(addressof(x)) {}
        constexpr back_insert_iterator& operator=(const typename Container::value_type& value) {
            container->push_back(value);
            return *this;
        }

        constexpr back_insert_iterator& operator=(typename Container::value_type&& value) {
            container->push_back(move(value));
            return *this;
        }

        constexpr back_insert_iterator& operator*() {
            return *this;
        }

        constexpr back_insert_iterator& operator++() {
            return *this;
        }

        constexpr back_insert_iterator operator++(int) {
            return *this;
        }
    };

    template<class Container>
    constexpr back_insert_iterator<Container> back_inserter(Container& x) {
        return back_insert_iterator(x);
    }

    template<class Container>
    class front_insert_iterator {
    protected:
        Container* container = nullptr;

    public:
        using iterator_category = output_iterator_tag;
        using value_type = void;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = void;
        using container_type = Container;

        constexpr front_insert_iterator() noexcept = default;
        constexpr explicit front_insert_iterator(Container& x) : container(addressof(x)) {}
        constexpr front_insert_iterator& operator=(const typename Container::value_type& value) {
            container->push_front(value);
            return *this;
        }

        constexpr front_insert_iterator& operator=(typename Container::value_type&& value) {
            container->push_front(move(value));
            return *this;
        }

        constexpr front_insert_iterator& operator*() {
            return *this;
        }

        constexpr front_insert_iterator& operator++() {
            return *this;
        }

        constexpr front_insert_iterator operator++(int) {
            return *this;
        }
    };

    template<class Container>
    constexpr front_insert_iterator<Container> front_inserter(Container& x) {
        return front_insert_iterator(x);
    }

    template<class Container>
    class insert_iterator {
    private:
        using iterator_t = decltype(ranges::begin(declval<Container&>()));

    protected:
        Container* container = nullptr;
        iterator_t iter = iterator_t();

    public:
        using iterator_category = output_iterator_tag;
        using value_type = void;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = void;
        using container_type = Container;

        insert_iterator() = default;
        constexpr insert_iterator(Container& x, iterator_t i) : container(addressof(x)), iter(i) {}
        constexpr insert_iterator& operator=(const typename Container::value_type& value) {
            iter = container->insert(iter, value);
            ++iter;
            return *this;
        }

        constexpr insert_iterator& operator=(typename Container::value_type&& value) {
            iter = container->insert(iter, move(value));
            ++iter;
            return *this;
        }

        constexpr insert_iterator& operator*() {
            return *this;
        }

        constexpr insert_iterator& operator++() {
            return *this;
        }

        constexpr insert_iterator& operator++(int) {
            return *this;
        }
    };

    template<class Container>
    constexpr insert_iterator<Container> inserter(Container& x, decltype(ranges::begin(declval<Container&>())) i) {
        return insert_iterator<Container>(x, i);
    }

    /* 23.5.3 Move iterators and sentinels */
    template<semiregular S>
    class move_sentinel {
    private:
        S last;
    public:
        constexpr move_sentinel() : last() {}

        constexpr explicit move_sentinel(S s) : last(move(s)) {}

        template<class S2>
        requires convertible_to<const S2&, S>
        constexpr move_sentinel(const move_sentinel<S2>& s) : last(s) {}

        template<class S2>
        requires assignable_from<S&, const S2&>
        constexpr move_sentinel& operator=(const move_sentinel<S2>& s) {
            last = s.last;
            return *this;
        }

        constexpr S base() const {
            return last;
        }
    };

    template<class Iter>
    requires __internal::legacy_input_iterator<Iter> || input_iterator<Iter>
    class move_iterator {
    private:
        Iter current;
    public:
        using iterator_type = Iter;
        using iterator_concept = input_iterator_tag;
        using iterator_category = conditional_t<derived_from<typename iterator_traits<Iter>::iterator_category, random_access_iterator_tag>,
            random_access_iterator_tag, typename iterator_traits<Iter>::iterator_category>;
        using value_type = iter_value_t<Iter>;
        using difference_type = iter_difference_t<Iter>;
        using pointer = Iter;
        using reference = iter_rvalue_reference_t<Iter>;

        constexpr move_iterator() : current() {}

        constexpr explicit move_iterator(Iter i) : current(move(i)) {}

        template<convertible_to<Iter> U>
        constexpr move_iterator(const move_iterator<U>& u) : current(u.base()) {}

        template<convertible_to<Iter> U>
        constexpr move_iterator& operator=(const move_iterator<U>& u) {
            current = u.base();
            return *this;
        }

        constexpr iterator_type base() const &
        requires copy_constructible<Iter> {
            return current;
        }

        constexpr iterator_type base() && {
            return move(current);
        }

        constexpr reference operator*() const {
            return ranges::iter_move(current);
        }

        constexpr move_iterator& operator++() {
            current++;
            return *this;
        }

        constexpr auto operator++(int) {
            if constexpr (forward_iterator<Iter>) {
                move_iterator tmp = *this;
                current++;
                return tmp;
            } else {
                current++;
            }
        }

        constexpr move_iterator& operator--()
        requires __internal::legacy_bidirectional_iterator<Iter> || bidirectional_iterator<Iter> {
            current--;
            return *this;
        }

        constexpr move_iterator operator--(int)
        requires __internal::legacy_bidirectional_iterator<Iter> || bidirectional_iterator<Iter> {
            move_iterator tmp = *this;
            current--;
            return tmp;
        }

        constexpr move_iterator operator+(difference_type n) const
        requires __internal::legacy_random_access_iterator<Iter> || random_access_iterator<Iter> {
            return move_iterator(current + n);
        }

        constexpr move_iterator& operator+=(difference_type n)
        requires __internal::legacy_random_access_iterator<Iter> || random_access_iterator<Iter> {
            current += n;
            return *this;
        }

        constexpr move_iterator operator-(difference_type n) const
        requires __internal::legacy_random_access_iterator<Iter> || random_access_iterator<Iter> {
            return move_iterator(current - n);
        }

        constexpr move_iterator& operator-=(difference_type n)
        requires __internal::legacy_random_access_iterator<Iter> || random_access_iterator<Iter> {
            current -= n;
            return *this;
        }

        constexpr reference operator[](difference_type n) const
        requires __internal::legacy_random_access_iterator<Iter> || random_access_iterator<Iter> {
            return ranges::iter_move(current + n);
        }

        template<sentinel_for<Iter> S>
        requires requires (const move_iterator& x, const move_sentinel<S>& y) { { x.base() == y.base() } -> convertible_to<bool>; }
        friend constexpr bool operator==(const move_iterator& x, const move_sentinel<S>& y) {
            return x.base() == y.base();
        }

        template<sized_sentinel_for<Iter> S>
        friend constexpr iter_difference_t<Iter> operator-(const move_sentinel<S>& x, const move_iterator& y) {
            return x.base() - y.base();
        }

        template<sized_sentinel_for<Iter> S>
        friend constexpr iter_difference_t<Iter> operator-(const move_iterator& x, const move_sentinel<S>& y) {
            return x.base() - y.base();
        }

        friend constexpr iter_rvalue_reference_t<Iter> iter_move(const move_iterator& i) noexcept(noexcept(ranges::iter_move(i.current))) {
            return ranges::iter_move(i.current);
        }

        template<indirectly_swappable<Iter> Iter2>
        friend constexpr void iter_swap(const move_iterator& x, const move_iterator<Iter2>& y) noexcept(noexcept(ranges::iter_swap(x.current, y.current))) {
            ranges::iter_swap(x.current, y.current);
        }
    };

    template<class Iter1, class Iter2>
    requires requires (const move_iterator<Iter1>& x, const move_iterator<Iter2>& y) { { x.base() == y.base() } -> convertible_to<bool>; }
    constexpr bool operator==(const move_iterator<Iter1>& x, const move_iterator<Iter2>& y) {
        return x.base() == y.base();
    }

    template<class Iter1, class Iter2>
    requires requires (const move_iterator<Iter1>& x, const move_iterator<Iter2>& y) { { x.base() < y.base() } -> convertible_to<bool>; }
    constexpr bool operator<(const move_iterator<Iter1>& x, const move_iterator<Iter2>& y) {
        return x.base() < y.base();
    }

    template<class Iter1, class Iter2>
    requires requires (const move_iterator<Iter1>& x, const move_iterator<Iter2>& y) { { y.base() < x.base() } -> convertible_to<bool>; }
    constexpr bool operator>(const move_iterator<Iter1>& x, const move_iterator<Iter2>& y) {
        return y < x;
    }

    template<class Iter1, class Iter2>
    requires requires (const move_iterator<Iter1>& x, const move_iterator<Iter2>& y) { { y.base() < x.base() } -> convertible_to<bool>; }
    constexpr bool operator<=(const move_iterator<Iter1>& x, const move_iterator<Iter2>& y) {
        return !(y < x);
    }

    template<class Iter1, class Iter2>
    requires requires (const move_iterator<Iter1>& x, const move_iterator<Iter2>& y) { { x.base() < y.base() } -> convertible_to<bool>; }
    constexpr bool operator>=(const move_iterator<Iter1>& x, const move_iterator<Iter2>& y) {
        return !(x < y);
    }

    template<class Iter1, three_way_comparable_with<Iter1> Iter2>
    constexpr compare_three_way_result_t<Iter1, Iter2> operator<=>(const move_iterator<Iter1>& x, const move_iterator<Iter2>& y) {
        return x.base() <=> y.base();
    }

    template<class Iter1, class Iter2>
    constexpr auto operator-(const move_iterator<Iter1>& x, const move_iterator<Iter2>& y) -> decltype(x.base() - y.base()) {
        return x.base() - y.base();
    }

    template<class Iter>
    requires requires (iter_difference_t<Iter> n, const move_iterator<Iter>& x) { { x + n } -> same_as<Iter>; }
    constexpr move_iterator<Iter> operator+(iter_difference_t<Iter> n, const move_iterator<Iter>& x) {
        return x + n;
    }

    template<class Iter>
    constexpr move_iterator<Iter> make_move_iterator(Iter i) {
        return move_iterator<Iter>(move(i));
    }

    /* 23.5.4 Common iterators */
    template<input_or_output_iterator I, sentinel_for<I> S>
    requires (!same_as<I, S> && copyable<I>)
    class common_iterator {
    private:
        struct storage {
            union {
                char dummy;
                I i;
                S s;
            };

            enum { None, Iterator, Sentinel } curr_member;

            constexpr storage() noexcept : dummy(0), curr_member(None) {}
            constexpr storage(I i) noexcept : i(move(i)), curr_member(Iterator) {}
            constexpr storage(S s) noexcept : s(move(s)), curr_member(Sentinel) {}

            constexpr ~storage() noexcept {
                switch (curr_member) {
                    case Iterator:
                        if constexpr (requires { i.~I(); }) {
                            i.~I();
                        }
                        break;

                    case Sentinel:
                        if constexpr (requires { s.~S(); }) {
                            s.~S();
                        }
                        break;
                }
            }

            template<class Ip, class Sp>
            constexpr storage(const typename common_iterator<Ip, Sp>::storage& storage) : dummy(0) {
                ~storage();
                switch (storage.curr_member) {
                    case Iterator:
                        i = storage.i;
                        break;

                    case Sentinel:
                        s = storage.s;
                        break;
                }
            }

            template<class Ip, class Sp>
            constexpr storage& operator=(const typename common_iterator<Ip, Sp>::storage& storage) {
                ~storage();
                switch (storage.curr_member) {
                    case None:
                        dummy = 0;
                        break;

                    case Iterator:
                        i = storage.i;
                        break;

                    case Sentinel:
                        s = storage.s;
                        break;
                }
            }
        };

        storage s;
    public:
        constexpr common_iterator() = default;
        constexpr common_iterator(I i) : s(move(i)) {}
        constexpr common_iterator(S s) : s(move(s)) {}

        template<class I2, class S2>
        requires convertible_to<const I2&, I> && convertible_to<const S2&, S>
        constexpr common_iterator(const common_iterator<I2, S2>& x) : s(x.s) {}


        template<class I2, class S2>
        requires convertible_to<const I2&, I> && convertible_to<const S2&, S>
            && assignable_from<I&, const I2&> && assignable_from<S&, const S2&>
        constexpr common_iterator& operator=(const common_iterator<I2, S2>& x) {
            s = x.s;
            return *this;
        }

        decltype(auto) operator*() {
            return *s.i;
        }

        decltype(auto) operator*() const
        requires __internal::dereferenceable<const I> {
            return *s.i;
        }

    private:
        class proxy {
            iter_value_t<I> keep;
            proxy(iter_reference_t<I>&& x) : keep(move(x)) {}
        public:
            const iter_value_t<I>* operator->() const {
                return addressof(keep);
            }
        };

    public:
        decltype(auto) operator->() const
        requires indirectly_readable<const I>
            && (requires (const I& i) { i.operator->(); } || is_reference_v<iter_reference_t<I>> || constructible_from<iter_value_t<I>, iter_reference_t<I>>) {
            if constexpr (is_pointer_v<I> || requires { s.i.operator->(); }) {
                return s.i;
            } else if (is_reference_v<iter_reference_t<I>>) {
                auto&& tmp = *s.i;
                return addressof(tmp);
            } else {
                return proxy(*s.i);
            }
        }

        common_iterator& operator++() {
            s.i++;
            return *this;
        }

        decltype(auto) operator++(int) {
            if constexpr (forward_iterator<I>) {
                common_iterator tmp = *this;
                ++*this;
                return tmp;
            } else {
                return s.i++;
            }
        }

        template<class I2, sentinel_for<I> S2>
        requires sentinel_for<S, I2>
        friend bool operator==(const common_iterator& x, const common_iterator<I2, S2>& y) {
            if (static_cast<std::size_t>(x.s.curr_member) == static_cast<std::size_t>(y.s.curr_member)) {
                return true;
            } else if (x.s.curr_member == common_iterator::storage::Sentinel && y.s.curr_member == common_iterator<I2, S2>::storage::Iterator) {
                return x.s.s == y.s.i;
            } else {
                return x.s.i == y.s.s;
            }
        }

        template<class I2, sentinel_for<I> S2>
        requires sentinel_for<S, I2> && equality_comparable_with<I, I2>
        friend bool operator==(const common_iterator& x, const common_iterator<I2, S2>& y) {
            if (x.s.curr_member == common_iterator::storage::Sentinel && y.s.curr_member == common_iterator<I2, S2>::storage::Sentinel) {
                return true;
            } else if (x.s.curr_member == common_iterator::storage::Sentinel && y.s.curr_member == common_iterator<I2, S2>::storage::Iterator) {
                return x.s.s == y.s.i;
            } else if (x.s.curr_member == common_iterator::storage::Iterator && y.s.curr_member == common_iterator<I2, S2>::storage::Iterator) {
                return x.s.i == y.s.i;
            } else {
                return x.s.i == y.s.s;
            }
        }

        template<sized_sentinel_for<I> I2, sized_sentinel_for<I> S2>
        requires sized_sentinel_for<S, I2>
        friend iter_difference_t<I2> operator-(const common_iterator& x, const common_iterator<I2, S2>& y) {
            if (x.s.curr_member == common_iterator::storage::Sentinel && y.s.curr_member == common_iterator<I2, S2>::storage::Sentinel) {
                return 0;
            } else if (x.s.curr_member == common_iterator::storage::Sentinel && y.s.curr_member == common_iterator<I2, S2>::storage::Iterator) {
                return x.s.s - y.s.i;
            } else if (x.s.curr_member == common_iterator::storage::Iterator && y.s.curr_member == common_iterator<I2, S2>::storage::Iterator) {
                return x.s.i - y.s.i;
            } else {
                return x.s.i - y.s.s;
            }
        }

        friend iter_rvalue_reference_t<I> iter_move(const common_iterator& i)
        noexcept(noexcept(ranges::iter_move(declval<const I&>())))
        requires input_iterator<I> {
            return ranges::iter_move(i.s.i);
        }

        template<indirectly_swappable<I> I2, class S2>
        friend void iter_swap(const common_iterator& x, const common_iterator<I2, S2>& y)
        noexcept(noexcept(ranges::iter_swap(declval<const I&>(), declval<const I2&>()))) {
            ranges::iter_swap(x.s.i, y.s.i);
        }
    };

    template<class I, class S>
    struct incrementable_traits<common_iterator<I, S>> {
        using difference_type = iter_difference_t<I>;
    };

    template<input_iterator I, class S>
    struct iterator_traits<common_iterator<I, S>> {
    private:
        static constexpr auto get_pointer() noexcept {
            if constexpr (requires (const common_iterator<I, S>& a) { a.operator->(); }) {
                return declval<decltype(declval<const common_iterator<I, S>&>().operator->())>();
            }
        }
    public:
        using iterator_concept = conditional_t<forward_iterator<I>, forward_iterator_tag, input_iterator_tag>;
        using iterator_category = conditional_t<derived_from<typename iterator_traits<I>::iterator_category, forward_iterator_tag>, forward_iterator_tag, input_iterator_tag>;
        using value_type = iter_value_t<I>;
        using difference_type = iter_difference_t<I>;
        using pointer = decltype(get_pointer());
        using reference = iter_reference_t<I>;
    };

    /* 23.5.5 Default sentinel */
    struct default_sentinel_t {};

    inline constexpr default_sentinel_t default_sentinel;

    /* 23.5.7 Unreachable sentinel */
    struct unreachable_sentinel_t {
        template<weakly_incrementable I>
        friend constexpr bool operator==(unreachable_sentinel_t, const I&) noexcept {
            return false;
        }
    };

    inline constexpr unreachable_sentinel_t unreachable_sentinel;

    /* 23.5.6 Counted iterators */
    template<input_or_output_iterator I>
    class counted_iterator {
    private:
        I current = I();
        iter_difference_t<I> length = 0;
    public:
        using iterator_type = I;

        constexpr counted_iterator() = default;
        constexpr counted_iterator(I x, iter_difference_t<I> n) : current(move(x)), length(n) {}

        template<class I2>
        requires convertible_to<const I2&, I>
        constexpr counted_iterator(const counted_iterator<I2>& x) : current(x.current), length(x.length) {}

        template<class I2>
        requires assignable_from<I&, const I2&>
        constexpr counted_iterator& operator=(const counted_iterator<I2>& x) {
            current = x.current;
            length = x.length;
            return *this;
        }

        constexpr I base() const &
        requires copy_constructible<I> {
            return current;
        }

        constexpr I base() && {
            return move(current);
        }

        constexpr iter_difference_t<I> count() const noexcept {
            return length;
        }

        constexpr decltype(auto) operator*() {
            return *current;
        }

        constexpr decltype(auto) operator*() const
        requires __internal::dereferenceable<const I> {
            return *current;
        }

        constexpr counted_iterator& operator++() {
            current++;
            length--;
            return *this;
        }

        decltype(auto) operator++(int) {
            length--;
            try {
                return current++;
            } catch (...) {
                length++;
                throw;
            }
        }

        constexpr counted_iterator operator++(int)
        requires forward_iterator<I> {
            counted_iterator tmp = *this;
            *this++;
            return tmp;
        }

        constexpr counted_iterator& operator--()
        requires bidirectional_iterator<I> {
            current--;
            length++;
            return *this;
        }

        constexpr counted_iterator operator--(int)
        requires bidirectional_iterator<I> {
            counted_iterator tmp = *this;
            *this--;
            return tmp;
        }

        constexpr counted_iterator operator+(iter_difference_t<I> n) const
        requires random_access_iterator<I> {
            return counted_iterator(current + n, length - n);
        }

        friend constexpr counted_iterator operator+(iter_difference_t<I> n, const counted_iterator& x)
        requires random_access_iterator<I> {
            return x + n;
        }

        constexpr counted_iterator& operator+=(iter_difference_t<I> n)
        requires random_access_iterator<I> {
            current += n;
            length -= n;
            return *this;
        }

        constexpr counted_iterator operator-(iter_difference_t<I> n) const
        requires random_access_iterator<I> {
            return counted_iterator(current - n, length + n);
        }

        template<common_with<I> I2>
        friend constexpr iter_difference_t<I2> operator-(const counted_iterator& x, const counted_iterator<I2>& y) {
            return y.length - x.length;
        }

        friend constexpr iter_difference_t<I> operator-(const counted_iterator& x, default_sentinel_t) {
            return -x.length;
        }

        friend constexpr iter_difference_t<I> operator-(default_sentinel_t, const counted_iterator& y) {
            return y.length;
        }

        constexpr counted_iterator& operator-=(iter_difference_t<I> n)
        requires random_access_iterator<I> {
            current -= n;
            length += n;
            return *this;
        }

        constexpr decltype(auto) operator[](iter_difference_t<I> n) const
        requires random_access_iterator<I> {
            return current[n];
        }

        template<common_with<I> I2>
        friend constexpr bool operator==(const counted_iterator& x, const counted_iterator<I2>& y) {
            return x.length == y.length;
        }

        friend constexpr bool operator==(const counted_iterator& x, default_sentinel_t) {
            return x.length == 0;
        }

        template<common_with<I> I2>
        friend constexpr strong_ordering operator<=>(const counted_iterator& x, const counted_iterator<I2>& y) {
            return y.length <=> x.length;
        }

        friend constexpr iter_rvalue_reference_t<I> iter_move(const counted_iterator& i)
        noexcept(noexcept(ranges::iter_move(i.current)))
        requires input_iterator<I> {
            return ranges::iter_move(i.current);
        }

        template<indirectly_swappable<I> I2>
        friend constexpr void iter_swap(const counted_iterator& x, const counted_iterator<I2>& y)
        noexcept(noexcept(ranges::iter_swap(x.current, y.current))) {
            return ranges::iter_swap(x.current, y.current);
        }
    };

    template<class I>
    struct incrementable_traits<counted_iterator<I>> {
        using difference_type = iter_difference_t<I>;
    };

    template<input_iterator I>
    struct iterator_traits<counted_iterator<I>> : iterator_traits<I> {
        using pointer = void;
    };

    /* 23.6 Stream iterators */
    template<class T, class charT = char, class traits = char_traits<charT>, class Distance = std::ptrdiff_t>
    class istream_iterator {
    public:
        using iterator_category = input_iterator_tag;
        using value_type = T;
        using difference_type = Distance;
        using pointer = const T*;
        using reference = const T&;
        using char_type = charT;
        using traits_type = traits;
        using istream_type = basic_istream<charT, traits>;

        constexpr istream_iterator() : in_stream(nullptr), value() {}

        constexpr istream_iterator(default_sentinel_t) : in_stream(nullptr), value() {}

        istream_iterator(istream_type& s) : in_stream(addressof(s)), value() {
            operator++();
        }

        istream_iterator(const istream_iterator& x) = default;
        ~istream_iterator() = default;
        istream_iterator& operator=(const istream_iterator&) = default;

        const T& operator*() const {
            return value;
        }

        const T* operator->() const {
            return addressof(value);
        }

        istream_iterator& operator++() {
            if (!(*in_stream >> value)) {
                in_stream = nullptr;
            }
            return *this;
        }

        istream_iterator operator++(int) {
            istream_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        friend bool operator==(const istream_iterator& i, default_sentinel_t) {
            return !i.in_stream;
        }

    private:
        basic_istream<charT, traits>* in_stream;
        T value;
    };

    template<class T, class charT, class traits, class Distance>
    bool operator==(const istream_iterator<T, charT, traits, Distance>& x, const istream_iterator<T, charT, traits, Distance>& y) {
        return x.in_stream == y.in_stream;
    }

    template<class T, class charT = char, class traits = char_traits<charT>>
    class ostream_iterator {
    public:
        using iterator_category = output_iterator_tag;
        using value_type = void;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = void;
        using char_type = charT;
        using traits_type = traits;
        using ostream_type = basic_ostream<charT, traits>;

        constexpr ostream_iterator() noexcept = default;
        ostream_iterator(ostream_type& s) : out_stream(addressof(s)), delim(nullptr) {}

        ostream_iterator(ostream_type& s, const charT* delimiter) : out_stream(addressof(s)), delim(delimiter) {}

        ostream_iterator(const ostream_iterator& x) = default;
        ~ostream_iterator() = default;
        ostream_iterator& operator=(const ostream_iterator&) = default;

        ostream_iterator& operator=(const T& value) {
            *out_stream << value;
            if (delim) {
                *out_stream << delim;
            }

            return *this;
        }

        ostream_iterator& operator*() {
            return *this;
        }

        ostream_iterator& operator++() {
            return *this;
        }

        ostream_iterator& operator++(int) {
            return *this;
        }

    private:
        basic_ostream<charT, traits>* out_stream = nullptr;
        const charT* delim = nullptr;
    };

    template<class CharT, class Traits>
    class istreambuf_iterator {
    public:
        using iterator_category = input_iterator_tag;
        using value_type = CharT;
        using difference_type = typename Traits::off_type;
        using pointer = CharT*;
        using reference = CharT;
        using char_type = CharT;
        using traits = Traits;
        using int_type = typename Traits::int_type;
        using streambuf_type = basic_streambuf<CharT, Traits>;
        using istream_type = basic_istream<CharT, Traits>;

    private:
        class proxy {
            CharT keep;
            basic_streambuf<CharT, Traits>* sbuf;

            proxy(CharT c, basic_streambuf<CharT, Traits>* sbuf) : keep(c), sbuf(sbuf) {}

        public:
            CharT operator*() {
                return keep;
            }
        };

    public:
        constexpr istreambuf_iterator() noexcept : sbuf(nullptr) {}

        constexpr istreambuf_iterator(default_sentinel_t) noexcept : sbuf(nullptr) {}

        istreambuf_iterator(const istreambuf_iterator&) noexcept = default;
        ~istreambuf_iterator() = default;

        istreambuf_iterator(istream_type& s) noexcept : sbuf(s.rdbuf()) {}

        istreambuf_iterator(streambuf_type* s) noexcept : sbuf(s) {}

        istreambuf_iterator(const proxy& p) noexcept : sbuf(p.sbuf) {}

        istreambuf_iterator& operator=(const istreambuf_iterator&) noexcept = default;

        CharT operator*() const {
            return sbuf->sgetc();
        }

        istreambuf_iterator& operator++() {
            sbuf->sbumpc();
            return *this;
        }

        proxy operator++(int) {
            return proxy(sbuf->sbumpc(), sbuf);
        }

        bool equal(const istreambuf_iterator& b) const {
            if (!sbuf || !b.sbuf) {
                return sbuf == b.sbuf;
            }

            const bool a_is_eof = traits::eq_int_type(traits::eof(), sbuf->sgetc());
            const bool b_is_eof = traits::eq_int_type(traits::eof(), b.sbuf->sgetc());

            return a_is_eof == b_is_eof;
        }

        friend bool operator==(const istreambuf_iterator& i, default_sentinel_t s) {
            return i.equals(s);
        }

    private:
        streambuf_type* sbuf;
    };

    template<class charT, class traits>
    bool operator==(const istreambuf_iterator<charT, traits>& a, const istreambuf_iterator<charT, traits>& b) {
        return a.equal(b);
    }

    template<class charT, class traits>
    class ostreambuf_iterator {
    public:
        using iterator_category = output_iterator_tag;
        using value_type = void;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = void;
        using char_type = charT;
        using traits_type = traits;
        using streambuf_type = basic_streambuf<charT, traits>;
        using ostream_type = basic_ostream<charT, traits>;

        constexpr ostreambuf_iterator() noexcept = default;
        ostreambuf_iterator(ostream_type& s) noexcept : sbuf(s.rdbuf()) {}

        ostreambuf_iterator(streambuf_type* s) noexcept : sbuf(s) {}

        ostreambuf_iterator& operator=(charT c) {
            if (!failed()) {
                if (traits::eq_int_type(sbuf->sputc(c), traits::eof())) {
                    failed_flag = true;
                }
            }
            return *this;
        }

        ostreambuf_iterator& operator*() {
            return *this;
        }

        ostreambuf_iterator& operator++() {
            return *this;
        }

        ostreambuf_iterator& operator++(int) {
            return *this;
        }

        bool failed() const noexcept {
            return failed_flag;
        }

    private:
        streambuf_type* sbuf = nullptr;
        bool failed_flag = false;
    };

    /* 23.7 Range access */
    template<class C>
    constexpr auto begin(C& c) -> decltype(c.begin()) {
        return c.begin();
    }

    template<class C>
    constexpr auto begin(const C& c) -> decltype(c.begin()) {
        return c.begin();
    }

    template<class T, std::size_t N>
    constexpr T* begin(T (&array)[N]) noexcept {
        return array;
    }

    template<class C>
    constexpr auto end(C& c) -> decltype(c.end()) {
        return c.end();
    }

    template<class C>
    constexpr auto end(const C& c) -> decltype(c.end()) {
        return c.end();
    }

    template<class T, std::size_t N>
    constexpr T* end(T (&array)[N]) noexcept {
        return array + N;
    }

    template<class C>
    constexpr auto cbegin(const C& c) noexcept(noexcept(std::begin(c))) -> decltype(std::begin(c)) {
        return std::begin(c);
    }

    template<class C>
    constexpr auto cend(const C& c) noexcept(noexcept(std::end(c))) -> decltype(std::end(c)) {
        return std::end(c);
    }

    template<class C>
    constexpr auto rbegin(C& c) -> decltype(c.rbegin()) {
        return c.rbegin();
    }

    template<class C>
    constexpr auto rbegin(const C& c) -> decltype(c.rbegin()) {
        return c.rbegin();
    }

    template<class T, std::size_t N>
    constexpr reverse_iterator<T*> rbegin(T (&array)[N]) {
        return reverse_iterator<T*>(array + N);
    }

    template<class E>
    constexpr reverse_iterator<const E*> rbegin(initializer_list<E> il) {
        return reverse_iterator<const E*>(il.end());
    }

    template<class C>
    constexpr auto rend(C& c) -> decltype(c.rend()) {
        return c.rend();
    }

    template<class C>
    constexpr auto rend(const C& c) -> decltype(c.rend()) {
        return c.rend();
    }

    template<class T, std::size_t N>
    constexpr reverse_iterator<T*> rend(T (&array)[N]) {
        return reverse_iterator<T*>(array);
    }

    template<class E>
    constexpr reverse_iterator<const E*> rend(initializer_list<E> il) {
        return reverse_iterator<const E*>(il.begin());
    }

    template<class C>
    constexpr auto crbegin(const C& c) -> decltype(std::rbegin(c)) {
        return std::rbegin(c);
    }

    template<class C>
    constexpr auto crend(const C& c) -> decltype(std::rend(c)) {
        return std::rend(c);
    }

    template<class C>
    constexpr auto size(const C& c) -> decltype(c.size()) {
        return c.size();
    }

    template<class T, std::size_t N>
    constexpr std::size_t size(const T (&)[N]) noexcept {
        return N;
    }

    template<class C>
    constexpr auto ssize(const C& c) -> common_type_t<std::ptrdiff_t, make_signed_t<decltype(c.size())>> {
        return static_cast<common_type_t<std::ptrdiff_t, make_signed_t<decltype(c.size())>>>(c.size());
    }

    template<class T, std::ptrdiff_t N>
    constexpr std::ptrdiff_t ssize(const T (&)[N]) noexcept {
        return N;
    }

    template<class C>
    [[nodiscard]] constexpr auto empty(const C& c) -> decltype(c.empty()) {
        return c.empty();
    }

    template<class T, std::size_t N>
    [[nodiscard]] constexpr bool empty(const T (&)[N]) noexcept {
        return false;
    }

    template<class E>
    [[nodiscard]] constexpr bool empty(initializer_list<E> il) noexcept {
        return il.size() == 0;
    }

    template<class C>
    constexpr auto data(C& c) -> decltype(c.data()) {
        return c.data();
    }

    template<class C>
    constexpr auto data(const C& c) -> decltype(c.data()) {
        return c.data();
    }

    template<class T, std::size_t N>
    constexpr T* data(T (&array)[N]) noexcept {
        return array;
    }

    template<class E>
    constexpr const E* data(initializer_list<E> il) noexcept {
        return il.begin();
    }

    /* Implementation */
    namespace ranges::__iter_swap_internal {
        template<class X, class Y>
        constexpr bool iter_swap_fn::use_ranges_swap() noexcept {
            return !use_adl_iter_swap<X, Y>() && indirectly_readable<X>
                && indirectly_readable<Y> && swappable_with<X&, Y&>;
        }

        template<class X, class Y>
        constexpr bool iter_swap_fn::use_iter_exchange_move() noexcept {
            return !use_ranges_swap<X, Y>() && indirectly_movable_storable<X, Y>
                && indirectly_movable_storable<Y, X>;
        }
    }
}