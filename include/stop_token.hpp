#pragma once

#include "cstddef.hpp"
#include "type_traits.hpp"
#include "concepts.hpp"
#include "utility.hpp"
#include "mutex.hpp"
#include "exception.hpp"
#include "memory.hpp"
#include "optional.hpp"
#include "thread.hpp"
#include "semaphore.hpp"

#include "pthread.h"

namespace std {
    // Forward declaration. Declared below.
    class stop_source;
    template<class Callback> class stop_callback;

    namespace __internal {
        /* Base class of stop_callback. It simply provides a virtual method which all stop_callbacks override to invoke the callback. Since stop_callback
         * is a class template, this provides a way to type-erase the Callback type; a potential list of callbacks can simply do cb->execute() to invoke
         * the callback. */
        struct __stop_callback_base {
            virtual void execute() = 0;
            virtual ~__stop_callback_base() = default;
        };

        /* An entry of the linked list that contains the stop_callbacks. This list is not synchronized, so when used in a multi-threaded setting an external
         * mutex is needed. */
        struct __stop_callback_list_entry {
        public:
            __stop_callback_list_entry* before;
            __stop_callback_list_entry* next;
            __stop_callback_base* callback;
            /* Pointer to a boolean variable temporarily owned by a thread A executing request_stop() and invoking the callback stored by this.
             * If it so happens that this is being destroyed at the same time, the destroyer thread will set this variable to true, to let 
             * thread A know that information here couldn't be accessed anymore. */
            bool* destroyed;
            /* A semaphore initialized to have no resource. It's given a resource by the thread A executing request_stop() and invoking the callback
             * stored by this after the execution is complete. The thread B destroying this should acquire the semaphore before the destruction. */
            binary_semaphore executed;

            __stop_callback_list_entry(__stop_callback_list_entry* before, __stop_callback_list_entry* after, __stop_callback_base* callback = nullptr);
            ~__stop_callback_list_entry();
        };

        struct __stop_state {
        private:
            size_t refcount;
            /* An unsigned number where the lowest bit represents whether a stop has been requested, and the remaining bits represents the number of
             * stop_sources that own it. 
             * This is designed so both states can be inquired atomically. */
            size_t stop_stat;

            /* Controls access to the callback list. */
            std::mutex mtx;
            /* Front sentinel of the callback list. */
            __stop_callback_list_entry callbacks_start;
            /* End sentinel of the callback list. */
            __stop_callback_list_entry callbacks_end;
            /* The thread that executes the callback. If the callbacks haven't been executed yet, this is empty. */
            optional<thread::id> callback_executor;

            // Can only be called by stop_source.
            __stop_state();

            void increment_ssource_refcount() noexcept;
            void decrement_ssource_refcount() noexcept;

        public:
            ~__stop_state() = default;

            void increment_refcount() noexcept;
            void decrement_refcount() noexcept;
            /* Requests a stop. Returns whether the state is already stopped. */
            bool request_stop() noexcept;
            /* Returns whether a stop has been requested. */
            bool stop_requested() const noexcept;
            /* Returns whether a stop can be made. It can only be made if a stop hasn't been made, and ssource_refcount != 0. */
            bool stop_possible() const noexcept;
            /* Registers a new callback by producing a callbacks list entry. */
            __stop_callback_list_entry* register_callback(__stop_callback_base* callback) noexcept;

            friend class std::stop_source;
            template<class Callback> friend class std::stop_callback;
        };
    }

    /* 32.3.3 Class stop_token */
    class stop_token {
    private:
        __internal::__stop_state* state;

        stop_token(__internal::__stop_state* state) noexcept;
    public:
        stop_token() noexcept;

        stop_token(const stop_token&) noexcept;
        stop_token(stop_token&&) noexcept;
        stop_token& operator=(const stop_token&) noexcept;
        stop_token& operator=(stop_token&&) noexcept;
        ~stop_token();
        void swap(stop_token&) noexcept;

        [[nodiscard]] bool stop_requested() const noexcept;
        [[nodiscard]] bool stop_possible() const noexcept;

        friend bool operator==(const stop_token& lhs, const stop_token& rhs) noexcept;

        friend class stop_source;
        template<class Callback> friend class stop_callback;
    };

    [[nodiscard]] bool operator==(const stop_token& lhs, const stop_token& rhs) noexcept;
    void swap(stop_token& lhs, stop_token& rhs) noexcept;

    /* 32.3.4 Class stop_source */
    struct nostopstate_t {
        explicit nostopstate_t() = default;
    };
    inline constexpr nostopstate_t nostopstate;

    class stop_source {
    private:
        __internal::__stop_state* state;
    public:
        stop_source();
        explicit stop_source(nostopstate_t) noexcept;

        stop_source(const stop_source&) noexcept;
        stop_source(stop_source&&) noexcept;
        stop_source& operator=(const stop_source&) noexcept;
        stop_source& operator=(stop_source&&) noexcept;
        ~stop_source();
        void swap(stop_source&) noexcept;

        [[nodiscard]] stop_token get_token() const noexcept;
        [[nodiscard]] bool stop_possible() const noexcept;
        [[nodiscard]] bool stop_requested() const noexcept;
        bool request_stop() noexcept;

        friend bool operator==(const stop_source& lhs, const stop_source &rhs) noexcept;
    };

    [[nodiscard]] bool operator==(const stop_source& lhs, const stop_source& rhs) noexcept;
    void swap(stop_source& lhs, stop_source& rhs) noexcept;

    /* 32.3.5 Class stop_callback */
    template<class Callback>
    class stop_callback : public __internal::__stop_callback_base {
    public:
        using callback_type = Callback;

        template<class C> requires invocable<C> && destructible<C> && constructible_from<Callback, C>
        explicit stop_callback(const stop_token& st, C&& cb) noexcept(is_nothrow_constructible_v<Callback, C>)
            : callback(forward<C>(cb)), state(st.state), callback_list_entry(nullptr) {
            try {
                if (st.stop_requested()) forward<C>(cb)();
                else if (st.state) {
                    state->increment_refcount();
                    callback_list_entry = state->register_callback(this);
                }
            } catch (...) {
                terminate();
            }
        }

        template<class C> requires invocable<C> && destructible<C> && constructible_from<Callback, C>
        explicit stop_callback(stop_token&& st, C&& cb) noexcept(is_nothrow_constructible_v<Callback, C>)
            : callback(forward<C>(cb)), callback_list_entry(nullptr) {
            try {
                if (st.stop_requested()) forward<C>(cb)();
                else if (st.state) {
                    state = st.state;
                    state->increment_refcount();
                    callback_list_entry = state->register_callback(this);
                }
            } catch (...) {
                terminate();
            }
        }

        ~stop_callback() {
            if (state) {
                {
                    unique_lock<std::mutex> lock(state->mtx);
                    if (callback_list_entry->destroyed) {
                        *callback_list_entry->destroyed = true;
                        if (state->callback_executor != this_thread::get_id()) {
                            callback_list_entry->executed.acquire();
                        }
                    }
                    delete callback_list_entry;
                }
                state->decrement_refcount();
            }
        }

        stop_callback(const stop_callback&) = delete;
        stop_callback(stop_callback&&) = delete;
        stop_callback& operator=(const stop_callback&) = delete;
        stop_callback& operator=(stop_callback&&) = delete;

        void execute() override { callback(); }

    private:
        Callback callback;
        __internal::__stop_state* state;
        __internal::__stop_callback_list_entry* callback_list_entry;
    };

    template<class Callback> stop_callback(stop_token, Callback) -> stop_callback<Callback>;
}