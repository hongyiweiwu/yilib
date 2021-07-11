#pragma once

#include "cstddef.hpp"
#include "iosfwd.hpp"

namespace std {
    template<class charT> struct char_traits {};
    template<> struct char_traits<char> {
        using char_type = char;
        using int_type = int;
        using pos_type = streampos;
        using off_type = streamoff;

        static constexpr int_type eof() noexcept;
    };

    template<class CharT, class = void, class = void> class basic_string {
    public:
        using size_type = size_t;

        static constexpr size_type npos = -1;

        constexpr basic_string(const CharT*) {}

        constexpr const CharT* c_str() const noexcept {
            return nullptr;
        }

        constexpr size_type length() const noexcept {
            return 0;
        }
    };

    template<class charT, class traits, class Allocator>
    constexpr bool operator==(const basic_string<charT, traits, Allocator>&, const basic_string<charT, traits, Allocator>&) {
        return true;
    }

    template<class charT, class traits, class Allocator>
    constexpr bool operator==(const basic_string<charT, traits, Allocator>&, const charT*) {
        return true;
    }

    using string = basic_string<char>;
    using wstring = basic_string<wchar_t>;
}