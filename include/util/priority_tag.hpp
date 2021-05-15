#pragma once

namespace std::__internal {
    template<unsigned int N> struct priority_tag : priority_tag<N - 1> {};
    template<> struct priority_tag<0> {};
}