#pragma once

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#define __has_intrinsics_for(test) __has_builtin(__##test)
#else
#define __has_intrinsics_for(test) 0
#endif