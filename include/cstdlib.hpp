#pragma once

#include <stdlib.h>

namespace yilib {
    using ::size_t;
    using ::div_t;
    using ::ldiv_t;
    using ::lldiv_t;

    /* 17.5 Startup and termination */
    [[noreturn]] void abort() noexcept;
    int atexit(void (*func)()) noexcept;

#ifndef __APPLE_CC__
    // TODO: Mac's default C library doesn't have at_quick_exit.
    inline int at_quick_exit(void (*func)()) noexcept {
        return ::at_quick_exit(func);
    }
#endif

    [[noreturn]] void exit(int status);
    [[noreturn]] void _Exit(int status) noexcept;
    
#ifndef __APPLE_CC__
    // TODO: Mac's default C library doesn't have quick_exit.
    [[noreturn]] inline void quick_exit(int status) noexcept {
        ::quick_exit(status);
    }
#endif

    /* 20.10.12 C library memory allocation */
    using ::aligned_alloc;
    using ::calloc;
    using ::free;
    using ::malloc;
    using ::realloc;

    using ::atof;
    using ::atoi;
    using ::atol;
    using ::atoll;
    using ::strtod;
    using ::strtof;
    using ::strtold;
    using ::strtol;
    using ::strtoll;
    using ::strtoul;
    using ::strtoull;

    /* 21.5.6 Multibyte/wide string and character conversion functions */
    using ::mblen;
    using ::mbtowc;
    using ::wctomb;
    using ::mbstowcs;
    using ::wcstombs;

    /* 25.12 C standard library algorithms */
    using ::bsearch;
    using ::qsort;

    /* 26.6.10 Low-quality random number generation */
    using ::rand;
    using ::srand;

    /* 26.8.2 Absolute values */
    using ::abs;
    using ::labs;
    using ::llabs;

    using ::div;
    using ::ldiv;
    using ::lldiv;
}