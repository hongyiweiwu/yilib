#pragma once

#include "system_error.hpp"
#include "type_traits.hpp"
#include "stdexcept.hpp"
#include "memory/construct_destroy.hpp"
#include "exception.hpp"
#include "mutex.hpp"
#include "condition_variable.hpp"
#include "utility.hpp"
#include "util/at_thread_exits.hpp"
#include "thread.hpp"

namespace std {
    enum class future_errc {
        broken_promise = 1,
        future_already_retrieved = 2,
        promise_already_satisfied = 3,
        no_state = 4,
    };

    enum class launch : unsigned int {
        async = 0b1,
        deferred = 0b10,
    };

    enum class future_status {
        ready, timeout, deferred,
    };

    template<>
    struct is_error_code_enum<future_errc> : public true_type {};

    error_code make_error_code(future_errc e) noexcept;
    error_condition make_error_condition(future_errc e) noexcept;

    const error_category& future_category() noexcept;

    class future_error : public logic_error {
    private:
        error_code ec;
    public:
        explicit future_error(future_errc e);

        const error_code& code() const noexcept;
        const char* what() const noexcept;
    };

    // Forward declaration, defined below.
    template<class>
    class future;

    template<class>
    class packaged_task;

    namespace __internal {
        /* The shared state that connects promises and futures that they create. */
        template<class R>
        struct __promise_state {
            aligned_union_t<0, R, exception_ptr> storage;

            enum status_t {
                error, success, processing
            } state = processing;

            bool stored = false;
            bool retrieved = false;
            std::mutex lock;
            condition_variable cv;

            /* A function that should be invoked at the beginning of every `wait()` call for every `future` that shares ownership of
             * this state. The default implementation doesn't do anything. */
            constexpr virtual void do_on_wait() const noexcept {}

            constexpr bool ready() const noexcept {
                return state != processing;
            }

            template<class T, class... Args>
            constexpr void emplace(Args&&... args) {
                construct_at(static_cast<T*>(&storage), forward<Args>(args)...);
            }

            virtual ~__promise_state() = default;
        };

        /* Base of `promise` that all its three specializations inherit from. */
        template<class R>
        struct __promise_base {
        protected:
            using state_t = __internal::__promise_state<R>;
            shared_ptr<state_t> state;

        public:
            __promise_base() : state(make_shared<state_t>()) {}

            template<class Allocator>
            __promise_base(allocator_arg_t, const Allocator& a) : state(allocate_shared<state_t>(a)) {}

            __promise_base(__promise_base&& rhs) noexcept : state(move(rhs).state) {}

            __promise_base(const __promise_base&) = delete;

            ~__promise_base() {
                if (!state) {
                    return;
                } else {
                    const lock_guard<std::mutex> lock(state->lock);
                    if (!state->ready() && !state->stored) {
                        state->template emplace<exception_ptr>(make_exception_ptr(future_error(future_errc::broken_promise)));
                        state->state = state_t::error;
                        state->stored = true;
                        state->cv.notify_all();
                    }
                }

                state.reset();
            }

            __promise_base& operator=(__promise_base&& rhs) noexcept {
                ~__promise_base();
                state = move(rhs).state;
                return *this;
            }

            __promise_base& operator=(const __promise_base&) = delete;

            void swap(__promise_base& other) noexcept {
                using std::swap;
                swap(state, other.state);
            }

            future<R> get_future() {
                if (!state) {
                    throw future_error(future_errc::no_state);
                }

                const lock_guard<std::mutex> lock(state->lock);
                if (state->retrieved) {
                    throw future_error(future_errc::future_already_retrieved);
                } else {
                    state->retrieved = true;
                    return future<R>(state);
                }
            }

            void set_exception(exception_ptr p) {
                if (!state) {
                    throw future_error(future_errc::no_state);
                }
                const lock_guard<std::mutex> lock(state->lock);
                if (state->ready() || state->stored) {
                    throw future_error(future_errc::promise_already_satisfied);
                }

                state->template emplace<exception_ptr>(p);
                state->state = state_t::error;
                state->stored = true;
                state->cv.notify_all();
            }

        protected:
            template<typename state_t::status_t status>
            struct set_at_thread_exit_fn_block : public __internal::at_thread_exit_fn_block {
                shared_ptr<state_t> state;

                set_at_thread_exit_fn_block(const shared_ptr<state_t>& state)
                    : __internal::at_thread_exit_fn_block({.callback = callback, .next = nullptr}), state(state) {}

                static void callback(void* p) {
                    set_at_thread_exit_fn_block* const ptr = static_cast<set_at_thread_exit_fn_block*>(p);
                    {
                        const lock_guard<std::mutex> lock(ptr->state->lock);
                        ptr->state->state = status;
                        ptr->state->cv.notify_all();
                    }
                    delete ptr;
                }
            };
        public:
            void set_exception_at_thread_exit(exception_ptr p) {
                if (!state) {
                    throw future_error(future_errc::no_state);
                }

                __internal::at_thread_exit_fn_block::initialize_key();
                {
                    const lock_guard<std::mutex> lock(state->lock);
                    if (state->ready() || state->stored) {
                        throw future_error(future_errc::promise_already_satisfied);
                    }

                    state->template emplace<exception_ptr>(p);
                    state->stored = true;
                }
                __internal::at_thread_exit_fn_block::push_front(new set_at_thread_exit_fn_block<state_t::error>(state));
            }

            template<class>
            friend class packaged_task;
        };
    }

    template<class R>
    class promise : public __internal::__promise_base<R> {
    public:
        using __internal::__promise_base<R>::__promise_base;
        using __internal::__promise_base<R>::operator=;

        void set_value(const R& r) {
            if (!this->state) {
                throw future_error(future_errc::no_state);
            }
            const lock_guard<mutex> lock(this->state->lock);
            if (this->state->ready() || this->state->stored) {
                throw future_error(future_errc::promise_already_satisfied);
            }
            this->state->template emplace<R>(r);
            this->state->stored = true;
            this->state->state = __internal::__promise_base<R>::state_t::success;
            this->state->cv.notify_all();
        }

        void set_value(R&& r) {
            if (!this->state) {
                throw future_error(future_errc::no_state);
            }
            const lock_guard<mutex> lock(this->state->lock);
            if (this->state->ready() || this->state->stored) {
                throw future_error(future_errc::promise_already_satisfied);
            }
            this->state->template emplace<R>(move(r));
            this->state->state = __internal::__promise_base<R>::state_t::success;
            this->state->stored = true;
            this->state->cv.notify_all();
        }

        void set_value_at_thread_exit(const R& r) {
            if (!this->state) {
                throw future_error(future_errc::no_state);
            }
            __internal::at_thread_exit_fn_block::initialize_key();
            {
                const lock_guard<mutex> lock(this->state->lock);
                if (this->state->ready() || this->state->stored) {
                    throw future_error(future_errc::promise_already_satisfied);
                }
                this->state->template emplace<R>(r);
                this->state->stored = true;
            }
            __internal::at_thread_exit_fn_block::push_front(
                new typename __internal::__promise_base<R>::template set_at_thread_exit_fn_block<
                    __internal::__promise_base<R>::state_t::success>(this->state));
        }

        void set_value_at_thread_exit(R&& r) {
            if (!this->state) {
                throw future_error(future_errc::no_state);
            }

            __internal::at_thread_exit_fn_block::initialize_key();
            {
                const lock_guard<mutex> lock(this->state->lock);
                if (this->state->ready() || this->state->stored) {
                    throw future_error(future_errc::promise_already_satisfied);
                }
                this->state->template emplace<R>(move(r));
                this->state->stored = true;
            }

            __internal::at_thread_exit_fn_block::push_front(
                new typename __internal::__promise_base<R>::template set_at_thread_exit_fn_block<
                    __internal::__promise_base<R>::state_t::success>(this->state));
        }
    };

    template<class R>
    struct promise<R&> : public __internal::__promise_base<R&> {
    public:
        using __internal::__promise_base<R&>::__promise_base;
        using __internal::__promise_base<R&>::operator=;

        void set_value(R& r) {
            if (!this->state) {
                throw future_error(future_errc::no_state);
            }
            const lock_guard<mutex> lock(this->state->lock);
            if (this->state->ready() || this->state->stored) {
                throw future_error(future_errc::promise_already_satisfied);
            }
            this->state->template emplace<R&>(r);
            this->state->state = __internal::__promise_base<R>::state_t::success;
            this->state->stored = true;
            this->state->cv.notify_all();
        }

        void set_value_at_thread_exit(R& r) {
            if (!this->state) {
                throw future_error(future_errc::no_state);
            }
            __internal::at_thread_exit_fn_block::initialize_key();
            {
                const lock_guard<mutex> lock(this->state->lock);
                if (this->state->ready() || this->state->stored) {
                    throw future_error(future_errc::promise_already_satisfied);
                }
                this->state->template emplace<R&>(r);
                this->state->stored = true;
            }
            __internal::at_thread_exit_fn_block::push_front(
                new typename __internal::__promise_base<R>::template set_at_thread_exit_fn_block<
                    __internal::__promise_base<R>::state_t::success>(this->state));
        }
    };

    template<>
    struct promise<void> : public __internal::__promise_base<char> {
    public:
        using __internal::__promise_base<char>::__promise_base;
        using __internal::__promise_base<char>::operator=;

        void set_value();
        void set_value_at_thread_exit();
    };

    template<class R>
    void swap(promise<R>& x, promise<R>& y) noexcept {
        x.swap(y);
    }

    template<class R, class Alloc>
    struct uses_allocator<promise<R>, Alloc> : true_type {};

    // Forward declaration, defined below.
    template<class R>
    class shared_future;

    namespace __internal {
        template<class R>
        class __future_base {
        protected:
            shared_ptr<__internal::__promise_state<R>> state;

            __future_base(const shared_ptr<__internal::__promise_state<R>>& state) noexcept : state(state) {}
        public:
            __future_base() noexcept : state() {}
            __future_base(__future_base&& rhs) noexcept : state(move(rhs).state) {}
            __future_base(const __future_base&) = delete;
            ~__future_base() = default;

            __future_base& operator=(const __future_base&) = delete;
            __future_base& operator=(__future_base&& rhs) noexcept {
                state = move(rhs).state;
                return *this;
            }

            shared_future<R> share() noexcept {
                return shared_future(state);
            }

            bool valid() const noexcept {
                return state;
            }

            void wait() const {
                state->do_on_wait();
                unique_lock<std::mutex> lock(state->lock);
                state->cv.wait(lock, [this] {
                    return state->ready();
                });
            }

            template<class Rep, class Period>
            future_status wait_for(const chrono::duration<Rep, Period>& rel_time) const {
                state->do_on_wait();
                unique_lock<std::mutex> lock(state->lock);
                const bool is_not_timeout = state->cv.wait_for(lock, rel_time, [this] {
                    return state->ready();
                });

                return is_not_timeout ? future_status::ready : future_status::timeout;
            }

            template<class Clock, class Duration>
            future_status wait_until(const chrono::time_point<Clock, Duration>& abs_time) const {
                state->do_on_wait();
                unique_lock<std::mutex> lock(state->lock);
                const bool is_not_timeout = state->cv.wait_until(lock, abs_time, [this] {
                    return state->ready();
                });

                return is_not_timeout ? future_status::ready : future_status::timeout;
            }

            friend struct __internal::__promise_base<R>;
        };
    }

    template<class R>
    class future : public __internal::__future_base<R> {
    public:
        using __internal::__future_base<R>::__future_base;
        using __internal::__future_base<R>::operator=;

        R get() {
            aligned_union_t<0, R, exception_ptr> storage;
            bool success = false;

            {
                unique_lock<std::mutex> lock(this->state->lock);
                this->state->cv.wait(lock, [this] {
                    return this->state->ready();
                });

                if (this->state->state == __internal::__promise_state<R>::success) {
                    new (&storage) R(move(*reinterpret_cast<R*>(&this->state->storage)));
                    success = true;
                } else {
                    new (&storage) exception_ptr(*reinterpret_cast<exception_ptr*>(&this->state->storage));
                }
            }

            this->state.reset();
            if (success) {
                return move(*reinterpret_cast<R*>(&storage));
            } else {
                rethrow_exception(*reinterpret_cast<exception_ptr*>(&storage));
            }
        }
    };

    template<class R>
    class future<R&> : public __internal::__future_base<R&> {
    public:
        using __internal::__future_base<R&>::__future_base;
        using __internal::__future_base<R&>::operator=;

        R& get() {
            aligned_union_t<0, reference_wrapper<R>, exception_ptr> storage;
            bool success = false;

            {
                unique_lock<std::mutex> lock(this->state->lock);
                this->state->cv.wait(lock, [this] {
                    return this->state->ready();
                });

                if (this->state->state == __internal::__promise_state<R>::success) {
                    new (&storage) reference_wrapper(*reinterpret_cast<R*>(&this->state->storage));
                    success = true;
                } else {
                    new (&storage) exception_ptr(*reinterpret_cast<exception_ptr*>(&this->state->storage));
                }
            }

            this->state.reset();
            if (success) {
                return reinterpret_cast<reference_wrapper<R>*>(&storage)->get();
            } else {
                rethrow_exception(*reinterpret_cast<exception_ptr*>(&storage));
            }
        }
    };

    template<>
    class future<void> : public __internal::__future_base<char> {
    public:
        using __internal::__future_base<char>::__future_base;
        using __internal::__future_base<char>::operator=;

        void get();
    };

    template<class R>
    class shared_future : public future<R> {
    public:
        using future<R>::future;
        using future<R>::operator=;

        shared_future(const shared_future& rhs) noexcept : future<R>(rhs.state) {}
        shared_future& operator=(const shared_future& rhs) noexcept {
            this->state = rhs.state;
            return *this;
        }

        R get() = delete;
        const R& get() const {
            aligned_union_t<0, R, exception_ptr> storage;
            bool success = false;

            {
                unique_lock<std::mutex> lock(this->state->lock);
                this->state->cv.wait(lock, [this] {
                    return this->state->ready();
                });

                if (this->state->state == __internal::__promise_state<R>::success) {
                    new (&storage) R(*reinterpret_cast<R*>(&this->state->storage));
                    success = true;
                } else {
                    new (&storage) exception_ptr(*reinterpret_cast<exception_ptr*>(&this->state->storage));
                }
            }

            this->state.reset();
            if (success) {
                return *reinterpret_cast<R*>(&storage);
            } else {
                rethrow_exception(*reinterpret_cast<exception_ptr*>(&storage));
            }
        }
    };

    template<class R>
    class shared_future<R&> : public future<R&> {
    public:
        using future<R>::future;
        using future<R>::operator=;

        shared_future(const shared_future& rhs) noexcept : future<R&>(rhs.state) {}
        shared_future& operator=(const shared_future& rhs) noexcept {
            this->state = rhs.state;
            return *this;
        }

        R& get() const {
            return const_cast<shared_future<R&>*>(this)->future<void>::get();
        }
    };

    template<>
    class shared_future<void> : public future<void> {
    public:
        using future<void>::future;
        using future<void>::operator=;

        shared_future(const shared_future& rhs) noexcept : future<void>(rhs.state) {}
        shared_future& operator=(const shared_future& rhs) noexcept {
            this->state = rhs.state;
            return *this;
        }

        void get() const {
            return const_cast<shared_future<void>*>(this)->future<void>::get();
        }
    };

    template<class>
    class packaged_task;

    template<class R, class ...ArgTypes>
    class packaged_task<R(ArgTypes...)> {
    private:
        /* Base class of the storage of the function to be executed. This class doesn't actually store the function, nor is it aware of the type of the
         * function being stored. */
        struct fn_storage_base {
            virtual R operator()(ArgTypes...) = 0;
            virtual ~fn_storage_base() = default;
        };

        unique_ptr<fn_storage_base> storage;
        promise<R> p;
        bool invoked = false;

        template<class F>
        struct fn_storage : public fn_storage_base {
            F fn;

            R operator()(ArgTypes ...args) override {
                return invoke(forward<F>(fn), forward<ArgTypes>(args)...);
            }
        };

        using state_t = __internal::__promise_state<R>;

    public:
        packaged_task() noexcept = default;

        template<class F>
        explicit packaged_task(F&& f) : storage(make_shared<fn_storage_base>(fn_storage<F>{forward<F>(f)})), p() {}

        ~packaged_task() = default;

        packaged_task(const packaged_task&) = delete;
        packaged_task& operator=(const packaged_task&) = delete;

        packaged_task(packaged_task&& rhs) noexcept : storage(move(rhs).storage), p(move(rhs).promise) {}

        packaged_task& operator=(packaged_task&& rhs) noexcept = default;

        void swap(packaged_task& other) noexcept {
            using std::swap;
            swap(storage, other.storage);
            swap(p, other.promise);
        }

        bool valid() const noexcept {
            return p.valid();
        }

        future<R> get_future() {
            return p.get_future();
        }

        void operator()(ArgTypes ...args) {
            if (invoked) {
                throw future_error(future_errc::promise_already_satisfied);
            } else if (!p.valid()) {
                throw future_error(future_errc::no_state);
            }

            invoked = true;

            try {
                if constexpr (is_void_v<R>) {
                    storage->invoke(forward<ArgTypes>(args)...);
                    p.set_value();
                } else {
                    p.set_value(storage->invoke(forward<ArgTypes>(args)...));
                }
            } catch (...) {
                p.set_exception(current_exception());
            }
        }

    public:
        void make_ready_at_thread_exit(ArgTypes ...args) {
            if (invoked) {
                throw future_error(future_errc::promise_already_satisfied);
            } else if (!p.valid()) {
                throw future_error(future_errc::no_state);
            }

            invoked = true;

            try {
                if constexpr (is_void_v<R>) {
                    storage->invoke(forward<ArgTypes>(args)...);
                    p.set_value_at_thread_exit();
                } else {
                    p.set_value_at_thread_exit(storage->invoke(forward<ArgTypes>(args)...));
                }
            } catch (...) {
                p.set_exception_at_thread_exit(current_exception());
            }
        }

        void reset() {
            if (!p.valid()) {
                throw future_error(future_errc::no_state);
            } else {
                p = std::promise<R>();
            }
        }
    };

    template<class R, class ...ArgTypes>
    void swap(packaged_task<R(ArgTypes...)>& x, packaged_task<R(ArgTypes...)>& y) noexcept {
        x.swap(y);
    }

    template<class F, class ...Args>
    requires is_constructible_v<decay_t<F>, F> && (is_constructible_v<decay_t<Args>, Args> && ...)
        && is_move_constructible_v<decay_t<F>> && (is_move_constructible_v<decay_t<Args>> && ...)
        && is_invocable_v<decay_t<F>, decay_t<Args>...>
    [[nodiscard]] future<invoke_result_t<decay_t<F>, decay_t<Args>...>> async(F&& f, Args&& ...args) {
        return async(static_cast<launch>(0b11), forward<F>(f), forward<Args>(args)...);
    }

    template<class F, class ...Args>
    requires is_constructible_v<decay_t<F>, F> && (is_constructible_v<decay_t<Args>, Args> && ...)
        && is_move_constructible_v<decay_t<F>> && (is_move_constructible_v<decay_t<Args>> && ...)
        && is_invocable_v<decay_t<F>, decay_t<Args>...>
    [[nodiscard]] future<invoke_result_t<decay_t<F>, decay_t<Args>...>> async(launch policy, F&& f, Args&& ...args) {
        using return_t = invoke_result_t<decay_t<F>, decay_t<Args>...>;

        switch (static_cast<unsigned int>(policy)) {
            case static_cast<unsigned int>(launch::async): {
                shared_ptr<promise<return_t>> p = make_shared<promise<return_t>>();
                const future<return_t> future = p->get_future();
                try {
                    // This outer try block captures exceptions thrown during decay_copy.
                    thread([p](F&& f, Args&& ...args) {
                        try {
                            // This inner try block captures exceptions thrown during the function execution.
                            if constexpr (is_void_v<return_t>) {
                                invoke(forward<F>(f), forward<Args>(args)...);
                                p->set_value();
                            } else {
                                p->set_value(invoke(forward<F>(f), forward<Args>(args)...));
                            }
                        } catch (...) {
                            p->set_exception(current_exception());
                        }
                    }, forward<F>(f), forward<Args>(args)...);
                } catch (...) {
                    p->set_exception(current_exception());
                }

                return future;
            }

            case static_cast<unsigned int>(launch::deferred):
                /* A wrapper around the normal `__promise_state` that overrides its `do_on_wait` method to invoke the deferred function call. */
                struct deferred_promise_state : public __internal::__promise_state<return_t> {
                    promise<return_t> p;
                    tuple<decay_t<F>, decay_t<Args>...> callable;

                    deferred_promise_state(F&& f, Args&& ...args)
                        : __internal::__promise_state<return_t>(), callable(make_tuple(forward<F>(f), forward<Args>(args)...)), p(shared_ptr(this)) {}

                    void do_on_wait() noexcept override {
                        static constinit once_flag once_guard;
                        call_once(once_guard, [this] {
                            try {
                                if constexpr (is_void_v<return_t>) {
                                    invoke<decay_t<F>, decay_t<Args>...>, move(*callable);
                                    p.set_value();
                                } else {
                                    p.set_value(invoke<decay_t<F>, decay_t<Args>...>, move(*callable));
                                }
                            } catch (...) {
                                p.set_exception(current_exception());
                            }
                        });
                    }
                };

                /* Encapsulates the shared logic between the `launch::deferred` and `launch(0b11)` branch. The type parameter must be `deferred_promise_state` if the former, or `deferred_async_promise_state`
                 * if the latter. */
                static constexpr auto setup_deferred = []<class T>(F&& f, Args&& ...args) {
                    T* const state_ptr = operator new(sizeof(T), align_val_t(alignof(T)));
                    try {
                        new (state_ptr) T(forward<F>(f), forward<Args>(args)...);
                        return state_ptr->p.get_future();
                    } catch (...) {
                        operator delete(state_ptr);

                        promise<return_t> p;
                        p.set_exception(current_exception());
                        return p.get_future();
                    }
                };

                return setup_deferred.template operator()<deferred_promise_state>(forward<F>(f), forward<Args>(args)...);

            case 0b11:  // launch::deferred | launch::async
                struct deferred_async_promise_state : public deferred_promise_state {
                    using deferred_promise_state::deferred_promise_state;

                    void do_on_wait() noexcept override {
                        try {
                            thread([this] {
                                this->deferred_promise_state::do_on_wait();
                            });
                        } catch (...) {
                            this->p.set_exception(current_exception());
                        }
                    }
                };

                return setup_deferred.template operator()<deferred_async_promise_state>(forward<F>(f), forward<Args>(args)...);
        }
    }
}