#include "optional.hpp"

namespace std {
    const char* bad_optional_access::what() const noexcept { return "std::bad_optional_access"; }
}