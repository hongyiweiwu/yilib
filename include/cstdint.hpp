#pragma once

#include <stdint.h>

namespace yilib {
#ifdef INT8_MAX
    using ::int8_t;
#endif
#ifdef INT16_MAX
    using ::int16_t;
#endif
#ifdef INT32_MAX
    using ::int32_t;
#endif
#ifdef INT64_MAX
    using ::int64_t;
#endif

    using ::int_fast8_t;
    using ::int_fast16_t;
    using ::int_fast32_t;
    using ::int_fast64_t;

    using ::int_least8_t;
    using ::int_least16_t;
    using ::int_least32_t;
    using ::int_least64_t;

    using ::intmax_t;
#ifdef INTPTR_MAX
    using ::intptr_t;
#endif

#ifdef UINT8_MAX
    using ::uint8_t;
#endif
#ifdef UINT16_MAX
    using ::uint16_t;
#endif
#ifdef UINT32_MAX
    using ::uint32_t;
#endif
#ifdef UINT64_MAX
    using ::uint64_t;
#endif

    using ::uint_fast8_t;
    using ::uint_fast16_t;
    using ::uint_fast32_t;
    using ::uint_fast64_t;

    using ::uint_least8_t;
    using ::uint_least16_t;
    using ::uint_least32_t;
    using ::uint_least64_t;

    using ::uintmax_t;
#ifdef UINTPTR_MAX
    using ::uintptr_t;
#endif
}