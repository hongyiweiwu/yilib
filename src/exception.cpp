#include "exception.hpp"
#include "cstdlib.hpp"

#include <stdatomic.h>

#include <cxxabi.h>

namespace std {
    exception::~exception() {}
    const char* exception::what() const noexcept { return "std::exception"; }

    const char* bad_exception::what() const noexcept { return "std::bad_exception"; }

    static constinit _Atomic(terminate_handler) __curr_terminate_handler;

    terminate_handler get_terminate() noexcept {
        return atomic_load(&__curr_terminate_handler);
    }

    terminate_handler set_terminate(terminate_handler f) noexcept {
        return atomic_exchange(&__curr_terminate_handler, f);
    }

    [[noreturn]] void terminate() noexcept {
        try {
            (*get_terminate())();
            abort();
        } catch (...) {
            abort();
        }
    }

    int uncaught_exceptions() noexcept {
        return __cxxabiv1::__cxa_uncaught_exceptions();
    }

    exception_ptr::exception_ptr(void* ptr) noexcept : ptr(ptr) {}
    exception_ptr::exception_ptr() noexcept : ptr(nullptr) {}
    exception_ptr::exception_ptr(nullptr_t) noexcept : exception_ptr() {}
    exception_ptr::~exception_ptr() noexcept {
        __cxxabiv1::__cxa_decrement_exception_refcount(ptr);
    }

    exception_ptr::exception_ptr(const exception_ptr& other) noexcept : ptr(other.ptr) {
        __cxxabiv1::__cxa_increment_exception_refcount(other.ptr);
    }

    exception_ptr& exception_ptr::operator=(const exception_ptr& other) noexcept {
        __cxxabiv1::__cxa_decrement_exception_refcount(ptr);
        ptr = other.ptr;
        __cxxabiv1::__cxa_increment_exception_refcount(ptr);
        return *this;
    }

    bool exception_ptr::operator==(const exception_ptr& other) const noexcept {
        return ptr == other.ptr;
    }

    bool exception_ptr::operator!=(const exception_ptr& other) const noexcept {
        return ptr != other.ptr;
    }

    exception_ptr::operator bool() const noexcept {
        return ptr != nullptr;
    }

    [[noreturn]] void exception_ptr::rethrow() throw() {
        __cxxabiv1::__cxa_rethrow_primary_exception(ptr);
        terminate();
    }

    exception_ptr exception_ptr::current_exception() noexcept {
        return exception_ptr(__cxxabiv1::__cxa_current_primary_exception());
    }

    exception_ptr current_exception() noexcept {
        return exception_ptr::current_exception();
    }

    [[noreturn]] void rethrow_exception(exception_ptr p) {
        p.rethrow();
    }

    nested_exception::nested_exception() noexcept : ptr(current_exception()) {}
    
    [[noreturn]] void nested_exception::rethrow_nested() const {
        if (nested_ptr() != nullptr) {
            rethrow_exception(ptr);
        } else {
            terminate();
        }
    }

    exception_ptr nested_exception::nested_ptr() const noexcept {
        return ptr;
    }
}