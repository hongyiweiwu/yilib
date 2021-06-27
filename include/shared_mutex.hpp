#include "pthread.h"

#include "cstddef.hpp"
#include "mutex.hpp"
#include "memory.hpp"
#include "system_error.hpp"

// TODO: Implement shared_timed_mutex.

namespace std {
    class shared_mutex {
    private:
        pthread_rwlock_t handle;
        static constexpr int max_reader = 10000;
    public:
        constexpr shared_mutex() : handle(PTHREAD_RWLOCK_INITIALIZER) {}
        ~shared_mutex();

        shared_mutex(const shared_mutex&) = delete;
        shared_mutex& operator=(const shared_mutex&) = delete;

        void lock();
        bool try_lock() noexcept;
        void unlock() noexcept;

        void lock_shared();
        bool try_lock_shared() noexcept;
        void unlock_shared() noexcept;

        using native_handle_type = pthread_rwlock_t*;
        native_handle_type native_handle();
    };

    namespace __internal {
        template<class T>
        concept shared_mutex = mutex<T> && requires (T& lock) {
            lock.lock_shared();
            lock.try_lock_shared();
            lock.unlock_shared();
        };
    }

    template<__internal::shared_mutex Mutex>
    class shared_lock {
    public:
        using mutex_type = Mutex;

        shared_lock() noexcept : pm(nullptr), owns(false) {}
        explicit shared_lock(mutex_type& m) : pm(addressof(m)), owns(true) {
            m.lock_shared();
        }

        shared_lock(mutex_type& m, defer_lock_t) noexcept : pm(addressof(m)), owns(false) {}
        shared_lock(mutex_type& m, try_to_lock_t) : pm(addressof(m)), owns(m.try_lock_shared()) {}
        shared_lock(mutex_type& m, adopt_lock_t) : pm(addressof(m)), owns(true) {}
        template<class Clock, class Duration>
        shared_lock(mutex_type& m, const chrono::time_point<Clock, Duration>& abs_time) : pm(addressof(m)), owns(m.try_lock_shared_until(abs_time)) {}
        template<class Rep, class Period>
        shared_lock(mutex_type& m, const chrono::duration<Rep, Period>& rel_time) : pm(addressof(m)), owns(m.try_lock_shared_for(rel_time)) {}
        ~shared_lock() { if (owns) pm->unlock_shared(); }

        shared_lock(const shared_lock&) = delete;
        shared_lock& operator=(const shared_lock&) = delete;

        shared_lock(shared_lock&& u) noexcept : pm(u.pm), owns(u.owns) {
            u.pm = nullptr;
            u.owns = false;
        }

        shared_lock& operator=(shared_lock&& u) noexcept {
            if (owns) pm->unlock_shared();
            pm = u.pm;
            owns = u.owns;
            u.pm = nullptr;
            u.owns = false;
            return *this;
        }

        void lock() {
            if (!pm) throw system_error(make_error_code(errc::operation_not_permitted));
            else if (owns) throw system_error(make_error_code(errc::resource_deadlock_would_occur));
            pm->lock_shared();
            owns = true;
        }

        bool try_lock() {
            if (!pm) throw system_error(make_error_code(errc::operation_not_permitted));
            else if (owns) throw system_error(make_error_code(errc::resource_deadlock_would_occur));
            return owns = pm->try_lock_shared();
        }
        
        template<class Rep, class Period>
        bool try_lock_for(const chrono::duration<Rep, Period>& rel_time) {
            if (!pm) throw system_error(make_error_code(errc::operation_not_permitted));
            else if (owns) throw system_error(make_error_code(errc::resource_deadlock_would_occur));
            return owns = pm->try_lock_shared_for(rel_time);
        }

        template<class Clock, class Duration>
        bool try_lock_until(const chrono::time_point<Clock, Duration>& abs_time) {
            if (!pm) throw system_error(make_error_code(errc::operation_not_permitted));
            else if (owns) throw system_error(make_error_code(errc::resource_deadlock_would_occur));
            return owns = pm->try_lock_shared_until(abs_time);
        }

        void unlock() { if (!owns) throw system_error(make_error_code(errc::operation_not_permitted)); }

        void swap(shared_lock& u) noexcept {
            std::swap(pm, u.pm);
            std::swap(owns, u.owns);
        }

        mutex_type* release() noexcept {
            const mutex_type* old_mutex = pm;
            pm = nullptr;
            owns = false;
            return old_mutex;
        }

        bool owns_lock() const noexcept { return owns; }
        explicit operator bool() const noexcept { return owns; }
        mutex_type* mutex() const noexcept { return pm; }

    private:
        mutex_type* pm;
        bool owns;
    };

    template<class Mutex>
    void swap(shared_lock<Mutex>& x, shared_lock<Mutex>& y) { x.swap(y); }
}