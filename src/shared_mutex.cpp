#include "shared_mutex.hpp"
#include "system_error.hpp"

#include "pthread.h"
#include <pthread.h>

namespace std {
    shared_mutex::~shared_mutex() { pthread_rwlock_destroy(&handle); }

    void shared_mutex::lock() { 
        const int ec = pthread_rwlock_wrlock(&handle);
        if (ec) throw system_error(ec, system_category());
    }

    bool shared_mutex::try_lock() noexcept { return !pthread_rwlock_trywrlock(&handle); }
    void shared_mutex::unlock() noexcept { pthread_rwlock_unlock(&handle); }

    void shared_mutex::lock_shared() {
        const int ec = pthread_rwlock_rdlock(&handle);
        if (ec) throw system_error(ec, system_category());
    }

    bool shared_mutex::try_lock_shared() noexcept { return !pthread_rwlock_tryrdlock(&handle); }

    void shared_mutex::unlock_shared() noexcept { unlock(); }

    shared_mutex::native_handle_type shared_mutex::native_handle() { return &handle; }
}