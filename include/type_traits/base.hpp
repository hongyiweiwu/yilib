#pragma once

namespace yilib::__internal {
    template<class T, T v> struct integral_constant {
        static constexpr T value = v;
        using value_type = T;
        using type = integral_constant;
        constexpr operator value_type() const noexcept { return value; }
        constexpr value_type operator()() const noexcept { return value; }
    };

    template<bool B> using bool_constant = integral_constant<bool, B>;
    using true_type = bool_constant<true>;
    using false_type = bool_constant<false>;

    template<class ...> using void_t = void; 

    template<class T> struct type_identity { using type = T; };
}