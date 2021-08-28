#pragma once

#include "pthread.h"

#include "mutex.hpp"
#include "chrono.hpp"
#include "ctime.hpp"
#include "stop_token.hpp"
#include "memory.hpp"

namespace std {
    enum class cv_status { no_timeout, timeout };

    /* 32.6.4 Class condition_variable */
    class condition_variable {
    private:
        pthread_cond_t handle;
    public:
        constexpr condition_variable() : handle(PTHREAD_COND_INITIALIZER) {}
        ~condition_variable();

        condition_variable(const condition_variable&) = delete;
        condition_variable& operator=(const condition_variable&) = delete;

        void notify_one() noexcept;
        void notify_all() noexcept;
        void wait(unique_lock<mutex>& lock);

        template<class Predicate>
        void wait(unique_lock<mutex>& lock, Predicate pred) {
            while (!pred()) {
                wait(lock);
            }
        }

        template<class Clock, class Duration>
        cv_status wait_until(unique_lock<mutex>& lock, const chrono::time_point<Clock, Duration>& abs_time) {
            const chrono::time_point<Clock, chrono::seconds> secs = chrono::time_point_cast<chrono::seconds>(abs_time);
            const chrono::time_point<Clock, chrono::nanoseconds> ns = chrono::time_point_cast<chrono::nanoseconds>(abs_time)
                 - chrono::time_point_cast<chrono::nanoseconds>(secs);

            const std::timespec abs_time_spec = { .tv_sec = secs.count(), .tv_nsec = ns.count() };
            const int ec = pthread_cond_timedwait(&handle, lock.mutex()->native_handle(), &abs_time_spec);

            if (ec == ETIMEDOUT) {
                return cv_status::timeout;
            } else if (ec) {
                terminate();
            } else {
                return cv_status::no_timeout;
            }
        }

        template<class Clock, class Duration, class Predicate>
        bool wait_until(unique_lock<mutex>& lock, const chrono::time_point<Clock, Duration>& abs_time, Predicate pred) {
            while (!pred()) {
                if (wait_until(lock, abs_time) == cv_status::timeout) {
                    return pred();
                }
            }
            return true;
        }

        template<class Rep, class Period>
        cv_status wait_for(unique_lock<mutex>& lock, const chrono::duration<Rep, Period>& rel_time) {
            const chrono::seconds sec = chrono::duration_cast<chrono::seconds>(rel_time);
            const std::timespec rel_time_spec = {
                .tv_sec = sec.count(),
                .tv_nsec = chrono::duration_cast<chrono::microseconds>(rel_time - sec).count()
            };
            const int ec = pthread_cond_timedwait_relative_np(
                &handle,
                lock.mutex()->native_handle(),
                &rel_time_spec
            );

            if (ec == ETIMEDOUT) {
                return cv_status::timeout;
            } else if (ec) {
                terminate();
            } else {
                return cv_status::no_timeout;
            }
        }

        template<class Rep, class Period, class Predicate>
        bool wait_for(unique_lock<mutex>& lock, const chrono::duration<Rep, Period>& rel_time, Predicate pred) {
            while (!pred()) {
                if (wait_for(lock, rel_time) == cv_status::timeout) {
                    return pred();
                }
            }
            return true;
        }

        using native_handle_type = pthread_cond_t*;
        native_handle_type native_handle();
    };

    class condition_variable_any {
    private:
        shared_ptr<mutex> mtx;
        condition_variable cv;
    public:
        condition_variable_any();
        ~condition_variable_any() = default;

        condition_variable_any(const condition_variable_any&) = delete;
        condition_variable_any& operator=(const condition_variable_any&) = delete;

        void notify_one() noexcept;
        void notify_any() noexcept;

        template<__internal::basic_lockable Lock>
        void wait(Lock& lock) {
            const shared_ptr<mutex> mtx_ref = mtx;
            mtx->lock();
            lock.unlock();
            unique_lock<mutex> l(*mtx, adopt_lock);
            cv.wait(l);
            l.release();
            mtx->unlock();
            lock.lock();
        }

        template<__internal::basic_lockable Lock, class Predicate>
        void wait(Lock& lock, Predicate pred) {
            while (!pred()) {
                wait(lock);
            }
        }

        template<__internal::basic_lockable Lock, class Clock, class Duration>
        cv_status wait_until(Lock& lock, const chrono::time_point<Clock, Duration>& abs_time) {
            const shared_ptr<mutex> mtx_ref = mtx;
            mtx->lock();
            lock.unlock();
            unique_lock<mutex> l(*mtx, adopt_lock);
            const cv_status result = cv.wait_until(l, abs_time);
            l.release();
            mtx->unlock();
            lock.lock();
            return result;
        }

        template<__internal::basic_lockable Lock, class Clock, class Duration, class Predicate>
        bool wait_until(Lock& lock, const chrono::time_point<Clock, Duration>& abs_time, Predicate pred) {
            while (!pred()) {
                if (wait_until(lock, abs_time) == cv_status::timeout) {
                    return pred();
                }
            }
            return true;
        }

        template<__internal::basic_lockable Lock, class Rep, class Period>
        cv_status wait_for(Lock& lock, const chrono::duration<Rep, Period>& rel_time) {
            const shared_ptr<mutex> mtx_ref = mtx;
            mtx->lock();
            lock.unlock();
            unique_lock<mutex> l(*mtx, adopt_lock);
            const cv_status result = cv.wait_for(l, rel_time);
            l.release();
            mtx->unlock();
            lock.lock();
            return result;
        }

        template<__internal::basic_lockable Lock, class Rep, class Period, class Predicate>
        bool wait_for(Lock& lock, const chrono::duration<Rep, Period>& rel_time, Predicate pred) {
            while (!pred()) {
                if (wait_for(lock, rel_time) == cv_status::timeout) {
                    return pred();
                }
            }
            return true;
        }

        template<__internal::basic_lockable Lock, class Predicate>
        bool wait(Lock& lock, stop_token stoken, Predicate pred) {
            while (!stoken.stop_requested()) {
                if (pred()) {
                    return true;
                }
                wait(lock);
            }
            return pred();
        }

        template<__internal::basic_lockable Lock, class Clock, class Duration, class Predicate>
        bool wait_until(Lock& lock, stop_token stoken, const chrono::time_point<Clock, Duration>& abs_time, Predicate pred) {
            while (!stoken.stop_requested()) {
                if (pred()) {
                    return true;
                } else if (wait_until(lock, abs_time) == cv_status::timeout) {
                    return pred();
                }
            }

            return pred();
        }

        template<__internal::basic_lockable Lock, class Rep, class Period, class Predicate>
        bool wait_for(Lock& lock, stop_token stoken, const chrono::duration<Rep, Period>& rel_time, Predicate pred) {
            while (!stoken.stop_requested()) {
                if (pred()) {
                    return true;
                } else if (wait_for(lock, rel_time) == cv_status::timeout) {
                    return pred();
                }
            }

            return pred();
        }
    };

    /* 32.6.3 Non-member functions */
    void notify_all_at_thread_exit(condition_variable& cond, unique_lock<mutex> lk);
}
