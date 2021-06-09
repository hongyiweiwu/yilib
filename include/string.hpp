#pragma once

#include "cstddef.hpp"

namespace std {
    template<class charT> struct char_traits {};

    template<class CharT, class = void, class = void> class basic_string {
    public:
        using size_type = size_t;

        static const size_type npos = -1;

        constexpr basic_string(const CharT*) {}

        constexpr const CharT* c_str() const noexcept {
            return "Unimplemeneted";
        }
    };

    using string = basic_string<char>;
}