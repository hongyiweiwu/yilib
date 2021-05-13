#pragma once

#include "cstddef.hpp"
#include "type_traits.hpp"
#include "concepts.hpp"
#include "limits.hpp"

#include "utility/typecast.hpp"
#include "util/always_false.hpp"

namespace std {
    namespace __internal {
        enum class __ord  {
            equal = 0, equivalent = equal, less = -1, greater = 1
        };

        enum class __ncmp {
            unordered = -127
        };
    }

    /* 17.11.2.2 Class partial_ordering */
    class partial_ordering {
    private:
        int value;
        bool is_ordered;

        constexpr explicit partial_ordering(__internal::__ord v) noexcept : value(int(v)), is_ordered(true) {};
        constexpr explicit partial_ordering(__internal::__ncmp v) noexcept : value(int(v)), is_ordered(false) {};

    public:
        static const partial_ordering less;
        static const partial_ordering equivalent;
        static const partial_ordering greater;
        static const partial_ordering unordered;

        friend constexpr bool operator==(partial_ordering v, nullptr_t zero) noexcept;
        friend constexpr bool operator==(partial_ordering v, partial_ordering w) noexcept = default;
        friend constexpr bool operator<(partial_ordering v, nullptr_t zero) noexcept;
        friend constexpr bool operator>(partial_ordering v, nullptr_t zero) noexcept;
        friend constexpr bool operator<=(partial_ordering v, nullptr_t zero) noexcept;
        friend constexpr bool operator>=(partial_ordering v, nullptr_t zero) noexcept;
        friend constexpr bool operator<(nullptr_t zero, partial_ordering v) noexcept;
        friend constexpr bool operator>(nullptr_t zero, partial_ordering v) noexcept;
        friend constexpr bool operator<=(nullptr_t zero, partial_ordering v) noexcept;
        friend constexpr bool operator>=(nullptr_t zero, partial_ordering v) noexcept;
        friend constexpr partial_ordering operator<=>(partial_ordering v, nullptr_t zero) noexcept;
        friend constexpr partial_ordering operator<=>(nullptr_t zero, partial_ordering v) noexcept;
    };

    inline constexpr partial_ordering partial_ordering::less(__internal::__ord::less);
    inline constexpr partial_ordering partial_ordering::equivalent(__internal::__ord::equivalent);
    inline constexpr partial_ordering partial_ordering::greater(__internal::__ord::greater);
    inline constexpr partial_ordering partial_ordering::unordered(__internal::__ncmp::unordered);

    constexpr bool operator==(partial_ordering v, nullptr_t zero) noexcept {
        return v.is_ordered && v.value == 0;
    }

    constexpr bool operator<(partial_ordering v, nullptr_t zero) noexcept {
        return v.is_ordered && v.value < 0;
    }

    constexpr bool operator>(partial_ordering v, nullptr_t zero) noexcept {
        return v.is_ordered && v.value > 0;
    }

    constexpr bool operator<=(partial_ordering v, nullptr_t zero) noexcept {
        return v.is_ordered && v.value <= 0;
    }

    constexpr bool operator>=(partial_ordering v, nullptr_t zero) noexcept {
        return v.is_ordered && v.value >= 0;
    }

    constexpr bool operator<(nullptr_t zero, partial_ordering v) noexcept {
        return v.is_ordered && 0 < v.value;
    }

    constexpr bool operator>(nullptr_t zero, partial_ordering v) noexcept {
        return v.is_ordered && 0 > v.value;
    }

    constexpr bool operator<=(nullptr_t zero, partial_ordering v) noexcept {
        return v.is_ordered && 0 <= v.value;
    }

    constexpr bool operator>=(nullptr_t zero, partial_ordering v) noexcept {
        return v.is_ordered && 0 >= v.value;
    }

    constexpr partial_ordering operator<=>(partial_ordering v, nullptr_t zero) noexcept {
        return v;
    }

    constexpr partial_ordering operator<=>(nullptr_t zero, partial_ordering v) noexcept {
        return v < 0 ? partial_ordering::greater : v > 0 ? partial_ordering::less : v;
    }

    /* 17.11.2.3 Class weak_ordering */
    class weak_ordering {
    private:
        int value;

        constexpr explicit weak_ordering(__internal::__ord v) : value(int(v)) {}

    public:
        static const weak_ordering less;
        static const weak_ordering equivalent;
        static const weak_ordering greater;

        constexpr operator partial_ordering() const noexcept {
            return value == 0 ? partial_ordering::equivalent
                : value < 0 ? partial_ordering::less
                : partial_ordering::greater;
        }

        friend constexpr bool operator==(weak_ordering v, nullptr_t zero) noexcept;
        friend constexpr bool operator==(weak_ordering v, weak_ordering w) noexcept = default;
        friend constexpr bool operator<(weak_ordering v, nullptr_t zero) noexcept;
        friend constexpr bool operator>(weak_ordering v, nullptr_t zero) noexcept;
        friend constexpr bool operator<=(weak_ordering v, nullptr_t zero) noexcept;
        friend constexpr bool operator>=(weak_ordering v, nullptr_t zero) noexcept;
        friend constexpr bool operator<(nullptr_t zero, weak_ordering v) noexcept;
        friend constexpr bool operator>(nullptr_t zero, weak_ordering v) noexcept;
        friend constexpr bool operator<=(nullptr_t zero, weak_ordering v) noexcept;
        friend constexpr bool operator>=(nullptr_t zero, weak_ordering v) noexcept;
        friend constexpr weak_ordering operator<=>(weak_ordering v, nullptr_t zero) noexcept;
        friend constexpr weak_ordering operator<=>(nullptr_t zero, weak_ordering v) noexcept;
    };

    inline constexpr weak_ordering weak_ordering::less(__internal::__ord::less);
    inline constexpr weak_ordering weak_ordering::equivalent(__internal::__ord::equivalent);
    inline constexpr weak_ordering weak_ordering::greater(__internal::__ord::greater);

    constexpr bool operator==(weak_ordering v, nullptr_t zero) noexcept {
        return v.value == 0;
    }

    constexpr bool operator<(weak_ordering v, nullptr_t zero) noexcept {
        return v.value < 0;
    }

    constexpr bool operator>(weak_ordering v, nullptr_t zero) noexcept {
        return v.value > 0;
    }

    constexpr bool operator<=(weak_ordering v, nullptr_t zero) noexcept {
        return v.value <= 0;
    }

    constexpr bool operator>=(weak_ordering v, nullptr_t zero) noexcept {
        return v.value >= 0;
    }

    constexpr bool operator<(nullptr_t zero, weak_ordering v) noexcept {
        return 0 < v.value;
    }

    constexpr bool operator>(nullptr_t zero, weak_ordering v) noexcept {
        return 0 > v.value;
    }

    constexpr bool operator<=(nullptr_t zero, weak_ordering v) noexcept {
        return 0 <= v.value;
    }

    constexpr bool operator>=(nullptr_t zero, weak_ordering v) noexcept {
        return 0 >= v.value;
    }

    constexpr weak_ordering operator<=>(weak_ordering v, nullptr_t zero) noexcept {
        return v;
    }

    constexpr weak_ordering operator<=>(nullptr_t zero, weak_ordering v) noexcept {
        return v < 0 ? weak_ordering::greater : v > 0 ? weak_ordering::less : v;
    }

    class strong_ordering {
    private:
        int value;

        constexpr explicit strong_ordering(__internal::__ord v) noexcept : value(int(v)) {}
    public:
        static const strong_ordering less;
        static const strong_ordering equal;
        static const strong_ordering equivalent;
        static const strong_ordering greater;

        constexpr operator partial_ordering() const noexcept {
            return value == 0 ? partial_ordering::equivalent
                : value < 0 ? partial_ordering::less
                : partial_ordering::greater;
        }
        constexpr operator weak_ordering() const noexcept {
            return value == 0 ? weak_ordering::equivalent
                : value < 0 ? weak_ordering::less
                : weak_ordering::greater;
        }

        friend constexpr bool operator==(strong_ordering v, nullptr_t zero) noexcept;
        friend constexpr bool operator==(strong_ordering v, strong_ordering w) noexcept = default;
        friend constexpr bool operator<(strong_ordering v, nullptr_t zero) noexcept;
        friend constexpr bool operator>(strong_ordering v, nullptr_t zero) noexcept;
        friend constexpr bool operator<=(strong_ordering v, nullptr_t zero) noexcept;
        friend constexpr bool operator>=(strong_ordering v, nullptr_t zero) noexcept;
        friend constexpr bool operator<(nullptr_t zero, strong_ordering v) noexcept;
        friend constexpr bool operator>(nullptr_t zero, strong_ordering v) noexcept;
        friend constexpr bool operator<=(nullptr_t zero, strong_ordering v) noexcept;
        friend constexpr bool operator>=(nullptr_t zero, strong_ordering v) noexcept;
        friend constexpr strong_ordering operator<=>(strong_ordering v, nullptr_t zero) noexcept;
        friend constexpr strong_ordering operator<=>(nullptr_t zero, strong_ordering v) noexcept;
    };

    inline constexpr strong_ordering strong_ordering::less(__internal::__ord::less);
    inline constexpr strong_ordering strong_ordering::equal(__internal::__ord::equal);
    inline constexpr strong_ordering strong_ordering::equivalent(__internal::__ord::equivalent);
    inline constexpr strong_ordering strong_ordering::greater(__internal::__ord::greater);

    constexpr bool operator==(strong_ordering v, nullptr_t zero) noexcept {
        return v.value == 0;
    }

    constexpr bool operator<(strong_ordering v, nullptr_t zero) noexcept {
        return v.value < 0;
    }

    constexpr bool operator>(strong_ordering v, nullptr_t zero) noexcept {
        return v.value > 0;
    }

    constexpr bool operator<=(strong_ordering v, nullptr_t zero) noexcept {
        return v.value <= 0;
    }

    constexpr bool operator>=(strong_ordering v, nullptr_t zero) noexcept {
        return v.value >= 0;
    }

    constexpr bool operator<(nullptr_t zero, strong_ordering v) noexcept {
        return 0 < v.value;
    }

    constexpr bool operator>(nullptr_t zero, strong_ordering v) noexcept {
        return 0 > v.value;
    }

    constexpr bool operator<=(nullptr_t zero, strong_ordering v) noexcept {
        return 0 <= v.value;
    }

    constexpr bool operator>=(nullptr_t zero, strong_ordering v) noexcept {
        return 0 >= v.value;
    }

    constexpr strong_ordering operator<=>(strong_ordering v, nullptr_t zero) noexcept {
        return v;
    }

    constexpr strong_ordering operator<=>(nullptr_t zero, strong_ordering v) noexcept {
        return v < 0 ? strong_ordering::greater : v > 0 ? strong_ordering::less : v;
    }

    constexpr bool is_eq(partial_ordering cmp) noexcept { return cmp == 0; }
    constexpr bool is_neq(partial_ordering cmp) noexcept { return cmp != 0; }
    constexpr bool is_lt(partial_ordering cmp) noexcept { return cmp < 0; }
    constexpr bool is_lteq(partial_ordering cmp) noexcept { return cmp <= 0; }
    constexpr bool is_gt(partial_ordering cmp) noexcept { return cmp > 0; }
    constexpr bool is_gteq(partial_ordering cmp) noexcept { return cmp >= 0; }

    /* 17.11.3 Class template common_comparison_category */
    template<class ...Ts> struct common_comparison_category {};
    template<class ...Ts> struct common_comparison_category<partial_ordering, Ts...> {
        using returned_type = typename common_comparison_category<Ts...>::type;
        using type = conditional_t<is_void_v<returned_type>, void, partial_ordering>;
    };
    template<class ...Ts> struct common_comparison_category<weak_ordering, Ts...> { 
        using returned_type = typename common_comparison_category<Ts...>::type;
        using type = conditional_t<is_void_v<returned_type>, void, conditional_t<is_same_v<partial_ordering, returned_type>, partial_ordering, weak_ordering>>;
    };
    template<class ...Ts> struct common_comparison_category<strong_ordering, Ts...> {
        using type = typename common_comparison_category<Ts...>::type;
    };
    template<class T, class ...Ts> struct common_comparison_category<T, Ts...> {
        using type = void;
    };
    template<> struct common_comparison_category<> {
        using type = strong_ordering;
    };

    template<class ...Ts> using common_comparison_category_t = typename common_comparison_category<Ts...>::type;

    /* 17.11.4 Concept three-way_comparable */
    namespace __internal {
        template<class T, class Cat> concept compares_as = same_as<common_comparison_category_t<T, Cat>, Cat>;
    }

    template<class T, class Cat = partial_ordering> concept three_way_comparable = 
        __internal::weakly_equality_comparable_with<T, T>
        && __internal::partially_ordered_with<T, T>
        && requires (const remove_reference_t<T>& t1, const remove_reference_t<T>& t2) {
            { t1 <=> t2 } -> __internal::compares_as<Cat>;
        };

    template<class T, class U, class Cat = partial_ordering> concept three_way_comparable_with =
        three_way_comparable<T, Cat>
        && three_way_comparable<U, Cat>
        && common_reference_with<const remove_reference_t<T>&, const remove_reference_t<U>&>
        && three_way_comparable<common_reference_t<const remove_reference_t<T>&, const remove_reference_t<U>&>, Cat>
        && __internal::weakly_equality_comparable_with<T, U>
        && __internal::partially_ordered_with<T, U>
        && requires (const remove_reference_t<T>& t, const remove_reference_t<U>& u) {
            { t <=> u } -> __internal::compares_as<Cat>;
            { u <=> t } -> __internal::compares_as<Cat>;
        };

    template<class T, class U, class __Void = void> requires is_void_v<__Void> struct __compare_three_way_result {};
    template<class T, class U> struct __compare_three_way_result<T, U, void_t<decltype(declval<const remove_reference_t<T>&>() <=> declval<const remove_reference_t<U>&>())>> {
        using type = decltype(declval<const remove_reference_t<T>&>() <=> declval<const remove_reference_t<U>&>());
    };
    template<class T, class U = T> struct compare_three_way_result : __compare_three_way_result<T, U> {};
    template<class T, class U = T> using compare_three_way_result_t = typename compare_three_way_result<T, U>::type;

    namespace __internal {
        // 20.14.8.8 Models T and U if <=> in the expression "declval<T>() <=> declval<U>()"  resolvevs to a built-in operator comparing pointers.
        template<class T, class U> concept __builtin_ptr_three_way = 
            convertible_to<T, const volatile void*> && convertible_to<T, const volatile void*>
            && requires (T&& t, U&& u) { static_cast<const volatile void*>(forward<T>(t)) <=> static_cast<const volatile void *>(forward<U>(u)); }
            // User-defined operators can only be called through function notations. We use this check to make sure no <=> operators have been defined by the user
            // for these two types.
            && !requires (T&& t, U&& u) { operator<=>(forward<T>(t), forward<U>(u)); }
            && !requires (T&& t, U&& u) { forward<T>(t).operator<=>(forward<U>(u)); };
    }

    struct compare_three_way {
        template<class T, class U> requires three_way_comparable_with<T, U> || __internal::__builtin_ptr_three_way<T, U>
        constexpr auto operator()(T&& t, U&& u) const noexcept(noexcept(__internal::forward<T>(t) <=> __internal::forward<U>(u))) {
            if constexpr (__internal::__builtin_ptr_three_way<T, U>) {
                return static_cast<const volatile void*>(t) <=> static_cast<const volatile void*>(u);
            } else {
                return __internal::forward<T>(t) <=> __internal::forward<U>(u);
            }
        }

        using is_transparent = void;
    };

    namespace __strong_order {
        // Brings forward, declval, and always_false from std::__internal into this namespace to make things clearer.
        using ::std::__internal::forward;
        using ::std::__internal::declval;
        using ::std::__internal::always_false;
        
        // All xxx_order structs are defined in a context where their actual std-qualified versions are deleted.
        template<class E, class F> constexpr strong_ordering strong_order(E&&, F&&) = delete;

        struct __strong_order {
            template<class E, class F> static consteval bool __noexcept() {
                if constexpr (is_same_v<decay_t<E>, decay_t<F>>) {
                    if constexpr (requires (E&& e, F&& f) { strong_ordering(strong_order(forward<E>(e), forward<F>(f))); }) {
                        return noexcept(strong_order(declval<E>(), declval<F>()));
                    } else if constexpr (is_floating_point_v<decay_t<E>>) {
                        // TODO: Implement.
                        return true;
                    } else if constexpr (requires (E&& e, F&& f) { strong_ordering(compare_three_way()(forward<E>(), forward<F>())); }) {
                        return noexcept(compare_three_way()(declval<E>(), declval<F>()));
                    }
                }

                return true;
            }

            template<class E, class F>
            constexpr strong_ordering operator()(E&& e, F&& f) const noexcept(__noexcept<E, F>()) {
                if constexpr (is_same_v<decay_t<E>, decay_t<F>>) {
                    if constexpr (requires (E&& e, F&& f) { strong_ordering(strong_order(forward<E>(e), forward<F>(f))); }) {
                        return strong_order(forward<E>(e), forward<F>(f));
                    } else if constexpr (is_floating_point_v<decay_t<E>>) {
                        // TODO: Implement.
                        return strong_ordering::less;
                    } else if constexpr (requires (E&& e, F&& f) { strong_ordering(compare_three_way()(forward<E>(), forward<F>())); }) {
                        return compare_three_way()(forward<E>(e), forward<F>(f));
                    }
                }

                static_assert(always_false<E>, "Deliberately triggering substitution failure.");
            }
        };
    }

    inline namespace __inline_order {
        inline constexpr __strong_order::__strong_order strong_order{};
    }

    namespace __weak_order {
        // Brings forward, declval, and always_false from std::__internal into this namespace to make things clearer.
        using ::std::__internal::forward;
        using ::std::__internal::declval;
        using ::std::__internal::always_false;

        // All xxx_order structs are defined in a context where their actual std-qualified versions are deleted.
        template<class E, class F> constexpr weak_ordering weak_order(E&&, F&&) = delete;

        struct __weak_order {
            template<class E, class F> static consteval bool __noexcept() {
                if constexpr (is_same_v<decay_t<E>, decay_t<F>>) {
                    if constexpr (requires (E&& e, F&& f) { weak_ordering(weak_order(forward<E>(e), forward<F>(f))); }) {
                        return noexcept(weak_order(declval<E>(), declval<F>()));
                    } else if constexpr (is_floating_point_v<decay_t<E>>) {
                        // TODO: Implement.
                        return true;
                    } else if constexpr (requires (E&& e, F&& f) { weak_ordering(compare_three_way()(forward<E>(e), forward<F>(f))); }) {
                        return noexcept(compare_three_way()(declval<E>(), declval<F>()));
                    } else if constexpr (requires (E&& e, F&& f) { strong_order(forward<E>(e), forward<F>(f)); }) {
                        return noexcept(strong_order(declval<E>(), declval<F>()));
                    }
                }

                return true;
            }

            template<class E, class F>
            constexpr weak_ordering operator()(E&& e, F&& f) const noexcept(__noexcept<E, F>()) {
                if constexpr (is_same_v<decay_t<E>, decay_t<F>>) {
                    if constexpr (requires (E&& e, F&& f) { weak_ordering(weak_order(forward<E>(e), forward<F>(f))); }) {
                        return weak_order(forward<E>(e), forward<F>(f));
                    } else if constexpr (is_floating_point_v<decay_t<E>>) {
                        // TODO: Implement.
                        return weak_ordering::less;
                    } else if constexpr (requires (E&& e, F&& f) { weak_ordering(compare_three_way()(forward<E>(e), forward<F>(f))); }) {
                        return compare_three_way()(forward<E>(e), forward<F>(f));
                    } else if constexpr (requires (E&& e, F&& f) { strong_order(forward<E>(e), forward<F>(f)); }) {
                        return strong_order(forward<E>(e), forward<F>(f));
                    }
                }

                static_assert(always_false<E>, "Deliberately triggering substitution failure.");
            }
        };
    }

    inline namespace __inline_order {
        inline constexpr __weak_order::__weak_order weak_order{};
    }

    namespace __partial_order {
        // Brings forward, declval, and always_false from std::__internal into this namespace to make things clearer.
        using ::std::__internal::forward;
        using ::std::__internal::declval;
        using ::std::__internal::always_false;

        // All xxx_order structs are defined in a context where their actual std-qualified versions are deleted.
        template<class E, class F> constexpr partial_ordering partial_order(E&&, F&&) = delete;

        struct __partial_order {
            template<class E, class F> static consteval bool __noexcept() {
                if constexpr (is_same_v<decay_t<E>, decay_t<F>>) {
                    if constexpr (requires (E&& e, F&& f) { partial_ordering(partial_order(forward<E>(e), forward<F>(f))); }) {
                        return noexcept(partial_order(declval<E>(), declval<F>()));
                    } else if constexpr (requires (E&& e, F&& f) { partial_ordering(compare_three_way()(forward<E>(e), forward<F>(f))); }) {
                        return noexcept(compare_three_way()(declval<E>(), declval<F>()));
                    } else if constexpr (requires (E&& e, F&& f) { weak_order(forward<E>(e), forward<F>(f)); }) {
                        return noexcept(weak_order(declval<E>(), declval<F>()));
                    }
                }

                return true;
            }

            template<class E, class F>
            constexpr partial_ordering operator()(E&& e, F&& f) const noexcept(__noexcept<E, F>()) {
                if constexpr (is_same_v<decay_t<E>, decay_t<F>>) {
                    if constexpr (requires (E&& e, F&& f) { partial_ordering(partial_order(forward<E>(e), forward<F>(f))); }) {
                        return partial_order(forward<E>(e), forward<F>(f));
                    } else if constexpr (requires (E&& e, F&& f) { partial_ordering(compare_three_way()(forward<E>(e), forward<F>(f))); }) {
                        return compare_three_way()(forward<E>(e), forward<F>(f));
                    } else if constexpr (requires (E&& e, F&& f) { weak_order(forward<E>(e), forward<F>(f)); }) {
                        return weak_order(forward<E>(e), forward<F>(f));
                    }
                }

                static_assert(always_false<E>, "Deliberately triggering substitution failure.");
            }
        };
    }

    inline namespace __inline_order {
        inline constexpr __partial_order::__partial_order partial_order{};
    }

    namespace __internal {
        struct compare_strong_order_fallback {
            template<class E, class F>
            requires same_as<decay_t<E>, decay_t<F>> && requires (E&& e, F&& f) { strong_order(forward<E>(e), forward<F>(f)); }
            constexpr strong_ordering operator()(E&& e, F&& f) const noexcept(noexcept(strong_order(forward<E>(e), forward<F>(f)))) {
                return strong_order(forward<E>(e), forward<F>(f));
            }

            template<class E, class F>
            requires same_as<decay_t<E>, decay_t<F>> 
                && (!requires (E&& e, F&& f) { strong_order(forward<E>(e), forward<F>(f)); })
                && requires (E&& e, F&& f) {
                    { e == f } -> convertible_to<bool>;
                    { e < f } -> convertible_to<bool>;
            } constexpr strong_ordering operator()(E&& e, F&& f) const 
                noexcept(noexcept(e == f ? strong_ordering::equivalent
                                : e < f ? strong_ordering::less
                                : strong_ordering::greater)) {
                return e == f ? strong_ordering::equivalent
                     : e < f ? strong_ordering::less
                     : strong_ordering::greater;
            }
        };

        struct compare_weak_order_fallback {
            template<class E, class F>
            requires same_as<decay_t<E>, decay_t<F>> && requires (E&& e, F&& f) { weak_order(forward<E>(e), forward<F>(f)); }
            constexpr weak_ordering operator()(E&& e, F&& f) const noexcept(noexcept(weak_order(forward<E>(e), forward<F>(f)))) {
                return weak_order(forward<E>(e), forward<F>(f));
            }

            template<class E, class F>
            requires same_as<decay_t<E>, decay_t<F>> 
                && (!requires (E&& e, F&& f) { weak_order(forward<E>(e), forward<F>(f)); })
                && requires (E&& e, F&& f) {
                    { e == f } -> convertible_to<bool>;
                    { e < f } -> convertible_to<bool>;
            } constexpr weak_ordering operator()(E&& e, F&& f) const 
                noexcept(noexcept(e == f ? weak_ordering::equivalent
                                : e < f ? weak_ordering::less
                                : weak_ordering::greater)) {
                return e == f ? weak_ordering::equivalent
                     : e < f ? weak_ordering::less
                     : weak_ordering::greater;
            }
        };

        struct compare_partial_order_fallback {
            template<class E, class F>
            requires same_as<decay_t<E>, decay_t<F>> && requires (E&& e, F&& f) { partial_order(forward<E>(e), forward<F>(f)); }
            constexpr partial_ordering operator()(E&& e, F&& f) const noexcept(noexcept(partial_order(forward<E>(e), forward<F>(f)))) {
                return partial_order(forward<E>(e), forward<F>(f));
            }

            template<class E, class F>
            requires same_as<decay_t<E>, decay_t<F>> 
                && (!requires (E&& e, F&& f) { partial_order(forward<E>(e), forward<F>(f)); })
                && requires (E&& e, F&& f) {
                    { e == f } -> convertible_to<bool>;
                    { e < f } -> convertible_to<bool>;
            } constexpr partial_ordering operator()(E&& e, F&& f) const 
                noexcept(noexcept(e == f ? partial_ordering::equivalent
                                : e < f ? partial_ordering::less
                                : f < e ? partial_ordering::greater
                                : partial_ordering::unordered)) {
                return e == f ? partial_ordering::equivalent
                    : e < f ? partial_ordering::less
                    : f < e ? partial_ordering::greater
                    : partial_ordering::unordered;
            }
        };
    }

    inline namespace __inline_order {
        inline constexpr __internal::compare_strong_order_fallback compare_strong_order_fallback{};
        inline constexpr __internal::compare_weak_order_fallback compare_weak_order_fallback{};
        inline constexpr __internal::compare_partial_order_fallback compare_partial_order_fallback{};
    }
}