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
    namespace __internal {
        template<class Alloc> auto __allocator_traits_pointer_type() -> typename Alloc::value_type*;
        template<class Alloc> requires requires { typename Alloc::pointer; }
        auto __allocator_traits_pointer_type() -> typename Alloc::pointer;

        template<class Alloc> auto __allocator_traits_const_pointer_type() {
            if constexpr (requires { typename Alloc::const_pointer; }) return declval<typename Alloc::const_pointer>();

            using pointer = decltype(__allocator_traits_pointer_type<Alloc>());
            using value_type = typename Alloc::value_type;
            return declval<typename pointer_traits<pointer>::template rebind<const value_type>>();
        } 

        template<class Alloc> auto __allocator_traits_void_pointer_type() {
            if constexpr (requires { typename Alloc::void_pointer; }) return declval<typename Alloc::void_pointer>();

            using pointer = decltype(__allocator_traits_pointer_type<Alloc>());
            return declval<typename pointer_traits<pointer>::template rebind<void>>();
        } 

        template<class Alloc> auto __allocator_traits_const_void_pointer_type() {
            if constexpr (requires { typename Alloc::const_void_pointer; }) return declval<typename Alloc::const_void_pointer>();

            using pointer = decltype(__allocator_traits_pointer_type<Alloc>());
            return declval<typename pointer_traits<pointer>::template rebind<const void>>();
        }

        template<class Alloc> auto __allocator_traits_difference_type() {
            if constexpr (requires { typename Alloc::difference_type; }) return declval<typename Alloc::difference_type>();

            using pointer = decltype(__allocator_traits_pointer_type<Alloc>());
            return declval<typename pointer_traits<pointer>::difference_type>();
        }

        template<class Alloc> auto __allocator_traits_size_type() {
            if constexpr (requires { typename Alloc::size_type; }) return declval<typename Alloc::size_type>();
            return declval<make_unsigned_t<decltype(__allocator_traits_difference_type<Alloc>())>>();
        }

        template<class Alloc> auto __allocator_traits_propagate_on_container_copy_assignment() -> false_type;
        template<class Alloc> requires requires { typename Alloc::propagate_on_container_copy_assignment; }
        auto __allocator_traits_propagate_on_container_copy_assignment() -> typename Alloc::propagate_on_container_copy_assignment;

        template<class Alloc> auto __allocator_traits_propagate_on_container_move_assignment() -> false_type;
        template<class Alloc> requires requires { typename Alloc::propagate_on_container_move_assignment; }
        auto __allocator_traits_propagate_on_container_move_assignment() -> typename Alloc::propagate_on_container_move_assignment;

        template<class Alloc> auto __allocator_traits_propagate_on_container_swap() -> false_type;
        template<class Alloc> requires requires { typename Alloc::propagate_on_container_swap; }
        auto __allocator_traits_propagate_on_container_swap() -> typename Alloc::propagate_on_container_swap;

        template<class Alloc> auto __allocator_traits_is_always_equal() -> false_type;
        template<class Alloc> requires requires { typename Alloc::is_always_equal; }
        auto __allocator_traits_is_always_equal() -> typename Alloc::is_always_equal;

        template<class Alloc, class T> struct __allocator_traits_rebind_alloc {};
        template<class Alloc, class T> requires requires { typename Alloc::template rebind<T>::other; }
        struct __allocator_traits_rebind_alloc<Alloc, T> { using type = typename Alloc::template rebind<T>::other; };
        template<template<class, class...> class Alloc, class T, class U, class ...Args>
        struct __allocator_traits_rebind_alloc<Alloc<U, Args...>, T> { using type = Alloc<T, Args...>; };
    }

    template<class Alloc> requires requires { typename Alloc::value_type; }
    struct allocator_traits {
        using allocator_type = Alloc;

        using value_type = typename Alloc::value_type;

        using pointer = decltype(__internal::__allocator_traits_pointer_type<Alloc>());
        using const_pointer = decltype(__internal::__allocator_traits_const_pointer_type<Alloc>());
        using void_pointer = decltype(__internal::__allocator_traits_void_pointer_type<Alloc>());
        using const_void_pointer = decltype(__internal::__allocator_traits_const_void_pointer_type<Alloc>());

        using difference_type = decltype(__internal::__allocator_traits_difference_type<Alloc>());
        using size_type = decltype(__internal::__allocator_traits_size_type<Alloc>());

        using propagate_on_container_copy_assignment = decltype(__internal::__allocator_traits_propagate_on_container_copy_assignment<Alloc>());
        using propagate_on_container_move_assignment = decltype(__internal::__allocator_traits_propagate_on_container_move_assignment<Alloc>());
        using propagate_on_container_swap = decltype(__internal::__allocator_traits_propagate_on_container_swap<Alloc>());
        using is_always_equal = decltype(__internal::__allocator_traits_is_always_equal<Alloc>());

        template<class T> using rebind_alloc = typename __internal::__allocator_traits_rebind_alloc<Alloc, T>::type;
        template<class T> using rebind_traits = allocator_traits<rebind_alloc<T>>;

        [[nodiscard]] static constexpr pointer allocate(Alloc& a, size_type n) requires requires { a.allocate(n); } {
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

        static constexpr void deallocate(Alloc& a, pointer p, size_type n) requires requires { a.deallocate(p, n); }  {
            a.deallocate(p, n);
        }

        template<class T, class ...Args>
        static constexpr void construct(Alloc& a, T* p, Args&& ...args) 
            requires requires { a.construct(p, forward<Args>(args)...); } || requires { construct_at(p, forward<Args>(args)...); } {
            if constexpr (requires { a.construct(p, forward<Args>(args)...); }) {
                return a.construct(p, forward<Args>(args)...);
            } else {
                return construct_at(p, forward<Args>(args)...);
            }
        }

        template<class T> static constexpr void destroy(Alloc& a, T* p)
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
    template<class T> class allocator {
    public:
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using propagate_on_container_move_assignment = true_type;
        using is_always_equal = true_type;

        constexpr allocator() noexcept = default;
        constexpr allocator(const allocator&) noexcept = default;
        template<class U> constexpr allocator(const allocator<U>&) noexcept : allocator() {}
        constexpr ~allocator() = default;
        constexpr allocator& operator=(const allocator&) = default;

        [[nodiscard]] constexpr T* allocate(std::size_t n) requires __internal::is_complete<T>::value {
            if (numeric_limits<std::size_t>::max() / sizeof(T) < n) {
                throw bad_array_new_length();
            } else {
                return ::operator new(n * sizeof(T));
            }
        }

        constexpr void deallocate(T* p, std::size_t n) {
            ::operator delete(p, n * sizeof(T));
        }
    };

    template<class T, class U> constexpr bool operator==(const allocator<T>&, const allocator<U>&) noexcept {
        return true;
    }
}