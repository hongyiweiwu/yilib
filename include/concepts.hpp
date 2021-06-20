#pragma once

#include "type_traits.hpp"
#include "cstddef.hpp"
#include "utility/declval.hpp"
#include "utility/typecast.hpp"

namespace std {
    /* 18.4 Language-related concepts */
    template<class T, class U> concept same_as = is_same_v<T, U> && is_same_v<U, T>;
    template<class Derived, class Base> concept derived_from = is_base_of_v<Base, Derived> && is_convertible_v<const volatile Derived*, const volatile Base*>;
    template<class From, class To> concept convertible_to =
        is_convertible_v<From, To> &&
        requires (add_rvalue_reference_t<From> (&f)()) {
            static_cast<To>(f());
        };
    template<class T, class U> concept common_reference_with =
        same_as<common_reference_t<T, U>, common_reference_t<U, T>> &&
        convertible_to<T, common_reference_t<T, U>> &&
        convertible_to<U, common_reference_t<T, U>>;
    template<class T, class U> concept common_with =
        same_as<common_type_t<T, U>, common_type_t<U, T>> &&
        requires {
            static_cast<common_type_t<T, U>>(__internal::declval<T>());
            static_cast<common_type_t<T, U>>(__internal::declval<U>());
        } &&
        common_reference_with<add_lvalue_reference_t<const T>, add_lvalue_reference_t<const U>> &&
        common_reference_with<
            add_lvalue_reference_t<common_type_t<T, U>>,
            common_reference_t<
                add_lvalue_reference_t<const T>,
                add_lvalue_reference_t<const U>>>;

    template<class T> concept integral = is_integral_v<T>;
    template<class T> concept signed_integral = integral<T> && is_signed_v<T>;
    template<class T> concept unsigned_integral = integral<T> && !signed_integral<T>;
    template<class T> concept floating_point = is_floating_point_v<T>;

    template<class LHS, class RHS> concept assignable_from =
        is_lvalue_reference_v<LHS> &&
        common_reference_with<const remove_reference_t<LHS>&, const remove_reference_t<RHS>&> &&
        requires(LHS lhs, RHS&& rhs) {
            { lhs = __internal::forward<RHS>(rhs) } -> same_as<LHS>;
        };

    namespace ranges {
        // We use a separate __internal namespace under ranges instead of the traditional std::__internal because we don't
        // want to pollute the latter with our custom deleted swap function.
        namespace __swap_internal {
            // Prevents any ranges::swap declarations in the ranges namespace from engaging in overload resolution.
            template<class T> void swap(T&, T&) = delete;

            struct __swap_fn;

            template<class T, class U>
            concept __std_swap_usable = (is_class_v<remove_cvref_t<T>> || is_enum_v<remove_cvref_t<T>>) && (is_class_v<remove_cvref_t<U>> || is_enum_v<remove_cvref_t<U>>)
                    && requires (T&& e1, U&& e2) { (void) swap(e1, e2); };

            template<class T, class U>
            concept __array_swappable = !__std_swap_usable<T, U> && is_array_v<T> && is_array_v<U> && (extent_v<T> == extent_v<U>)
                && requires (T&& e1, U&&e2, const __swap_fn& swap) { swap(*e1, *e2); };

            template<class T>
            concept __exchangeable = (!__std_swap_usable<T&, T&>) && (!__array_swappable<T&, T&>)
                && assignable_from<T&, T>
                // move_constructible<T, T> spelled out 
                && is_nothrow_destructible_v<T> && is_constructible_v<T, T> && convertible_to<T, T>;

            struct __swap_fn {
                template<class T, class U> requires __std_swap_usable<T, U>
                constexpr void operator()(T&& e1, U&& e2) const
                    noexcept(noexcept(swap(::std::__internal::forward<T>(e1), ::std::__internal::forward<U>(e2))))
                {
                    swap(::std::__internal::forward<T>(e1), ::std::__internal::forward<U>(e2));
                }

                template<class T, class U, size_t N>
                requires __array_swappable<T(&)[N], U(&)[N]>
                constexpr void operator()(T(&e1)[N], U(&e2)[N]) const noexcept(noexcept((*this)(*e1, *e2))) {
                    // ranges::swap_ranges spelled out
                    for (auto i = 0; i < N; i++) {
                        (*this)(e1[i], e2[i]);
                    }
                }

                template<class T>
                requires __exchangeable<T>
                constexpr void operator()(T& e1, T& e2) const noexcept(noexcept(T(::std::__internal::move(e1)), e1 = ::std::__internal::move(e2), e2 = ::std::__internal::move(::std::__internal::declval<T&>()))) {
                    T t(::std::__internal::move(e1));
                    e1 = ::std::__internal::move(e2);
                    e2 = ::std::__internal::move(t);
                }
            };
        }

        inline namespace __inline_swap {
            inline constexpr ::std::ranges::__swap_internal::__swap_fn swap;
        }
    }

    template<class T> concept swappable = requires (T& a, T& b) { ranges::swap(a, b); };

    template<class T, class U> concept swappable_with =
        common_reference_with<T, U> &&
        requires (T&& t, U&& u) {
            ranges::swap(::std::__internal::forward<T>(t), ::std::__internal::forward<T>(t));
            ranges::swap(::std::__internal::forward<U>(u), ::std::__internal::forward<U>(u));
            ranges::swap(::std::__internal::forward<T>(t), ::std::__internal::forward<U>(u));
            ranges::swap(::std::__internal::forward<U>(u), ::std::__internal::forward<T>(t));
        };

    template<class T> concept destructible = is_nothrow_destructible_v<T>;
    template<class T, class ...Args> concept constructible_from = destructible<T> && is_constructible_v<T, Args...>;
    template<class T> concept default_initializable = constructible_from<T> && requires { 
        T{};
        ::new (static_cast<void*>(nullptr)) T;
    };

    template<class T> concept move_constructible = constructible_from<T, T> && convertible_to<T, T>;
    template<class T> concept copy_constructible = move_constructible<T>
        && constructible_from<T, T&> && convertible_to<T&, T>
        && constructible_from<T, const T&> && convertible_to<const T&, T>
        && constructible_from<T, const T> && convertible_to<const T, T>;

    /* 18.5 Comparison concepts */
    namespace __internal {
        template<class T> concept boolean_testable = convertible_to<T, bool>
            && requires (T&& t) {
                { !forward<T>(t) } -> convertible_to<bool>;
            };

        template<class T, class U> concept weakly_equality_comparable_with = requires(const remove_reference_t<T>& t, const remove_reference_t<U>& u) {
            { t == u } -> boolean_testable;
            { t != u } -> boolean_testable;
            { u == t } -> boolean_testable;
            { u != t } -> boolean_testable;
        };
    }

    template<class T> concept equality_comparable = __internal::weakly_equality_comparable_with<T, T>;

    template<class T, class U> concept equality_comparable_with = 
        equality_comparable<T> && equality_comparable<U>
        && common_reference_with<const remove_reference_t<T>&, const remove_reference_t<U>&> 
        && equality_comparable<common_reference_t<const remove_reference_t<T>&, const remove_reference_t<U>&>>
        && __internal::weakly_equality_comparable_with<T, U>;

    namespace __internal {
        template<class T, class U> concept partially_ordered_with = requires (const remove_reference_t<T>& t, const remove_reference_t<U>& u) {
            { t < u } -> boolean_testable;
            { t > u } -> boolean_testable;
            { t <= u } -> boolean_testable;
            { t >= u } -> boolean_testable;
            { u < t } -> boolean_testable;
            { u > t } -> boolean_testable;
            { u <= t } -> boolean_testable;
            { u >= t } -> boolean_testable;
        };
    }

    template<class T> concept totally_ordered = equality_comparable<T> && __internal::partially_ordered_with<T, T>;
    template<class T, class U> concept totally_ordered_with = totally_ordered<T> && totally_ordered<U> 
        && equality_comparable_with<T, U>
        && totally_ordered<common_reference_t<const remove_reference_t<T>&, const remove_reference_t<U>&>>
        && __internal::partially_ordered_with<T, U>;

    /* 18.6 Object concepts */
    template<class T> concept movable = is_object_v<T> && move_constructible<T> && assignable_from<T&, T> && swappable<T>;
    template<class T> concept copyable = copy_constructible<T> && movable<T> && assignable_from<T&, T&> && assignable_from<T&, const T&> && assignable_from<T&, const T>;
    template<class T> concept semiregular = copyable<T> && default_initializable<T>;
    template<class T> concept regular = semiregular<T> && equality_comparable<T>;

    /* 18.7 Callable concepts */
    template<class F, class ...Args> concept invocable = requires (F&& f, Args&& ...args) {
        __internal::__INVOKE(forward<F>(f), forward<Args>(args)...);  
    };
    template<class F, class ...Args> concept regular_invocable = invocable<F, Args...>;
    template<class F, class ...Args> concept predicate = regular_invocable<F, Args...> && __internal::boolean_testable<invoke_result_t<F, Args...>>;
    template<class R, class T, class U> concept relation = predicate<R, T, T> && predicate<R, T, U> && predicate<R, U, U> && predicate<R, U, T>;
    template<class R, class T, class U> concept equivalence_relation = relation<R, T, U>;
    template<class R, class T, class U> concept strict_weak_order = relation<R, T, U>;
}