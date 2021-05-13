#pragma once

#include <signal.h>

namespace std {
    using sig_atomic_t = ::sig_atomic_t;
    using ::signal;
    using ::raise;
}