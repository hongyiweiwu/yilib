#include "stop_token.hpp"
#include "utility.hpp"
#include "mutex.hpp"
#include "memory.hpp"
#include "exception.hpp"
#include "thread.hpp"

namespace std {
    __internal::__stop_callback_list_entry::__stop_callback_list_entry(__stop_callback_list_entry* before, __stop_callback_list_entry* next, __stop_callback_base* callback)
        : before(before), next(next), callback(callback), destroyed(nullptr), executed(0) {
            if (before) before->next = this;
            if (next) next->before = this;
        }

    __internal::__stop_callback_list_entry::~__stop_callback_list_entry() {
        if (before) before->next = next;
        if (next) next->before = before;
    }

    __internal::__stop_state::__stop_state() : refcount(1), stop_stat(1), callbacks_start(nullptr, nullptr), callbacks_end(nullptr, nullptr) {
        callbacks_start.next = &callbacks_end;
        callbacks_end.before = &callbacks_start;
    }

    void __internal::__stop_state::increment_ssource_refcount() noexcept {
        __atomic_fetch_add(&stop_stat, 1 << 1, __ATOMIC_SEQ_CST);
    }
    
    void __internal::__stop_state::decrement_ssource_refcount() noexcept {
        __atomic_fetch_sub(&stop_stat, 1 << 1, __ATOMIC_SEQ_CST);
    }

    void __internal::__stop_state::increment_refcount() noexcept {
        __atomic_fetch_add(&refcount, 1, __ATOMIC_SEQ_CST);
    }

    void __internal::__stop_state::decrement_refcount() noexcept {
        if (__atomic_sub_fetch(&refcount, 1, __ATOMIC_SEQ_CST) == 0) {
            delete this;
        } 
    }

    bool __internal::__stop_state::request_stop() noexcept {
        if (__atomic_fetch_or(&stop_stat, 1, __ATOMIC_SEQ_CST) & 1) {
            return false;
        }

        mtx.lock();
        callback_executor.emplace(this_thread::get_id().identifier);
        __stop_callback_list_entry* it = callbacks_start.next;
        while (it != &callbacks_end) {
            // TODO: Investigate if this is still valid if both the current callback and its immediate next callback are both deleted during the former's
            // execution.
            __stop_callback_list_entry* const curr_next = it->next;
            bool destroyed = false;
            it->destroyed = &destroyed;
            mtx.unlock();
            try {
                it->callback->execute();
            } catch (...) {
                terminate();
            }
            it->executed.release(1);
            mtx.lock();
            if (destroyed) it = curr_next;
            else {
                it = it->next;
                it->destroyed = nullptr;
            }
        }
        mtx.unlock();
        
        return true;
    }

    bool __internal::__stop_state::stop_requested() const noexcept {
        return __atomic_load_n(&stop_stat, __ATOMIC_SEQ_CST) & 1;
    }

    bool __internal::__stop_state::stop_possible() const noexcept {
        const std::size_t stop_stat = __atomic_load_n(&this->stop_stat, __ATOMIC_SEQ_CST);
        // The first condition makes sure stop has been requested; the second makes sure at least one stop_source owns it.
        return ((stop_stat & 1) != 0) && ((stop_stat | 1) != 1);
    }

    __internal::__stop_callback_list_entry* __internal::__stop_state::register_callback(__stop_callback_base* callback) noexcept {
        const unique_lock<std::mutex> lock(mtx);
        return new __internal::__stop_callback_list_entry(callbacks_end.before, &callbacks_end, callback);
    }

    stop_token::stop_token(__internal::__stop_state* state) noexcept : state(state) {
        if (state) state->increment_refcount();
    }

    stop_token::stop_token() noexcept : state(nullptr) {}
    
    stop_token::stop_token(const stop_token& other) noexcept : state(other.state) {
        if (state) state->increment_refcount();
    }

    stop_token::stop_token(stop_token&& other) noexcept : state(other.state) {
        other.state = nullptr;
    }

    stop_token& stop_token::operator=(const stop_token& other) noexcept {
        state = other.state;
        if (state) state->increment_refcount();
        return *this;
    }

    stop_token& stop_token::operator=(stop_token&& other) noexcept {
        state = other.state;
        other.state = nullptr;
        return *this;
    }

    stop_token::~stop_token() {
        if (state) state->decrement_refcount();
    }

    void stop_token::swap(stop_token& other) noexcept {
        std::swap(state, other.state);
    }

    [[nodiscard]] bool stop_token::stop_requested() const noexcept {
        return state && state->stop_requested();
    }

    [[nodiscard]] bool stop_token::stop_possible() const noexcept { 
        return state && state->stop_possible();
    }

    bool operator==(const stop_token& lhs, const stop_token& rhs) noexcept { return lhs.state == rhs.state; }
    void swap(stop_token& lhs, stop_token& rhs) noexcept { lhs.swap(rhs); }

    stop_source::stop_source() : state(new __internal::__stop_state()) {}
    stop_source::stop_source(nostopstate_t) noexcept : state(nullptr) {}

    stop_source::stop_source(const stop_source& rhs) noexcept : state(rhs.state) {
        if (state) {
            state->increment_refcount();
            state->increment_ssource_refcount();
        }
    }

    stop_source::stop_source(stop_source&& rhs) noexcept : state(rhs.state) {
        rhs.state = nullptr;
    }

    stop_source& stop_source::operator=(const stop_source& rhs) noexcept {
        state = rhs.state;
        if (state) {
            state->increment_refcount();
            state->increment_ssource_refcount();
        }
        return *this;
    }

    stop_source& stop_source::operator=(stop_source&& rhs) noexcept {
        state = rhs.state;
        rhs.state = nullptr;
        return *this;
    }

    stop_source::~stop_source() {
        state->decrement_ssource_refcount();
        state->decrement_refcount();
    }

    void stop_source::swap(stop_source& rhs) noexcept {
        std::swap(state, rhs.state);
    }

    [[nodiscard]] stop_token stop_source::get_token() const noexcept {
        return stop_possible() ? stop_token() : stop_token(state);
    }

    [[nodiscard]] bool stop_source::stop_possible() const noexcept { return state; }

    [[nodiscard]] bool stop_source::stop_requested() const noexcept { return state && state->stop_requested(); }
    bool stop_source::request_stop() noexcept { return state && state->request_stop(); }

    [[nodiscard]] bool operator==(const stop_source& lhs, const stop_source& rhs) noexcept { return lhs.state == rhs.state; }
    void swap(stop_source& lhs, stop_source& rhs) noexcept { lhs.swap(rhs); }
}