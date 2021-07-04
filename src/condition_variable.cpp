#include "condition_variable.hpp"
#include "system_error.hpp"
#include "mutex.hpp"
#include "exception.hpp"

#include "pthread.h"

namespace std {
    condition_variable::~condition_variable() { pthread_cond_destroy(&handle); }

    void condition_variable::notify_one() noexcept { pthread_cond_signal(&handle); }
    void condition_variable::notify_all() noexcept { pthread_cond_broadcast(&handle); }
    void condition_variable::wait(unique_lock<mutex>& lock) {
        const int ec = pthread_cond_wait(&handle, lock.mutex()->native_handle());
        if (ec) terminate();
    }

    condition_variable::native_handle_type condition_variable::native_handle() { return &handle; }

    condition_variable_any::condition_variable_any() : mtx(make_shared<mutex>()), cv() {}

    void condition_variable_any::notify_one() noexcept { cv.notify_one(); }
    void condition_variable_any::notify_any() noexcept { cv.notify_all(); }

    namespace __internal {
        struct __notify_all_at_thread_exit_struct {
        private:
            condition_variable& cond;
            std::mutex* mtx;
        public:
            constexpr __notify_all_at_thread_exit_struct(condition_variable& cond, std::mutex* mtx)
                : cond(cond), mtx(mtx) {}
            ~__notify_all_at_thread_exit_struct();
        };
    }

    __internal::__notify_all_at_thread_exit_struct::~__notify_all_at_thread_exit_struct() {
        mtx->unlock();
        cond.notify_all();
    } 

    void notify_all_at_thread_exit(condition_variable& cond, unique_lock<mutex> lk) {
        const thread_local static __internal::__notify_all_at_thread_exit_struct destructor(cond, lk.release());
    }
}
