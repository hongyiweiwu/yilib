#pragma once

#include "util/priority_tag.hpp"
#include "util/always_false.hpp"
#include "type_traits.hpp"
#include "concepts.hpp"
#include "cstddef.hpp"
#include "utility.hpp"
#include "limits.hpp"
#include "new.hpp"

#include "tuple.hpp"

namespace std {
    namespace __internal {
        template<class T> constexpr T* addressof(T& r) noexcept {
            if constexpr (is_object_v<T>) {
                // And we cast back to the desired T* type.
                return reinterpret_cast<T*>(
                    // And then we remove the cv-qualifiers, and take the address -- we know char doesn't have an overload & operator.
                    &const_cast<char&>(
                        // First we cast r to the most permissible (cv-qualified) char type.
                        reinterpret_cast<const volatile char&>(r)
                    )
                );
            } else {
                // For non-object types, overloading & is impossible so we can safely use it.
                return &r;
            }
        }

        template<class T> const T* addressof(const T&&) = delete;
    }

    /* 20.10.3 Pointer traits */
    namespace __internal {
        template<class Ptr> struct __pointer_element_type {};
        template<class Ptr> requires requires { typename Ptr::element_type; }
        struct __pointer_element_type<Ptr> { using type = typename Ptr::element_type; };
        template<template<class, class...> class SomePointer, class T, class ...Args> requires (!requires { typename SomePointer<T, Args...>::element_type; })
        struct __pointer_element_type<SomePointer<T, Args...>> { using type = T; };

        template<class Ptr> auto __pointer_element_difference_type() -> ptrdiff_t;
        template<class Ptr> requires requires { typename Ptr::difference_type; }
        auto __pointer_element_difference_type() -> typename Ptr::difference_type;

        template<class Ptr, class U> struct __pointer_rebind_type {};
        template<class Ptr, class U> requires requires { typename Ptr::template rebind<U>; }
        struct __pointer_rebind_type<Ptr, U> { using type = typename Ptr::template rebind<U>; };
        template<template<class, class...> class SomePointer, class U, class T, class ...Args> requires (!requires { typename SomePointer<T, Args...>::template rebind<U>; })
        struct __pointer_rebind_type<SomePointer<T, Args...>, U> { using type = SomePointer<U, Args...>; };
    }

    template<class Ptr> struct pointer_traits {
        using pointer = Ptr;
        using element_type = typename __internal::__pointer_element_type<Ptr>::type;
        using difference_type = decltype(__internal::__pointer_element_difference_type<Ptr>());

        template<class U> using rebind = typename __internal::__pointer_rebind_type<Ptr, U>::type;
    };

    template<class T> struct pointer_traits<T*> {
        using pointer = T*;
        using element_type = T;
        using difference_type = ptrdiff_t;

        template<class U> using rebind = U*;

        static constexpr pointer pointer_to(conditional_t<is_void_v<element_type>, char&, element_type&> r) noexcept {
            return addressof(r);
        }
    };

    /* 20.10.4 Pointer conversion */
    template<class T> requires (!is_function_v<T>)
    constexpr T* to_address(T* p) noexcept { return p; }

    template<class Ptr> auto to_address(const Ptr& p) noexcept {
        if constexpr (requires (const Ptr* p) { pointer_traits<Ptr>::to_address(p); }) {
            return pointer_traits<Ptr>::to_address(p);
        } else {
            return to_address(p.operator->());
        }
    }

    /* 20.10.5 Pointer safety */
    // NOTE: This section of the code hasn't been implemented. The empty declarations are included purely to satisfy
    // the standard. It's unlikely that it'll ever be implemented, and there's an active proposal (P2186) to remove
    // it from the standard.
    enum class pointer_safety { relaxed, preferred, strict };
    void declare_reachable(void* p);
    template<class T> T* undeclare_reachable(T* p) { return p; }
    void declare_no_pointers(char* p, size_t n);
    void undeclare_no_pointers(char* p, size_t n);
    pointer_safety get_pointer_safety() noexcept;

    /* 20.10.6 Pointer alignment */
    void* align(size_t alignment, size_t size, void*& ptr, size_t& space);

    template<size_t N, class T> requires (N != 0) && ((N & (N - 1)) == 0) // N is a power of two.
    [[nodiscard]] constexpr T* assume_aligned(T* ptr) { return ptr; }

    /* 20.10.7 Allocator argument tag */
    struct allocator_arg_t { explicit allocator_arg_t() = default; };
    inline constexpr allocator_arg_t allocator_arg{};

    /* 20.10.8 uses_allocator */
    template<class T, class Alloc> struct uses_allocator : false_type {};
    template<class T, class Alloc> requires requires { typename T::allocator_type; } && is_convertible_v<Alloc, typename T::allocator_type>
        struct uses_allocator<T, Alloc> : true_type {};
    template<class T, class Alloc> inline constexpr bool uses_allocator_v = uses_allocator<T, Alloc>::value;
    
    namespace __internal {
        // TODO: This merely checks if T is a class based on the pair template. It doesn't check if T is an explicitly defined specialization.
        template<class T> struct __is_specialized_of_pair : false_type {};
        template<class T1, class T2> struct __is_specialized_of_pair<pair<T1, T2>> : true_type {};
        template<class T> struct is_specialized_of_pair : __is_specialized_of_pair<remove_cv_t<T>> {};
    }

    template<class T, class Alloc, class ...Args> requires (!__internal::is_specialized_of_pair<T>::value)
    constexpr auto uses_allocator_construction_args(const Alloc& alloc, Args&& ...args) noexcept {
        if constexpr (!uses_allocator_v<T, Alloc> && is_constructible_v<T, Args...>) {
            return forward_as_tuple(forward<Args>(args)...);
        } else if constexpr (uses_allocator_v<T, Alloc> && is_constructible_v<T, allocator_arg_t, const Alloc&, Args...>) {
            return tuple<allocator_arg_t, const Alloc&, Args&&...>(allocator_arg, alloc, forward<Args>(args)...);
        } else if constexpr (uses_allocator_v<T, Alloc> && is_constructible_v<T, Args..., const Alloc&>) {
            return forward_as_tuple(forward<Args>(args)..., alloc);
        } else {
            static_assert(__internal::always_false<T>);
        }
    }

    template<class T, class Alloc, class Tuple1, class Tuple2> requires __internal::is_specialized_of_pair<T>::value
    constexpr auto uses_allocator_construction_args(const Alloc& alloc, piecewise_construct_t, Tuple1&& x, Tuple2&& y) noexcept {
        using T1 = typename T::first_type;
        using T2 = typename T::second_type;

        return make_tuple(
            piecewise_construct,
            apply([&alloc](auto&& ...args1) {
                return uses_allocator_construction_args<T1>(alloc, forward<decltype(args1)>(args1)...);
            }, forward<Tuple1>(x)),
            apply([&alloc](auto&& ...args2) {
                return uses_allocator_construction_args<T2>(alloc, forward<decltype(args2)>(args2)...);
            }, forward<Tuple2>(y))
        );
    }

    template<class T, class Alloc> requires __internal::is_specialized_of_pair<T>::value
    constexpr auto uses_allocator_construction_args(const Alloc& alloc) noexcept {
        return uses_allocator_construction_args<T>(alloc, piecewise_construct, forward_as_tuple(), forward_as_tuple());
    }

    template<class T, class Alloc, class U, class V> requires __internal::is_specialized_of_pair<T>::value
    constexpr auto uses_allocator_construction_args(const Alloc& alloc, U&& u, V&& v) noexcept {
        return uses_allocator_construction_args<T>(alloc, piecewise_construct, forward_as_tuple(forward<U>(u)), forward_as_tuple(forward<V>(v)));
    }

    template<class T, class Alloc, class U, class V> requires __internal::is_specialized_of_pair<T>::value
    constexpr auto uses_allocator_construction_args(const Alloc& alloc, const pair<U, V>& pr) noexcept {
        return uses_allocator_construction_args<T>(alloc, piecewise_construct, forward_as_tuple(pr.first), forward_as_tuple(pr.second));
    }

    template<class T, class Alloc, class U, class V> requires __internal::is_specialized_of_pair<T>::value
    constexpr auto uses_allocator_construction_args(const Alloc& alloc, pair<U, V>&& pr) noexcept {
        return uses_allocator_construction_args<T>(alloc, piecewise_construct, forward_as_tuple(move(pr).first), forward_as_tuple(move(pr).second));
    }

    template<class T, class Alloc, class ...Args>
    constexpr T make_obj_using_allocator(const Alloc& alloc, Args&& ...args) {
        return make_from_tuple<T>(uses_allocator_construction_args<T>(alloc, forward<Args>(args)...));
    }

    namespace __internal {
        template<class T, class ...Args> requires requires (void* ptr, Args&& ...args) { ::new (ptr) T(forward<Args>(args)...); }
        constexpr T* construct_at(T* p, Args&& ...args) {
            return ::new (const_cast<void*>(static_cast<const volatile void*>(p))) T(forward<Args>(args)...);
        }
    }

    template<class T, class Alloc, class ...Args>
    constexpr T* uninitialized_construct_using_allocator(T* p, const Alloc& alloc, Args&& ...args) {
        return apply([&]<class ...U>(U&& ...xs) {
            return construct_at(p, forward<U>(xs)...);
        }, uses_allocator_construction_args<T>(alloc, forward<Args>(args)...));
    }

    /* 20.10.9 Allocator traits */
    namespace __internal {
        template<class Alloc> auto __allocator_traits_pointer_type() -> typename Alloc::value_type;
        template<class Alloc> requires requires { typename Alloc::pointer; }
        auto __allocator_traits_pointer_type() -> typename Alloc::pointer;

        template<class Alloc> auto __allocator_traits_const_pointer_type() {
            if constexpr (requires { typename Alloc::const_pointer; }) return declval<Alloc::const_pointer>();

            using pointer = decltype(__allocator_traits_pointer_type<Alloc>());
            using value_type = typename Alloc::value_type;
            return declval<pointer_traits<pointer>::template rebind<const value_type>>();
        } 

        template<class Alloc> auto __allocator_traits_void_pointer_type() {
            if constexpr (requires { typename Alloc::void_pointer; }) return declval<Alloc::void_pointer>();

            using pointer = decltype(__allocator_traits_pointer_type<Alloc>());
            return declval<pointer_traits<pointer>::template rebind<void>>();
        } 

        template<class Alloc> auto __allocator_traits_const_void_pointer_type() {
            if constexpr (requires { typename Alloc::const_void_pointer; }) return declval<Alloc::const_void_pointer>();

            using pointer = decltype(__allocator_traits_pointer_type<Alloc>());
            return declval<pointer_traits<pointer>::template rebind<const void>>();
        }

        template<class Alloc> auto __allocator_traits_difference_type() {
            if constexpr (requires { typename Alloc::difference_type; }) return declval<Alloc::difference_type>();

            using pointer = decltype(__allocator_traits_pointer_type<Alloc>());
            return declval<pointer_traits<pointer>::difference_type>();
        }

        template<class Alloc> auto __allocator_traits_size_type() {
            if constexpr (requires { typename Alloc::size_type; }) return declval<Alloc::size_type>();
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

        template<class T> constexpr void destroy_at(T* location) {
            if constexpr (is_array<T>::value) {
                // Equivalent to destroy(begin(*location), end(*location)).
                for (auto &elem: *location) {
                    destroy_at(addressof(elem));
                }
            } else {
                location->~T();
            }
        }
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
            requires requires { a.construct(p, forward<Args>(args)...); } || requires { __internal::construct_at(p, forward<Args>(args)...); } {
            if constexpr (requires { a.construct(p, forward<Args>(args)...); }) {
                return a.construct(p, forward<Args>(args)...);
            } else {
                return __internal::construct_at(p, forward<Args>(args)...);
            }
        }

        template<class T> static constexpr void destroy(Alloc& a, T* p)
            requires requires { a.destroy(p); } || requires { __internal::destroy_at(p); } {
            if constexpr (requires { a.destroy(p); }) {
                a.destroy(p);
            } else {
                __internal::destroy_at(p);
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
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using propagate_on_container_move_assignment = true_type;
        using is_always_equal = true_type;

        constexpr allocator() noexcept = default;
        constexpr allocator(const allocator&) noexcept = default;
        template<class U> constexpr allocator(const allocator<U>&) noexcept : allocator() {}
        constexpr ~allocator() = default;
        constexpr allocator& operator=(const allocator&) = default;

        [[nodiscard]] constexpr T* allocate(size_t n) requires __internal::is_complete<T>::value {
            if (numeric_limits<size_t>::max() / sizeof(T) < n) {
                throw bad_array_new_length();
            } else {
                return ::operator new(n * sizeof(T));
            }
        }

        constexpr void deallocate(T* p, size_t n) {
            ::operator delete(p, n * sizeof(T));
        }
    };

    template<class T, class U> constexpr bool operator==(const allocator<T>&, const allocator<U>&) noexcept {
        return true;
    }

    /* 20.10.11 addressof */
    using __internal::addressof;

    /* 25.11.8 construct_at */
    using __internal::construct_at;

    namespace ranges {
        namespace __internal {
            struct __construct_at_fn {
                template<class T, class ...Args> requires requires (void* ptr, Args&& ...args) { ::new (ptr) T(forward<Args>(args)...); }
                constexpr T* operator()(T* p, Args&& ...args) const {
                    return std::construct_at(p, forward<Args>(args)...);
                }
            };
        }

        inline constexpr __internal::__construct_at_fn construct_at{};
    }

    /* 25.11.9 destroy */
    using __internal::destroy_at;
    
    namespace ranges {
        namespace __internal {
            struct __destroy_at_fn {
                template<destructible T> constexpr void destroy_at(T* location) noexcept {
                    return std::destroy_at(location);
                }
            };
        }

        inline constexpr __internal::__destroy_at_fn destroy_at{};
    }
}