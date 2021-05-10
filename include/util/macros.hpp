#pragma once

// TODO: This currently only supports Clang++. GCC and MSVC support is being investigated.
#define __has_intrinsics_for(test) __has_builtin(__##test) || !__is_identifier(__##test) || __has_feature(test)