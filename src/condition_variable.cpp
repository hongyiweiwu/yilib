#include "condition_variable.hpp"
#include "system_error.hpp"
#include "mutex.hpp"
#include "exception.hpp"
#include "util/at_thread_exits.hpp"

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

    void notify_all_at_thread_exit(condition_variable& cond, unique_lock<mutex> lk) {
        __internal::at_thread_exit_fn_block::initialize_key();

        struct notify_all_at_thread_exit_fn_block : public __internal::at_thread_exit_fn_block {
            condition_variable* const cond;
            std::mutex* const mtx;

            notify_all_at_thread_exit_fn_block(condition_variable& cond, unique_lock<mutex> lk)
                : __internal::at_thread_exit_fn_block({ .callback = callback, .next = nullptr }), cond(&cond), mtx(lk.release()) {}

            static void callback(void* p) {
                notify_all_at_thread_exit_fn_block* const ptr = static_cast<notify_all_at_thread_exit_fn_block*>(p);
                ptr->mtx->unlock();
                ptr->cond->notify_all();
                delete ptr;
            }
        };

        __internal::at_thread_exit_fn_block::push_front(new notify_all_at_thread_exit_fn_block(cond, move(lk)));
    }
}
