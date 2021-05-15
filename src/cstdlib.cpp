#include "cstdlib.hpp"

#include "stdlib.h"

namespace std {
    [[noreturn]] void abort() noexcept {
        ::abort();
    }

    int atexit(void (*func)()) noexcept {
        return ::atexit(func);
    }

    [[noreturn]] void exit(int status) {
        ::exit(status);
    }

    [[noreturn]] void _Exit(int status) noexcept {
        ::_Exit(status);
    }
}