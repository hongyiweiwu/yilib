#pragma once

#include "iosfwd.hpp"

namespace std {
    template<class CharT, class Traits>
    class istreambuf_iterator {
    public:
        constexpr istreambuf_iterator() noexcept = default;
        CharT operator*() const { return 0; }
        istreambuf_iterator& operator++() { return *this; }
    };

    template<class charT, class traits>
    bool operator==(const istreambuf_iterator<charT, traits>&, const istreambuf_iterator<charT, traits>&) {
        return true;
    }
}