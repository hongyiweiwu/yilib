#include "mutex.hpp"
#include "system_error.hpp"

#include "pthread.h"

namespace std {
    mutex::~mutex() { pthread_mutex_destroy(&mut); }

    void mutex::lock() {
        const int ec = pthread_mutex_lock(&mut);
        if (ec) throw system_error(ec, system_category());
    }

    bool mutex::try_lock() noexcept { return !pthread_mutex_trylock(&mut); }
    void mutex::unlock() noexcept { pthread_mutex_unlock(&mut); }

    mutex::native_handle_type mutex::native_handle() { return &mut; }

    recursive_mutex::attr::attr(int type) : handle() {
        const int rc = pthread_mutexattr_init(&handle);
        if (rc) throw system_error(rc, system_category());
        pthread_mutexattr_settype(&handle, type);
    }

    recursive_mutex::attr::~attr() { pthread_mutexattr_destroy(&handle); }

    recursive_mutex::recursive_mutex() : mut() {
        const int rc = pthread_mutex_init(&mut, &mut_attr.handle);
        if (rc) throw system_error(rc, system_category());
    }

    recursive_mutex::~recursive_mutex() { pthread_mutex_destroy(&mut); }

    void recursive_mutex::lock() {
        const int ec = pthread_mutex_lock(&mut);
        if (ec) throw system_error(ec, system_category());
    }

    bool recursive_mutex::try_lock() noexcept { return !pthread_mutex_trylock(&mut); }
    void recursive_mutex::unlock() noexcept { pthread_mutex_unlock(&mut); }

    recursive_mutex::native_handle_type recursive_mutex::native_handle() { return &mut; }

    recursive_mutex::attr recursive_mutex::mut_attr{PTHREAD_MUTEX_RECURSIVE};
}