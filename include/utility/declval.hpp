#pragma once

namespace std::__internal {
    template<class T>
    auto __declval(int) -> T&&;
    template<class T>
    auto __declval(...) -> T;
    template<class T>
    decltype(__declval<T>(0)) declval() noexcept;
}