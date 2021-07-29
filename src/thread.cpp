#include "stop_token.hpp"
#include "thread.hpp"
#include "cstddef.hpp"
#include "exception.hpp"
#include "system_error.hpp"

#include "pthread.h"
#include "unistd.h"

namespace std {
    thread::thread() noexcept : handle(0) {}

    thread::~thread() {
        if (joinable()) {
            terminate();
        }
    }

    thread::thread(thread&& other) noexcept : handle(other.handle) {
        other.handle = 0;
    }

    thread& thread::operator=(thread&& other) noexcept {
        if (joinable()) {
            terminate();
        }
        handle = other.handle;
        other.handle = 0;
        return *this;
    }

    void thread::swap(thread& other) noexcept {
        std::swap(handle, other.handle);
    }

    bool thread::joinable() const noexcept {
        return handle != 0;
    }

    void thread::join() {
        if (get_id() == this_thread::get_id()) {
            throw system_error(make_error_code(errc::resource_deadlock_would_occur));
        } else if (!joinable()) {
            throw system_error(make_error_code(errc::invalid_argument));
        }

        const int ec = pthread_join(handle, nullptr);
        if (ec != 0) {
            throw system_error(ec, system_category());
        }
    }

    void thread::detach() {
        if (!joinable()) {
            throw system_error(make_error_code(errc::invalid_argument));
        }

        const int ec = pthread_detach(handle);
        if (ec != 0) {
            throw system_error(ec, system_category());
        }

        handle = 0;
    }

    thread::id thread::get_id() const noexcept {
        return id(handle);
    }

    thread::native_handle_type thread::native_handle() {
        return handle;
    }

    unsigned int thread::hardware_concurrency() noexcept {
        return sysconf(_SC_NPROCESSORS_ONLN);
    }

    bool operator==(thread::id x, thread::id y) noexcept { 
        return x.identifier == y.identifier;
    }

    strong_ordering operator<=>(thread::id x, thread::id y) noexcept { 
        return x.identifier <=> y.identifier; 
    }

    std::size_t hash<thread::id>::operator()(const thread::id& id) const {
        return reinterpret_cast<std::size_t>(id.identifier);
    }

    void swap(thread& x, thread& y) noexcept {
        x.swap(y);
    }

    jthread::jthread() noexcept : handle(), ssource() {}

    jthread::~jthread() {
        if (joinable()) {
            request_stop();
            join();
        }
    }

    jthread::jthread(jthread&& rhs) noexcept : handle(rhs.handle), ssource(move(rhs).ssource) {
        rhs.handle = 0;
    }

    jthread& jthread::operator=(jthread&& rhs) noexcept {
        if (joinable()) {
            request_stop();
            join();
        }

        handle = rhs.handle;
        rhs.handle = 0;
        ssource = move(rhs).ssource;
        return *this;
    }

    void jthread::swap(jthread& other) noexcept {
        std::swap(handle, other.handle);
        std::swap(ssource, other.ssource);
    }

    bool jthread::joinable() const noexcept {
        return handle != 0;
    }

    void jthread::join() {
        if (get_id() == this_thread::get_id()) {
            throw system_error(make_error_code(errc::resource_deadlock_would_occur));
        } else if (!joinable()) {
            throw system_error(make_error_code(errc::invalid_argument));
        }

        const int ec = pthread_join(handle, nullptr);
        if (ec != 0) {
            throw system_error(ec, system_category());
        }
    }

    void jthread::detach() {
        if (!joinable()) {
            throw system_error(make_error_code(errc::invalid_argument));
        }

        const int ec = pthread_detach(handle);
        if (ec != 0) {
            throw system_error(ec, system_category());
        }

        handle = 0;
    }

    jthread::id jthread::get_id() const noexcept {
        return id(handle);
    }

    jthread::native_handle_type jthread::native_handle() {
        return handle;
    }

    stop_source jthread::get_stop_source() noexcept {
        return ssource;
    }

    stop_token jthread::get_stop_token() noexcept {
        return ssource.get_token();
    }

    bool jthread::request_stop() noexcept {
        return ssource.request_stop();
    }

    unsigned int jthread::hardware_concurrency() noexcept {
        return thread::hardware_concurrency();
    }

    void swap(jthread& x, jthread& y) noexcept { 
        x.swap(y);
    }

    namespace __internal {
        struct __thread_id_derived : public thread::id {
        public:
            constexpr __thread_id_derived(pthread_t id) : thread::id(id) {}
        };
    }

    namespace this_thread {
        thread::id get_id() noexcept {
            return thread::id(__internal::__thread_id_derived(pthread_self()));
        }

        void yield() noexcept {
            sched_yield();
        }
    }
}