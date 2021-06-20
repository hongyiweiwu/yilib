#pragma once

#include "cstddef.hpp"

namespace std {
    template<class E>
    class initializer_list {
    private:
        const E* beg;
        size_t s;
        
        constexpr initializer_list(const E* begin, size_t size) noexcept : beg(begin), s(size) {}
    public:
        using value_type = E;
        using reference = const E&;
        using const_reference = const E&;
        using size_type = size_t;
        using iterator = const E*;
        using const_iterator = const E*;
        
        constexpr initializer_list() noexcept : beg(nullptr), s(0) {}
        constexpr size_t size() const noexcept { return s; }
        constexpr const E* begin() const noexcept { return beg; }
        constexpr const E* end() const noexcept { return begin() + size(); }
    };

    /* 17.10.5 Initializer list range access */
    template<class E> constexpr const E* begin(initializer_list<E> il) noexcept { return il.begin(); }
    template<class E> constexpr const E* end(initializer_list<E> il) noexcept { return il.end(); }
}