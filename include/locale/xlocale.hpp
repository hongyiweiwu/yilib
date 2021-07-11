#pragma once

// For posix-specific extended locale APIs.
#if defined(__APPLE__) || defined(_GNU_SOURCE)
#define YILIB_HAS_XLOCALE 1
#else
#define YILIB_HAS_XLOCALE 0
#endif

#if YILIB_HAS_XLOCALE && defined(__APPLE__)
#include "xlocale.h"
#endif