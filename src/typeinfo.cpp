#include "typeinfo.hpp"

#include "cstddef.hpp"
#include "cstring.hpp"

namespace std {
    type_info::type_info(const char* name) : type_name(name) {}

    bool type_info::operator==(const type_info& rhs) const noexcept { 
        return name() == rhs.name();
    }

    bool type_info::before(const type_info& rhs) const noexcept {
        return strcmp(name(), rhs.name()) < 0;
    }

    size_t type_info::hash_code() const noexcept {
        return reinterpret_cast<size_t>(type_name);
    }

    const char* type_info::name() const noexcept {
        return type_name;
    }

    const char* bad_cast::what() const noexcept { return "std::bad_cast"; }
    const char* bad_typeid::what() const noexcept { return "std::bad_typeid"; }
}