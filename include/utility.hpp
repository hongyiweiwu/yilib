#pragma once

#include "utility/typecast.hpp"
#include "utility/declval.hpp"

namespace std {
    /* 20.2.4 Forward/move */
    using __internal::forward;
    using __internal::move;

    /* 20.2.6 Declval */
    using __internal::declval;
}