#pragma once

#include "setjmp.h"

namespace yilib {
    using jmp_buf = ::jmp_buf;
    using ::longjmp;
}