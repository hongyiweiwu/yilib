#pragma once

#include "limits.hpp"
#include "new.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "memory/pointer_util.hpp"
#include "memory/specialized_algorithms.hpp"
#include "cstddef.hpp"

namespace std {
    /* 20.10.9 Allocator traits */
    template<class Alloc>
    requires requires { typename Alloc::value_type; }
    struct allocator_traits {
    private:
        // TODO: Clang bug prevents these functions from being consteval. After Clang fixes this, change modifiers to consteval.
        static constexpr auto get_pointer_type() noexcept {
            if constexpr (requires { typename Alloc::pointer; }) {
                return declval<typename Alloc::pointer>();
            } else {
                return declval<typename Alloc::value_type*>();
            }
        }

        static constexpr auto get_const_pointer_type() noexcept {
            if constexpr (requires { typename Alloc::const_pointer; }) {
                return declval<typename Alloc::const_pointer>();
            } else {
                using pointer = decltype(get_pointer_type());
                using value_type = typename Alloc::value_type;
                return declval<typename pointer_traits<pointer>::template rebind<const value_type>>();
            }
        }

        static constexpr auto get_void_pointer_type() noexcept {
            if constexpr (requires { typename Alloc::void_pointer; }) {
                return declval<typename Alloc::void_pointer>();
            } else {
                using pointer = decltype(get_pointer_type());
                return declval<typename pointer_traits<pointer>::template rebind<void>>();
            }
        }

        static constexpr auto get_const_void_pointer_type() noexcept {
            if constexpr (requires { typename Alloc::const_void_pointer; }) {
                return declval<typename Alloc::const_void_pointer>();
            } else {
                using pointer = decltype(get_pointer_type());
                return declval<typename pointer_traits<pointer>::template rebind<const void>>();
            }
        }

        static constexpr auto get_difference_type() noexcept {
            if constexpr (requires { typename Alloc::difference_type; }) {
                return declval<typename Alloc::difference_type>();
            } else {
                using pointer = decltype(get_pointer_type());
                return declval<typename pointer_traits<pointer>::difference_type>();
            }
        }

        static constexpr auto get_size_type() noexcept {
            if constexpr (requires { typename Alloc::size_type; }) {
                return declval<typename Alloc::size_type>();
            } else {
                return declval<make_unsigned_t<decltype(get_difference_type())>>();
            }
        }

        static constexpr auto get_propagate_on_container_copy_assignment() noexcept {
            if constexpr (requires { typename Alloc::propagate_on_container_copy_assignment; }) {
                return declval<typename Alloc::propagate_on_container_copy_assignment>();
            } else {
                return false_type();
            }
        }

        static constexpr auto get_propagate_on_container_move_assignment() noexcept {
            if constexpr (requires { typename Alloc::propagate_on_container_move_assignment; }) {
                return declval<typename Alloc::propagate_on_container_move_assignment>();
            } else {
                return false_type();
            }
        }

        static constexpr auto get_propagate_on_container_swap() noexcept {
            if constexpr (requires { typename Alloc::propagate_on_container_swap; }) {
                return declval<typename Alloc::propagate_on_container_swap>();
            } else {
                return false_type();
            }
        }

        static constexpr auto get_is_always_equal() noexcept {
            if constexpr (requires { typename Alloc::is_always_equal; }) {
                return declval<typename Alloc::is_always_equal>();
            } else {
                return false_type();
            }
        }

        template<class Allocator, class T>
        struct get_rebind_alloc {};

        template<class Allocator, class T>
        requires requires { typename Allocator::template rebind<T>::other; }
        struct get_rebind_alloc<Allocator, T> {
            using type = typename Allocator::template rebind<T>::other;
        };

        template<template<class, class...> class Allocator, class T, class U, class ...Args>
        struct get_rebind_alloc<Allocator<U, Args...>, T> {
            using type = Allocator<T, Args...>;
        };
    public:
        using allocator_type = Alloc;

        using value_type = typename Alloc::value_type;

        using pointer = decltype(get_pointer_type());
        using const_pointer = decltype(get_const_pointer_type());
        using void_pointer = decltype(get_void_pointer_type());
        using const_void_pointer = decltype(get_const_void_pointer_type());

        using difference_type = decltype(get_difference_type());
        using size_type = decltype(get_size_type());

        using propagate_on_container_copy_assignment = decltype(get_propagate_on_container_copy_assignment());
        using propagate_on_container_move_assignment = decltype(get_propagate_on_container_move_assignment());
        using propagate_on_container_swap = decltype(get_propagate_on_container_swap());
        using is_always_equal = decltype(get_is_always_equal());

        template<class T>
        using rebind_alloc = typename get_rebind_alloc<Alloc, T>::type;
        template<class T>
        using rebind_traits = allocator_traits<rebind_alloc<T>>;

        [[nodiscard]] static constexpr pointer allocate(Alloc& a, size_type n)
        requires requires { a.allocate(n); } {
            return a.allocate(n);
        }

        [[nodiscard]] static constexpr pointer allocate(Alloc& a, size_type n, const_void_pointer hint)
        requires requires { a.allocate(n); } || requires { a.allocate(n, hint); } {
            if constexpr (requires { a.allocate(n, hint); }) {
                return a.allocate(n, hint);
            } else {
                return a.allocate(n);
            }
        }

        static constexpr void deallocate(Alloc& a, pointer p, size_type n)
        requires requires { a.deallocate(p, n); }  {
            a.deallocate(p, n);
        }

        template<class T, class ...Args>
        static constexpr void construct(Alloc& a, T* p, Args&& ...args)
        requires requires { a.construct(p, forward<Args>(args)...); } || requires { construct_at(p, forward<Args>(args)...); } {
            if constexpr (requires { a.construct(p, forward<Args>(args)...); }) {
                a.construct(p, forward<Args>(args)...);
            } else {
                construct_at(p, forward<Args>(args)...);
            }
        }

        template<class T>
        static constexpr void destroy(Alloc& a, T* p)
        requires requires { a.destroy(p); } || requires { destroy_at(p); } {
            if constexpr (requires { a.destroy(p); }) {
                a.destroy(p);
            } else {
                destroy_at(p);
            }
        }

        static constexpr size_type max_size(const Alloc& a) noexcept {
            if constexpr (requires { a.maxsize(); }) {
                return a.max_size();
            } else {
                return numeric_limits<size_type>::max() / sizeof(value_type);
            }
        }

        static constexpr Alloc select_on_container_copy_construction(const Alloc& rhs) {
            if constexpr (requires { rhs.select_on_container_copy_construction(); }) {
                return rhs.select_on_container_copy_construction();
            } else {
                return rhs;
            }
        }
    };

    /* 20.10.10 Default allocator */
    template<class T>
    class allocator {
    public:
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using propagate_on_container_move_assignment = true_type;
        using is_always_equal = true_type;

        constexpr allocator() noexcept = default;
        constexpr allocator(const allocator&) noexcept = default;

        template<class U>
        constexpr allocator(const allocator<U>&) noexcept : allocator() {}

        constexpr ~allocator() = default;
        constexpr allocator& operator=(const allocator&) = default;

        [[nodiscard]] constexpr T* allocate(std::size_t n)
        requires __internal::is_complete<T>::value {
            if (numeric_limits<std::size_t>::max() / sizeof(T) < n) {
                throw bad_array_new_length();
            } else {
                return static_cast<T*>(::operator new(n * sizeof(T)));
            }
        }

        constexpr void deallocate(T* p, std::size_t n) {
            ::operator delete(p, n * sizeof(T));
        }
    };

    template<class T, class U>
    constexpr bool operator==(const allocator<T>&, const allocator<U>&) noexcept {
        return true;
    }
}