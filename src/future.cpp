#include "future.hpp"
#include "system_error.hpp"
#include "string.hpp"
#include "cstring.hpp"

namespace std {
    error_code make_error_code(future_errc e) noexcept {
        return error_code(static_cast<int>(e), future_category());
    }

    error_condition make_error_condition(future_errc e) noexcept {
        return error_condition(static_cast<int>(e), future_category());
    }

    const error_category& future_category() noexcept {
        static constexpr struct : public error_category {
            const char* name() const noexcept {
                return "future";
            }

            string message(int ev) const {
                return std::strerror(ev);
            }
        } obj;

        return obj;
    }

    future_error::future_error(future_errc e) : logic_error("std::future_error"), ec(make_error_code(e)) {}

    const error_code& future_error::code() const noexcept {
        return ec;
    }

    const char* future_error::what() const noexcept {
        return "std::future_error";
    }

    void promise<void>::set_value() {
        if (!this->state) {
            throw future_error(future_errc::no_state);
        }
        const lock_guard<mutex> lock(this->state->lock);
        if (this->state->ready() || this->state->stored) {
            throw future_error(future_errc::promise_already_satisfied);
        }
        this->state->state = __internal::__promise_base<char>::state_t::success;
        this->state->stored = true;
        this->state->cv.notify_all();
    }

    void promise<void>::set_value_at_thread_exit() {
        if (!this->state) {
            throw future_error(future_errc::no_state);
        }
        __internal::at_thread_exit_fn_block::initialize_key();
        {
            const lock_guard<mutex> lock(this->state->lock);
            if (this->state->ready() || this->state->stored) {
                throw future_error(future_errc::promise_already_satisfied);
            }
            this->state->stored = true;
        }
        __internal::at_thread_exit_fn_block::push_front(
            new typename __internal::__promise_base<char>::template set_at_thread_exit_fn_block<
                __internal::__promise_base<char>::state_t::success>(this->state));
    }

    void future<void>::get() {
        optional<exception_ptr> storage;

        {
            unique_lock<std::mutex> lock(this->state->lock);
            this->state->cv.wait(lock, [this] {
                return this->state->ready();
            });

            if (this->state->state == __internal::__promise_state<char>::error) {
                storage.emplace(*reinterpret_cast<exception_ptr*>(&this->state->storage));
            }
        }

        this->state.reset();
        if (storage) {
            rethrow_exception(*storage);
        }
    }
}