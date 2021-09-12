#include "variant.hpp"
#include "compare.hpp"

namespace std {
    const char* bad_variant_access::what() const noexcept {
        return "std::bad_variant_access";
    }

    constexpr bool operator==(monostate, monostate) noexcept {
        return true;
    }

    constexpr strong_ordering operator<=>(monostate, monostate) noexcept {
        return strong_ordering::equal;
    }

    std::size_t hash<monostate>::operator()(const monostate&) const noexcept {
        return 0;
    }
}