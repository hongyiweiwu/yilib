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

#if defined(__APPLE__)
    timed_mutex::timed_mutex() : condvar(PTHREAD_COND_INITIALIZER), locked(false) {}

    timed_mutex::~timed_mutex() { pthread_cond_destroy(&condvar); }

    void timed_mutex::lock() {
        const unique_lock<mutex> lock(mtx);
        while (locked) {
            const int ec = pthread_cond_wait(&condvar, mtx.native_handle());
            if (ec != 0) throw system_error(ec, system_category());
        }

        locked = true;
    }

    bool timed_mutex::try_lock() {
        const unique_lock<mutex> lock(mtx);
        const bool acquired = !locked;
        locked = true;
        return acquired;
    }

    void timed_mutex::unlock() noexcept {
        mtx.lock();
        locked = false;
        mtx.unlock();
        pthread_cond_signal(&condvar);
    }
#else
    void timed_mutex::lock() { mutex::lock(); }
    bool timed_mutex::try_lock() { return mutex::try_lock(); }
    void timed_mutex::unlock() noexcept { mutex::unlock(); }
    timed_mutex::native_handle_type timed_mutex::native_handle() { return native_handle(); }
#endif

#if defined(__APPLE__)
    recursive_timed_mutex::recursive_timed_mutex() : condvar(PTHREAD_COND_INITIALIZER), locked(false) {}

    recursive_timed_mutex::~recursive_timed_mutex() { pthread_cond_destroy(&condvar); }

    void recursive_timed_mutex::lock() {
        const unique_lock<recursive_mutex> lock(mtx);
        while (locked) {
            const int ec = pthread_cond_wait(&condvar, mtx.native_handle());
            if (ec != 0) throw system_error(ec, system_category());
        }

        locked = true;
    }

    bool recursive_timed_mutex::try_lock() {
        const unique_lock<recursive_mutex> lock(mtx);
        const bool acquired = !locked;
        locked = true;
        return acquired;
    }

    void recursive_timed_mutex::unlock() noexcept {
        mtx.lock();
        locked = false;
        mtx.unlock();
        pthread_cond_signal(&condvar);
    }
#else
    void recursive_timed_mutex::lock() { recursive_mutex::lock(); }
    bool recursive_timed_mutex::try_lock() { return recursive_mutex::try_lock(); }
    void recursive_timed_mutex::unlock() noexcept { recursive_mutex::unlock(); }
    recursive_timed_mutex::native_handle_type recursive_timed_mutex::native_handle() { return native_handle(); }
#endif
}