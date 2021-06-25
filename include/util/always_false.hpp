#pragma once

namespace std::__internal {
    template<class T> constexpr bool always_false = false;

    template<class ...T> struct type_list {};
}