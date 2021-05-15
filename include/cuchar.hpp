#pragma once

// Macs don't come with uchar.h because Apple handles i18n differently.
#if !__APPLE__
#include "uchar.h"
#endif

namespace std {
#if !__APPLE__
    using mbstate_t = ::mbstate_t;
    using size_t = ::size_t;

    size_t mbrtoc8(char8_t* pc8, const char* s, size_t n, mbstate_t* ps);
    size_t c8rtomb(char* s, char8_t c8, mbstate_t* ps);

    using ::mbrtoc16;
    using ::c16rtomb;
    using ::mbrtoc32;
    using ::c32rtomb;
#endif
}