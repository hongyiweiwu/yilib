#pragma once

#include "type_traits.hpp"

namespace yilib {
    template<class T> concept integral = is_integral_v<T>;
}