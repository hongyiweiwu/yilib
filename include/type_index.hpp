#pragma once

#include "typeinfo.hpp"
#include "compare.hpp"
#include "cstddef.hpp"
#include "functional.hpp"

namespace std {
    class type_index {
    public:
        type_index(const type_info& rhs) noexcept;
        bool operator==(const type_index& rhs) const noexcept;
        bool operator<(const type_index& rhs) const noexcept;
        bool operator>(const type_index& rhs) const noexcept;
        bool operator<=(const type_index& rhs) const noexcept;
        bool operator>=(const type_index& rhs) const noexcept;
        strong_ordering operator<=>(const type_index& rhs) const noexcept;
        std::size_t hash_code() const noexcept;
        const char* name() const noexcept;

    private:
        const type_info* target;
    };

    template<>
    struct hash<type_index> : hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const type_index& index) const noexcept;
    };
}