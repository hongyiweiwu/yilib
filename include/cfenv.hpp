#pragma once

#include "fenv.h"

namespace std {
    using ::fenv_t;
    using ::fexcept_t;

    using ::feclearexcept;
    using ::fegetexceptflag;
    using ::feraiseexcept;
    using ::fesetexceptflag;
    using ::fetestexcept;

    using ::fegetround;
    using ::fesetround;

    using ::fegetenv;
    using ::feholdexcept;
    using ::fesetenv;
    using ::feupdateenv;
}