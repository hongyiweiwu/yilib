#include "shared_mutex.hpp"
#include "system_error.hpp"
#include "mutex.hpp"

#include "pthread.h"

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

#if defined(__APPLE__)
    shared_timed_mutex::shared_timed_mutex() 
        : no_writer_condvar(PTHREAD_COND_INITIALIZER), no_reader_condvar(PTHREAD_COND_INITIALIZER),
          writer_entered(false), reader_entered(0) {}

    shared_timed_mutex::~shared_timed_mutex() {
        pthread_cond_destroy(&no_writer_condvar);
        pthread_cond_destroy(&no_reader_condvar);
    }

    void shared_timed_mutex::lock() {
        const unique_lock<mutex> lock(mtx);
        while (writer_entered) {
            const int ec = pthread_cond_wait(&no_writer_condvar, mtx.native_handle());
            if (ec != 0) throw system_error(ec, system_category());
        }

        writer_entered = true;
        while (reader_entered != 0) {
            const int ec = pthread_cond_wait(&no_reader_condvar, mtx.native_handle());
            if (ec != 0) throw system_error(ec, system_category());
        }
    }

    bool shared_timed_mutex::try_lock() noexcept {
        const unique_lock<mutex> lock(mtx);
        const bool acquired = !writer_entered && (reader_entered == 0);
        if (acquired) writer_entered = true;
        return acquired;
    }

    void shared_timed_mutex::unlock() noexcept {
        mtx.lock();
        writer_entered = false;
        mtx.unlock();

        pthread_cond_broadcast(&no_writer_condvar);
        pthread_cond_broadcast(&no_reader_condvar);
    }

    void shared_timed_mutex::lock_shared() {
        const unique_lock<mutex> lock(mtx);
        while (writer_entered || reader_entered == max_reader) {
            const int ec = pthread_cond_wait(&no_writer_condvar, mtx.native_handle());
            if (ec != 0) throw system_error(ec, system_category());
        }

        reader_entered++;
    }

    bool shared_timed_mutex::try_lock_shared() noexcept {
        const unique_lock<mutex> lock(mtx);
        const bool acquired = !writer_entered && reader_entered != max_reader;
        if (acquired) reader_entered++;
        return acquired;
    }

    void shared_timed_mutex::unlock_shared() noexcept {
        const unique_lock<mutex> lock(mtx);
        reader_entered--;
        if (writer_entered && reader_entered == 0) {
            pthread_cond_signal(&no_reader_condvar);
        } else if (reader_entered == max_reader - 1) {
            pthread_cond_signal(&no_writer_condvar);
        }
    }
#else
    void shared_timed_mutex::lock() { shared_mutex::lock(); }
    bool shared_timed_mutex::try_lock() noexcept { return shared_mutex::try_lock(); }
    void shared_timed_mutex::unlock() noexcept { shared_mutex::unlock(); }
    void shared_timed_mutex::lock_shared() { shared_mutex::lock_shared(); }
    bool shared_timed_mutex::try_lock_shared() noexcept { return shared_mutex::try_lock_shared(); }
    void shared_timed_mutex::unlock_shared() noexcept { shared_mutex::unlock_shared(); }
#endif
}