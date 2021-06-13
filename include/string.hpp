#pragma once

#include "cstddef.hpp"
#include "iosfwd.hpp"

namespace std {
    template<class charT> struct char_traits {};
    template<> struct char_traits<char> {
        using char_type = char;
        using int_type = int;
        using pos_type = streampos;
        using off_type = long long; // Should be streamoff, defined in "ios.hpp". It is defined there as long long.
    };

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