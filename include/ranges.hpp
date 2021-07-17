#pragma once 

// NOTE: All implementations here are dummy ones.

namespace std::ranges {
    template<class T>
    using iterator_t = T;

    template<class T>
    using range_reference_t = T;

    template<class C>
    constexpr auto data(const C& c) -> decltype(c.data()) {
        return c.data();
    }

    template<class C>
    constexpr auto size(const C& c) -> decltype(c.size()) {
        return c.size();
    }

    template<class T>
    concept contiguous_range = true;

    template<class T>
    concept sized_range = true;

    template<class T>
    concept borrowed_range = true;

    template<class T>
    inline constexpr bool enable_view = false;
    template<class T>
    inline constexpr bool enable_borrowed_range = false;
}