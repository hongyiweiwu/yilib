#pragma once

#include "cstddef.hpp"
#include "type_traits.hpp"
#include <compare>

namespace yilib {
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

    /* 17.11.3 Class template common_comparison_category */
    template<class ...Ts> struct common_comparison_category {};
    template<class ...Ts> struct common_comparison_category<partial_ordering, Ts...> {
        using returned_type = common_comparison_category<Ts...>::type;
        using type = conditional_t<is_void_v<returned_type>, void, partial_ordering>;
    };
    template<class ...Ts> struct common_comparison_category<weak_ordering, Ts...> { 
        using returned_type = common_comparison_category<Ts...>::type;
        using type = conditional_t<is_void_v<returned_type>, void, conditional_t<is_same_v<partial_ordering, returned_type>, partial_ordering, weak_ordering>>;
    };
    template<class ...Ts> struct common_comparison_category<strong_ordering, Ts...> {
        using type = common_comparison_category<Ts...>::type;
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
    }
}