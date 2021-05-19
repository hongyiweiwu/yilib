#pragma once

#include "utility.hpp"

namespace std {
    /* 20.14.8 Comparisons */
    template<class T = void> struct less {
        constexpr bool operator()(const T& x, const T& y) const { return x < y; }
    };

    template<> struct less<void> {
        template<class T, class U> constexpr auto operator()(T&& t, U&& u) const {
            return forward<T>(t) < forward<U>(u);
        }
    };

    /* 20.14.19 Class template hash */
    namespace __internal {
        /* Used purely to specify a specialization of std::hash that represents when std::hash is supposed to be deleted. All deleted std::hash specializations
         * should inherit from std::hash<__delete_hash_t> and automatically receive all the deleted std::hash properties. */
        struct __deleted_hash_t {
            explicit __deleted_hash_t() = default;
        };
    }

    // Forward declaration.
    template<class T> struct hash;

    template<> struct hash<__internal::__deleted_hash_t> {
        hash() = delete;
        hash(hash&&) = delete;
        hash& operator=(hash&&) = delete;
        hash(const hash&) = delete;
        hash& operator=(const hash&) = delete;
    };

    template<class T> struct hash : hash<__internal::__deleted_hash_t> {};
}