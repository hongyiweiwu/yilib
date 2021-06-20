#pragma once

#include "cstddef.hpp"
#include "type_traits.hpp"

// TODO: Currently only supports Clang's C++ ABI. 
#include <cxxabi.h>

namespace std {
    /* 17.9.3 Class exception */
    class exception {
    public:
        exception() noexcept = default;
        exception(const exception&) noexcept = default;
        exception& operator=(const exception&) noexcept = default;
        virtual ~exception();
        virtual const char* what() const noexcept;
    };

    /* 17.9.4 Class bad_exception */
    class bad_exception : public exception {
    public:
        const char* what() const noexcept override;
    };

    /* 17.9.5 Abnormal termination */
    using terminate_handler = void (*)();
    terminate_handler get_terminate() noexcept;
    terminate_handler set_terminate(terminate_handler f) noexcept;
    [[noreturn]] void terminate() noexcept;

#ifdef _LIBCPPABI_VERSION
    /* 17.9.6 uncaught_exceptions */
    int uncaught_exceptions() noexcept;

    /* 17.9.7 Exception propagation */
    class exception_ptr {
    private:
        exception_ptr(void*) noexcept;
    public:
        exception_ptr() noexcept;
        exception_ptr(nullptr_t) noexcept;
        ~exception_ptr() noexcept;

        exception_ptr(const exception_ptr& other) noexcept;
        exception_ptr& operator=(const exception_ptr& other) noexcept;

        bool operator==(const exception_ptr& other) const noexcept;
        bool operator!=(const exception_ptr& other) const noexcept;

        operator bool() const noexcept;

        [[noreturn]] void rethrow() throw();

        static exception_ptr current_exception() noexcept;
    private:
        void* ptr;
    };

    exception_ptr current_exception() noexcept;
    [[noreturn]] void rethrow_exception(exception_ptr p);
    template<class E> exception_ptr make_exception_ptr(E e) noexcept {
        try {
            throw e;
        } catch (...) {
            return current_exception();
        }
    }

    /* 17.9.8 nested_exception */
    class nested_exception {
    private:
        exception_ptr ptr;
    public:
        nested_exception() noexcept;
        nested_exception(const nested_exception&) noexcept = default;
        nested_exception& operator=(const nested_exception&) noexcept = default;
        virtual ~nested_exception() = default;

        [[noreturn]] void rethrow_nested() const;
        exception_ptr nested_ptr() const noexcept;
    };

    template<class E, class U = decay_t<E>> struct __nested : public U, nested_exception {
    public:
        inline explicit __nested(const U& u) : U(u), nested_exception() {}
    };

    template<class T> [[noreturn]] void throw_with_nested(T&& t) {
        using U = decay_t<T>;
        static_assert(is_copy_constructible_v<U>, "decay_t<T> must meet the Cpp17CopyConstructible requirements.");

        if constexpr (is_class_v<U> && !is_final_v<U> && !is_base_of_v<nested_exception, U>) {
            throw __nested(__internal::forward<T>(t));
        } else {
            throw __internal::forward<T>(t);
        }
    }

    template<class E> void rethrow_if_nested(const E& e) {
        if constexpr (is_polymorphic_v<E> && (!is_base_of_v<nested_exception, E> || is_convertible_v<E*, nested_exception*>)) {
            if (const nested_exception* p = dynamic_cast<const nested_exception*>(addressof(e))) {
                p->rethrow_nested();
            }
        }
    }
#endif
}