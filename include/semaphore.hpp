#pragma once

#include "cstddef.hpp"
#include "chrono.hpp"
#include "limits.hpp"
#include "system_error.hpp"
#include "mutex.hpp"
#include "ctime.hpp"

namespace std {
#if !defined(__APPLE__)
    #include "semaphore.h"

    template<std::ptrdiff_t least_max_value = numeric_limits<std::ptrdiff_t>::max()>
    class counting_semaphore {
    private:
        sem_t handle;

    public:
        static constexpr std::ptrdiff_t max() noexcept { return least_max_value; }

        constexpr explicit counting_semaphore(std::ptrdiff_t desired) { sem_init(&handle, 0, desired); }
        ~counting_semaphore() { sem_destroy(&handle); }

        counting_semaphore(const counting_semaphore&) = delete;
        counting_semaphore& operator=(const counting_semaphore&) = delete;

        void release(std::ptrdiff_t update = 1) {
            for (std::ptrdiff_t i = 0; i < update; i++) {
                const int ec = sem_post(&handle);
                if (ec) throw system_error(ec, system_category());
            }
        }

        void acquire() { 
            const int ec = sem_wait(&handle);
            if (ec) throw system_error(ec, system_category());
        }

        bool try_acquire() noexcept { return !sem_trywait(&handle); }

        template<class Rep, class Period>
        bool try_acquire_for(const chrono::duration<Rep, Period>& rel_time) {
            return try_acquire_until(chrono::steady_clock::now() + rel_time);
        }

        template<class Clock, class Duration>
        bool try_acquire_until(const chrono::time_point<Clock, Duration>& abs_time) {
            const chrono::time_point<Clock, chrono::seconds> secs = chrono::time_point_cast<chrono::seconds>(abs_time);
            const chrono::time_point<Clock, chrono::nanoseconds> ns = chrono::time_point_cast<chrono::nanoseconds>(abs_time)
                 - chrono::time_point_cast<chrono::nanoseconds>(secs);

            const std::timespec abs_time_spec = { .tv_sec = secs.count(), .tv_nsec = ns.count() };
            const int ec = sem_timedwait(&handle, &abs_time_spec);
            if (ec) throw system_error(ec, system_category());
            else return true;
        }
    };
#else
    #include "pthread.h"

    template<std::ptrdiff_t least_max_value = numeric_limits<std::ptrdiff_t>::max()>
    class counting_semaphore {
    private:
        std::ptrdiff_t count;
        mutex mtx;
        pthread_cond_t condvar;

    public:
        static constexpr std::ptrdiff_t max() noexcept { return least_max_value; }

        constexpr explicit counting_semaphore(std::ptrdiff_t desired) : count(desired), condvar(PTHREAD_COND_INITIALIZER) {}
        ~counting_semaphore() {
            pthread_cond_destroy(&condvar);
        }

        counting_semaphore(const counting_semaphore&) = delete;
        counting_semaphore& operator=(const counting_semaphore&) = delete;

        void release(std::ptrdiff_t update = 1) {
            mtx.lock();
            count += update;
            mtx.unlock();
            while (update != 0) {
                const int ec = pthread_cond_signal(&condvar);
                if (ec != 0) throw system_error(ec, system_category());
                update--;
            }
        }

        void acquire() { 
            const unique_lock<mutex> lock(mtx);
            while (count == 0) {
                const int ec = pthread_cond_wait(&condvar, mtx.native_handle());
                if (ec != 0) throw system_error(ec, system_category());
            }

            count--;
        }

        bool try_acquire() noexcept {
            const unique_lock<mutex> lock(mtx);
            const bool acquired = count != 0;
            if (acquired) count--;
            return acquired;
        }

        template<class Rep, class Period>
        bool try_acquire_for(const chrono::duration<Rep, Period>& rel_time) {
            return try_acquire_until(chrono::steady_clock::now() + rel_time);
        }

        template<class Clock, class Duration>
        bool try_acquire_until(const chrono::time_point<Clock, Duration>& abs_time) {
            const chrono::time_point<Clock, chrono::seconds> secs = chrono::time_point_cast<chrono::seconds>(abs_time);
            const chrono::time_point<Clock, chrono::nanoseconds> ns = chrono::time_point_cast<chrono::nanoseconds>(abs_time)
                 - chrono::time_point_cast<chrono::nanoseconds>(secs);

            const std::timespec abs_time_spec = { .tv_sec = secs.count(), .tv_nsec = ns.count() };

            const unique_lock<mutex> lock(mtx);
            while (count == 0) {
                const int ec = pthread_cond_timedwait(&condvar, mtx.native_handle(), &abs_time_spec);
                if (ec != 0) return false;
            }

            count--;
            return true;
        }
    };
#endif

    using binary_semaphore = counting_semaphore<1>;
}
