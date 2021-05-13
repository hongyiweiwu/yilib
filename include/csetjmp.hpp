#pragma once

#include <setjmp.h>

namespace std {
    using jmp_buf = ::jmp_buf;
    using ::longjmp;
}