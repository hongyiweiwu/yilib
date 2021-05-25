#include "type_index.hpp"

namespace std {
    type_index::type_index(const type_info& rhs) noexcept : target(&rhs) {}
    bool type_index::operator==(const type_index& rhs) const noexcept {
        return *target == *rhs.target;
    }

    bool type_index::operator<(const type_index& rhs) const noexcept {
        return target->before(*rhs.target);
    }

    bool type_index::operator>(const type_index& rhs) const noexcept {
        return rhs.target->before(*target);
    }

    bool type_index::operator<=(const type_index& rhs) const noexcept {
        return !rhs.target->before(*target);
    }

    bool type_index::operator>=(const type_index& rhs) const noexcept {
        return !target->before(*rhs.target);
    }

    strong_ordering type_index::operator<=>(const type_index& rhs) const noexcept {
        if (*target == *rhs.target) return strong_ordering::equal;
        else if (target->before(*rhs.target)) return strong_ordering::less;
        else return strong_ordering::greater;
    }

    size_t type_index::hash_code() const noexcept { return target->hash_code(); }
    const char* type_index::name() const noexcept { return target->name(); }

    size_t hash<type_index>::operator()(const type_index& index) { return index.hash_code(); }
}