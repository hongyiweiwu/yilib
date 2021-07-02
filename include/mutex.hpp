#pragma once

#include "pthread.h"

#include "stdexcept.hpp"
#include "cstddef.hpp"
#include "ctime.hpp"
#include "chrono.hpp"
#include "tuple.hpp"
#include "utility.hpp"
#include "memory.hpp"
#include "system_error.hpp"
#include "concepts.hpp"
#include "type_traits.hpp"
#include "functional.hpp"
#include "cerrno.hpp"

namespace std {
    class mutex {
    public:
        constexpr mutex() noexcept : mut(PTHREAD_MUTEX_INITIALIZER) {}
        ~mutex();

        mutex(const mutex&) = delete;
        mutex& operator=(const mutex&) = delete;

        void lock();
        bool try_lock() noexcept;
        void unlock() noexcept;

        using native_handle_type = ::pthread_mutex_t*;
        native_handle_type native_handle();

    private:
        pthread_mutex_t mut;
    };

    class recursive_mutex {
    protected:
        /* RAII-wrapper of the pthread_mutexattr_t struct. */
        struct attr {
            pthread_mutexattr_t handle;

            attr(int type);
            ~attr();
        };
    public:
        recursive_mutex();
        ~recursive_mutex();

        recursive_mutex(const mutex&) = delete;
        recursive_mutex& operator=(const mutex&) = delete;

        void lock();
        bool try_lock() noexcept;
        void unlock() noexcept;

        using native_handle_type = ::pthread_mutex_t*;
        native_handle_type native_handle();

    private:
        pthread_mutex_t mut;

        static attr mut_attr;
    };

#if defined(__APPLE__)
    class timed_mutex {
    private:
        pthread_cond_t condvar;
        mutex mtx;
        bool locked;
    
    public:
        timed_mutex();
        ~timed_mutex();

        timed_mutex(const timed_mutex&) = delete;
        timed_mutex& operator=(const timed_mutex&) = delete;

        void lock();
        bool try_lock();

        template<class Rep, class Period>
        bool try_lock_for(const chrono::duration<Rep, Period>& rel_time) {
            return try_lock_until(chrono::steady_clock::now() + rel_time);
        }

        template<class Clock, class Duration>
        bool try_lock_until(const chrono::time_point<Clock, Duration>& abs_time) {
            const chrono::time_point<Clock, chrono::seconds> secs = chrono::time_point_cast<chrono::seconds>(abs_time);
            const chrono::time_point<Clock, chrono::nanoseconds> ns = chrono::time_point_cast<chrono::nanoseconds>(abs_time)
                 - chrono::time_point_cast<chrono::nanoseconds>(secs);

            const std::timespec abs_time_spec = { .tv_sec = secs.count(), .tv_nsec = ns.count() };

            mtx.lock();
            while (locked) {
                const int ec = pthread_cond_timedwait(&condvar, mtx.native_handle(), &abs_time_spec);
                if (ec == ETIMEDOUT) {
                    mtx.unlock();
                    return false;
                } else if (ec != 0) {
                    mtx.unlock();
                    throw system_error(ec, system_category());
                }
            }

            locked = true;
            mtx.unlock();
            return true;
        }

        void unlock() noexcept;
    };
#else
    class timed_mutex : private mutex {
    public:
        timed_mutex() = default;
        ~timed_mutex() = default;

        timed_mutex(const timed_mutex&) = delete;
        timed_mutex& operator=(const timed_mutex&) = delete;

        void lock();
        bool try_lock();

        template<class Rep, class Period>
        bool try_lock_for(const chrono::duration<Rep, Period>& rel_time) {
            return try_lock_until(chrono::steady_clock::now() + rel_time);
        }

        template<class Clock, class Duration>
        bool try_lock_until(const chrono::time_point<Clock, Duration>& abs_time) {
            const chrono::time_point<Clock, chrono::seconds> secs = chrono::time_point_cast<chrono::seconds>(abs_time);
            const chrono::time_point<Clock, chrono::nanoseconds> ns = chrono::time_point_cast<chrono::nanoseconds>(abs_time)
                 - chrono::time_point_cast<chrono::nanoseconds>(secs);

            const std::timespec abs_time_spec = { .tv_sec = secs.count(), .tv_nsec = ns.count() };
            return !pthread_mutex_timedlock(native_handle(), &abs_time_spec);
        }

        void unlock() noexcept;

        using native_handle_type = mutex::native_handle_type;
        native_handle_type native_handle();
    };
#endif

#if defined(__APPLE__)
    class recursive_timed_mutex {
    private:
        pthread_cond_t condvar;
        recursive_mutex mtx;
        bool locked;
    
    public:
        recursive_timed_mutex();
        ~recursive_timed_mutex();

        recursive_timed_mutex(const recursive_timed_mutex&) = delete;
        recursive_timed_mutex& operator=(const recursive_timed_mutex&) = delete;

        void lock();
        bool try_lock();

        template<class Rep, class Period>
        bool try_lock_for(const chrono::duration<Rep, Period>& rel_time) {
            return try_lock_until(chrono::steady_clock::now() + rel_time);
        }

        template<class Clock, class Duration>
        bool try_lock_until(const chrono::time_point<Clock, Duration>& abs_time) {
            const chrono::time_point<Clock, chrono::seconds> secs = chrono::time_point_cast<chrono::seconds>(abs_time);
            const chrono::time_point<Clock, chrono::nanoseconds> ns = chrono::time_point_cast<chrono::nanoseconds>(abs_time)
                 - chrono::time_point_cast<chrono::nanoseconds>(secs);

            const std::timespec abs_time_spec = { .tv_sec = secs.count(), .tv_nsec = ns.count() };

            mtx.lock();
            while (locked) {
                const int ec = pthread_cond_timedwait(&condvar, mtx.native_handle(), &abs_time_spec);
                if (ec == ETIMEDOUT) {
                    mtx.unlock();
                    return false;
                } else if (ec != 0) {
                    mtx.unlock();
                    throw system_error(ec, system_category());
                }
            }

            locked = true;
            mtx.unlock();
            return true;
        }

        void unlock() noexcept;
    };
#else
    class recursive_timed_mutex : private recursive_mutex {
    public:
        recursive_timed_mutex() = default;
        ~recursive_timed_mutex() = default;

        recursive_timed_mutex(const recursive_timed_mutex&) = delete;
        recursive_timed_mutex& operator=(const recursive_timed_mutex&) = delete;

        void lock();
        bool try_lock();

        template<class Rep, class Period>
        bool try_lock_for(const chrono::duration<Rep, Period>& rel_time) {
            return try_lock_until(chrono::steady_clock::now() + rel_time);
        }

        template<class Clock, class Duration>
        bool try_lock_until(const chrono::time_point<Clock, Duration>& abs_time) {
            const chrono::time_point<Clock, chrono::seconds> secs = chrono::time_point_cast<chrono::seconds>(abs_time);
            const chrono::time_point<Clock, chrono::nanoseconds> ns = chrono::time_point_cast<chrono::nanoseconds>(abs_time)
                 - chrono::time_point_cast<chrono::nanoseconds>(secs);

            const std::timespec abs_time_spec = { .tv_sec = secs.count(), .tv_nsec = ns.count() };
            return !pthread_mutex_timedlock(native_handle(), &abs_time_spec);
        }

        void unlock() noexcept;

        using native_handle_type = mutex::native_handle_type;
        native_handle_type native_handle();
    };
#endif

    struct defer_lock_t { explicit defer_lock_t() = default; };
    struct try_to_lock_t { explicit try_to_lock_t() = default; };
    struct adopt_lock_t { explicit adopt_lock_t() = default; };

    inline constexpr defer_lock_t defer_lock;
    inline constexpr try_to_lock_t try_to_lock;
    inline constexpr adopt_lock_t adopt_lock;

    /* 32.5.5 Locks */
    namespace __internal {
        template<class T>
        concept basic_lockable = requires (T lock) {
            lock.lock();
            { lock.unlock() } noexcept;
        };

        template<class T>
        concept lockable = basic_lockable<T> && requires (T lock) {
            { lock.try_lock() } -> same_as<bool>;
        };

        template<class T>
        concept mutex = lockable<T> && default_initializable<T> && destructible<T> 
            && !copy_constructible<T> && !move_constructible<T> && !assignable_from<T&, const T&> && !assignable_from<T&, T>;
    }

    template<__internal::basic_lockable Mutex> class lock_guard {
    public:
        using mutex_type = Mutex;

        explicit lock_guard(mutex_type& m) : pm(m) { m.lock(); }
        lock_guard(mutex_type& m, adopt_lock_t) : pm(m) {}
        ~lock_guard() { pm.unlock(); }

        lock_guard(const lock_guard&) = delete;
        lock_guard& operator=(const lock_guard&) = delete;

    private:
        mutex_type& pm;
    };

    namespace __internal {
        template<class ...MutexTypes>
        class __scoped_lock_base {};

        template<class MutexType>
        class __scoped_lock_base<MutexType> {
        public:
            using mutex_type = MutexType;
        };
    }

    template<class ...MutexTypes> requires (sizeof...(MutexTypes) == 1 && (__internal::basic_lockable<MutexTypes> && ...))
        || (__internal::lockable<MutexTypes> && ...)
    class scoped_lock : public __internal::__scoped_lock_base<MutexTypes...> {
    public:
        explicit scoped_lock(MutexTypes&... m)  : pm(tie(m...)) {
            if constexpr (sizeof...(MutexTypes) == 1) (m.lock(), ...);
            else if constexpr (sizeof...(MutexTypes) > 1) lock(m...);
        }

        explicit scoped_lock(adopt_lock_t, MutexTypes&... m) : pm(tie(m...)) {}

        ~scoped_lock() {
            constexpr auto helper = []<size_t ...I>(index_sequence<I...>, tuple<MutexTypes&...>& tp) {
                (std::get<I>(tp).unlock(), ...);
            };

            helper(index_sequence_for<MutexTypes...>{}, pm);
        };

        scoped_lock(const scoped_lock&) = delete;
        scoped_lock& operator=(const scoped_lock&) = delete;

    private:
        tuple<MutexTypes&...> pm;
    };

    template<__internal::basic_lockable Mutex> class unique_lock {
    public:
        using mutex_type = Mutex;

        unique_lock() noexcept : pm(nullptr), owns(false) {}
        explicit unique_lock(mutex_type& m) : pm(addressof(m)), owns(true) { m.lock(); }
        unique_lock(mutex_type& m, defer_lock_t) noexcept : pm(addressof(m)), owns(false) {}
        unique_lock(mutex_type& m, try_to_lock_t) requires __internal::lockable<Mutex>
            : pm(addressof(m)), owns(m.try_lock()) {}
        unique_lock(mutex_type& m, adopt_lock_t) : pm(addressof(m)), owns(true) {}
        template<class Clock, class Duration>
        unique_lock(mutex_type& m, const chrono::time_point<Clock, Duration>& abs_time) : pm(addressof(m)), owns(m.try_lock_until(abs_time)) {}
        template<class Rep, class Period>
        unique_lock(mutex_type& m, const chrono::duration<Rep, Period>& rel_time) : pm(addressof(m)), owns(m.try_lock_for(rel_time)) {}
        ~unique_lock() { if (owns) pm->unlock(); }

        unique_lock(const unique_lock&) = delete;
        unique_lock& operator=(const unique_lock&) = delete;

        unique_lock(unique_lock&& u) noexcept : pm(u.pm), owns(u.owns) {
            u.pm == 0;
            u.owns == false;
        }

        unique_lock& operator=(unique_lock&& u) {
            if (owns) pm->unlock();
            pm = u.pm;
            owns = u.owns;
            u.pm = nullptr;
            u.owns = false;
            return *this;
        }

        void lock() {
            if (!pm) throw system_error(make_error_code(errc::operation_not_permitted));
            else if (owns) throw system_error(make_error_code(errc::resource_deadlock_would_occur));
            pm->lock();
            owns = true;
        }

        bool try_lock() requires __internal::lockable<Mutex> { 
            if (!pm) throw system_error(make_error_code(errc::operation_not_permitted));
            else if (owns) throw system_error(make_error_code(errc::resource_deadlock_would_occur));
            owns = pm->try_lock(); 
            return owns;    
        }

        template<class Rep, class Period>
        bool try_lock_for(const chrono::duration<Rep, Period>& rel_time) {
            if (!pm) throw system_error(make_error_code(errc::operation_not_permitted));
            else if (owns) throw system_error(make_error_code(errc::resource_deadlock_would_occur));
            owns = pm->try_lock_for(rel_time);
            return owns;
        }

        template<class Clock, class Duration>
        bool try_lock_until(const chrono::time_point<Clock, Duration>& abs_time) {
            if (!pm) throw system_error(make_error_code(errc::operation_not_permitted));
            else if (owns) throw system_error(make_error_code(errc::resource_deadlock_would_occur));
            owns = pm->try_lock_until(abs_time);
            return owns;
        }

        void unlock() {
            if (!owns) throw system_error(make_error_code(errc::operation_not_permitted));
            pm->unlock();
            owns = false;
        }

        void swap(unique_lock& u) noexcept {
            std::swap(pm, u.pm);
            std::swap(owns, u.owns);
        }

        mutex_type* release() noexcept {
            const mutex_type* old_pm = pm;
            pm = nullptr;
            owns = false;
            return old_pm;
        }

        bool owns_lock() const noexcept { return owns; }
        explicit operator bool() const noexcept { return owns; }
        mutex_type* mutex() const noexcept { return pm; }

    private:
        mutex_type* pm;
        bool owns;
    };

    template<class Mutex>
    void swap(unique_lock<Mutex>& x, unique_lock<Mutex>& y) noexcept { x.swap(y); }

    /* 32.5.6 Generic locking algorithms */
    template<__internal::lockable L1, __internal::lockable L2, __internal::lockable... LN>
    int try_lock(L1& lock1, L2& lock2, LN&... lockn) {
        static constexpr auto helper = []<__internal::lockable Lock, __internal::lockable ...Locks>(const auto& next, Lock& lock, Locks&... locks) -> int {
            static constexpr size_t curr_index = sizeof...(LN) + 1 - sizeof...(Locks);
            bool locked = false;
            try {
                if ((locked = lock.try_lock())) {
                    if constexpr (sizeof...(Locks) == 0) return -1;
                    else {
                        const int helper_result = next(next, locks...);
                        if (helper_result != -1) {
                            lock.unlock();
                            return curr_index;
                        } else {
                            return -1;
                        }
                    }
                } else {
                    return curr_index;
                }
            } catch (...) {
                if (locked) lock.unlock();
                throw;
            }
        };

        return helper(helper, lock1, lock2, lockn...);
    }

    template<__internal::lockable L1, __internal::lockable L2, __internal::lockable... LN>
    int lock(L1& lock1, L2& lock2, LN&... lockn) {
        // TODO: Implement.
        return 0;
    }

    /* 32.5.7 Call once */
    struct once_flag {
        constexpr once_flag() noexcept : mut(), completed(false) {}
        once_flag(const once_flag&) = delete;
        once_flag& operator=(const once_flag&) = delete;

    private:
        mutex mut;
        bool completed;

        friend void call_once(once_flag& flag, auto&& func, auto&& ...args);
    };

    template<class Callable, class ...Args> requires is_invocable_v<Callable, Args...>
    void call_once(once_flag& flag, Callable&& func, Args&& ...args) {
        const lock_guard<mutex> lguard = lock_guard(flag.mut);
        if (flag.completed) return;
        invoke(forward<Callable>(func), forward<Args>(args)...);
        flag.completed = true;
    }

}