#pragma once

#include "compare.hpp"
#include "cstddef.hpp"
#include "functional.hpp"
#include "type_traits.hpp"
#include "tuple.hpp"
#include "utility.hpp"
#include "system_error.hpp"
#include "stop_token.hpp"
#include "ctime.hpp"
#include "memory.hpp"
#include "cerrno.hpp"

#include "pthread.h"

namespace std {
    namespace __internal {
        /* This is the callable passed into the pthread constructor. The "ptr" argument should be a pointer to a tuple containing
         * a Callable and its arguments. This function will simply call the Callable with the supplied arguments. */
        template<class ...T>
        void __thread_executor(void* ptr) {
            static constexpr auto helper = []<std::size_t ...I>(tuple<T...>* tp, index_sequence<I...>) {
                invoke(get<I>(move(*tp))...);
            };
                
            const unique_ptr<tuple<T...>> wrapped_ptr(static_cast<tuple<T...>*>(ptr));
            try {
                helper(wrapped_ptr.get(), index_sequence_for<T...>());
            } catch (...) {
                terminate();
            }
        }
    }

    // Forward declaration. Declared below.
    class jthread;

    class thread {
    public:
        class id {
        protected:
            constexpr id(pthread_t id) noexcept : identifier(id) {}
        public:
            pthread_t identifier;

            constexpr id() noexcept : identifier(0) {}

            friend bool operator==(thread::id, thread::id) noexcept;
            friend strong_ordering operator<=>(thread::id x, thread::id y) noexcept;
            friend struct hash<thread::id>;

            friend thread;
            friend jthread;
        };

        using native_handle_type = pthread_t;

        thread() noexcept;
        template<class F, class ...Args> requires (!is_same_v<remove_cvref_t<F>, thread>)
            && is_constructible_v<decay_t<F>, F> && ((is_constructible_v<decay_t<Args>, Args>) && ...)
            && is_move_constructible_v<decay_t<F>> && ((is_move_constructible_v<decay_t<Args>>) && ...)
            && is_invocable_v<decay_t<F>, decay_t<Args>...>
        explicit thread(F&& f, Args&& ...args) : handle() {
            tuple<decay_t<F>, decay_t<Args>...>* tp = new tuple<decay_t<F>, decay_t<Args>...>(__internal::decay_copy(forward<F>(f)), __internal::decay_copy(forward<Args>(args))...);
            const int ec = pthread_create(&handle, nullptr, &__internal::__thread_executor<decay_t<F>, decay_t<Args>...>, tp);
            if (ec != 0) {
                throw system_error(ec, system_category());
                delete tp;
            }
        }
        
        ~thread();
        thread(const thread&) = delete;
        thread(thread&&) noexcept;
        thread& operator=(const thread&) = delete;
        thread& operator=(thread&&) noexcept;

        void swap(thread&) noexcept;  
        bool joinable() const noexcept;
        void join();
        void detach();
        id get_id() const noexcept;
        native_handle_type native_handle();

        static unsigned int hardware_concurrency() noexcept;

    protected:
        pthread_t handle;
    };

    bool operator==(thread::id, thread::id) noexcept;
    strong_ordering operator<=>(thread::id x, thread::id y) noexcept;

    /* // TODO Uncomment after iostream is finished.
    template<class charT, class traits>
    basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& out, thread::id id); */

    template<> struct hash<thread::id> : hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const thread::id& id) const;
    };

    void swap(thread& x, thread& y) noexcept;

    class jthread {
    public:
        using id = thread::id;
        using native_handle_type = thread::native_handle_type;

        jthread() noexcept;
        template<class F, class ...Args> requires (!is_same_v<remove_cvref_t<F>, jthread>)
            && is_constructible_v<decay_t<F>, F> && ((is_constructible_v<decay_t<Args>, Args>) && ...)
            && is_move_constructible_v<decay_t<F>> && ((is_move_constructible_v<decay_t<Args>>) && ...)
            && (is_invocable_v<decay_t<F>, decay_t<Args>...> || is_invocable_v<decay_t<F>, stop_token, decay_t<Args>...>)
        explicit jthread(F&& f, Args&& ...args) : handle(), ssource() {
            int ec;
            if constexpr (is_invocable_v<decay_t<F>, stop_token, decay_t<Args>...>) {
                tuple<decay_t<F>, stop_token, decay_t<Args>...>* tp = new tuple<decay_t<F>, stop_token, decay_t<Args>...>(__internal::decay_copy(forward<F>(f)), get_stop_token(), __internal::decay_copy(forward<Args>(args))...);
                ec = pthread_create(&handle, nullptr, &__internal::__thread_executor<decay_t<F>, stop_token, decay_t<Args>...>, tp);
                if (ec != 0) {
                    delete tp;
                    throw system_error(ec, system_category());
                }
            } else {
                tuple<decay_t<F>, decay_t<Args>...>* tp = new tuple<decay_t<F>, decay_t<Args>...>(__internal::decay_copy(forward<F>(f)), __internal::decay_copy(forward<Args>(args))...);
                ec = pthread_create(&handle, nullptr, &__internal::__thread_executor<decay_t<F>, decay_t<Args>...>, tp);
                if (ec != 0) {
                    delete tp;
                    throw system_error(ec, system_category());
                }
            }
        }
        
        ~jthread();
        jthread(const jthread&) = delete;
        jthread(jthread&&) noexcept;
        jthread& operator=(const jthread&) = delete;
        jthread& operator=(jthread&&) noexcept;

        void swap(jthread&) noexcept;
        [[nodiscard]] bool joinable() const noexcept;
        void join();
        void detach();
        [[nodiscard]] id get_id() const noexcept;
        [[nodiscard]] native_handle_type native_handle();

        [[nodiscard]] stop_source get_stop_source() noexcept;
        [[nodiscard]] stop_token get_stop_token() noexcept;
        bool request_stop() noexcept;

        friend void swap(jthread& lhs, jthread& rhs) noexcept;

        [[nodiscard]] static unsigned int hardware_concurrency() noexcept;
    
    private:
        pthread_t handle;
        stop_source ssource;
    };

    void swap(jthread& x, jthread& y) noexcept;

    namespace this_thread {
        thread::id get_id() noexcept;
        void yield() noexcept;
        
        template<class Clock, class Duration>
        void sleep_until(const chrono::time_point<Clock, Duration>& abs_time) {
            return sleep_for(abs_time - chrono::steady_clock::now());
        }

        template<class Rep, class Period>
        void sleep_for(const chrono::duration<Rep, Period>& rel_time) {
            const chrono::seconds sec = chrono::duration_cast<chrono::seconds>(rel_time);
            std::timespec rel_time_spec = { 
                .tv_sec = sec.count(), 
                .tv_nsec = chrono::duration_cast<chrono::microseconds>(rel_time - sec).count()
            };

            while (true) {
                const int res = nanosleep(&rel_time_spec, &rel_time_spec);
                if (res == 0) {
                    break;
                } else if (res == -1 && errno != EINTR) {
                    throw system_error(errno, system_category());
                }
            }
        }
    }
}