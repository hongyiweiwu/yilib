#pragma once 

// NOTE: All implementations here are dummy ones.

namespace std::ranges {
    template<class T>
    using iterator_t = T;

    template<class T>
    inline constexpr bool enable_view = false;
    template<class T>
    inline constexpr bool enable_borrowed_range = false;
}