#pragma once

#include "cstddef.hpp"

namespace yilib {
    template<class E>
    class initializer_list {
    public:
        using value_type = E;
        using reference = const E&;
        using const_reference = const E&;
        using size_type = size_t;

        using iterator = const E*;
        using const_iterator = const E*;

    private:
        const size_type size_v;
        const const_iterator begin_v;

        constexpr initializer_list(const_iterator begin, size_type size) noexcept : begin_v(begin), size_v(size) {};
    
    public:
        /* 17.10.3 Initializer list constructors */
        constexpr initializer_list() noexcept : begin_v(nullptr), size_v(0) {};

        /* 17.10.4 Initializer list access */
        constexpr size_t size() const noexcept { return size_v; }
        constexpr const E* begin() const noexcept { return begin_v; }
        constexpr const E* end() const noexcept { return begin() + size(); }
    };

    /* 17.10.5 Initializer list range access */
    template<class E> constexpr const E* begin(initializer_list<E> il) noexcept { return il.begin(); }
    template<class E> constexpr const E* end(initializer_list<E> il) noexcept { return il.end(); }
}