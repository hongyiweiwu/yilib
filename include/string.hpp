#pragma once

namespace std {
    template<class CharT> class basic_string {
    public:
        constexpr basic_string(const CharT*) {}

        constexpr const CharT* c_str() const noexcept {
            return "Unimplemeneted";
        }
    };

    using string = basic_string<char>;
}