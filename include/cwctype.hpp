#pragma once

#include "wctype.h"

namespace std {
    using wint_t = ::wint_t;
    using wctrans_t = ::wctrans_t;
    using wctype_t = ::wctype_t;

    using ::iswalnum;
    using ::iswalpha;
    using ::iswblank;
    using ::iswcntrl;
    using ::iswdigit;
    using ::iswgraph;
    using ::iswlower;
    using ::iswprint;
    using ::iswpunct;
    using ::iswspace;
    using ::iswupper;
    using ::iswxdigit;
    using ::iswctype;
    using ::wctype;
    using ::towlower;
    using ::towupper;
    using ::towctrans;
    using ::wctrans;
}