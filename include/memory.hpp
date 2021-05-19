#pragma once

#include "util/priority_tag.hpp"
#include "util/always_false.hpp"
#include "type_traits.hpp"
#include "concepts.hpp"
#include "cstddef.hpp"
#include "utility.hpp"
#include "limits.hpp"
#include "new.hpp"
#include "functional.hpp"

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

    /* 20.11.1 Class template unique_ptr */
    /* 20.11.1.2 Default deleters */
    template<class T> struct default_delete {
        constexpr default_delete() noexcept = default;

        template<class U> requires is_convertible_v<U*, T*>
        default_delete(const default_delete<U>&) noexcept : default_delete() {};

        void operator()(T* ptr) const requires __internal::is_complete<T>::value { delete ptr; }
    };

    template<class T> struct default_delete<T[]> {
        constexpr default_delete() noexcept = default;

        template<class U> requires is_convertible_v<U*, T*>
        default_delete(const default_delete<U[]>&) noexcept : default_delete() {};

        template<class U> requires __internal::is_complete<U>::value && is_convertible_v<U*, T*>
        void operator()(U* ptr) const { delete[] ptr; }
    };

    /* 20.11.1.3 unique_ptr for single objects */
    namespace __internal {
        template<class T, class D> struct __unique_ptr_pointer_type { using type = T*; };
        template<class T, class D> requires requires { typename remove_reference_t<D>::pointer; } 
        struct __unique_ptr_pointer_type<T, D> { using type = typename remove_reference_t<D>::pointer;  };
    }

    template<class T, class D = default_delete<T>> 
        requires (is_function_v<D> || (!is_rvalue_reference_v<D> && is_object_v<remove_reference_t<D>> 
            && requires { declval<D>()(declval<typename __internal::__unique_ptr_pointer_type<T, D>::type>()); } ))
    class unique_ptr {
    public:
        using pointer = typename __internal::__unique_ptr_pointer_type<T, D>::type;
        using element_type = T;
        using deleter_type = D;

    private:
        pointer p;
        deleter_type d;

    public:
        /* 20.11.1.3.5 Observers */
        add_lvalue_reference_t<T> operator*() const { return *get(); }
        pointer operator->() const noexcept { return get(); }
        pointer get() const noexcept { return p; }
        deleter_type& get_deleter() noexcept { return d; }
        const deleter_type& get_deleter() const noexcept { return d; }
        explicit operator bool() const noexcept { return get() != nullptr; }

        /* 20.11.1.3.6 Modifiers */
        pointer release() noexcept { 
            pointer curr = get();
            p = nullptr;
            return curr;
        }

        void reset(pointer p = pointer()) noexcept requires requires { get_deleter()(get()); } {
            pointer old = get();
            this->p = p;
            if (old != nullptr) get_deleter()(get());
        }

        void swap(unique_ptr& u) noexcept requires is_nothrow_swappable_v<decltype(get_deleter())> {
            swap(get_deleter(), u.get_deleter());
            swap(p, u.p);
        }

        /* 20.11.1.3.2 Constructors */
        constexpr unique_ptr() noexcept requires (!is_pointer_v<deleter_type>) && is_nothrow_default_constructible_v<deleter_type> = default;
        
        // We wrap pointer with type_identity_t to prevent class template deduction.
        explicit unique_ptr(type_identity_t<pointer> p) noexcept requires (!is_pointer_v<deleter_type>) && is_nothrow_default_constructible_v<deleter_type>
            : p(p), d() {}

        unique_ptr(type_identity_t<pointer> p, const D& d) noexcept requires is_reference_v<D> || is_nothrow_copy_constructible_v<D>
            : p(p), d(d) {}

        unique_ptr(type_identity_t<pointer> p, remove_reference_t<D>&& d) requires is_reference_v<D> = delete;
        unique_ptr(type_identity_t<pointer> p, D&& d) noexcept requires (!is_reference_v<D>) 
            && is_nothrow_move_constructible_v<D> : p(p), d(move(d)) {}

        unique_ptr(unique_ptr&& u) noexcept requires is_move_constructible_v<D> && (is_reference_v<D> || is_nothrow_move_constructible_v<D>)
            : p(forward<pointer>(u.get())), d(forward<D>(u.get_deleter())) {
            u.p = nullptr;
        }

        template<class U, class E> requires is_convertible_v<typename unique_ptr<U, E>::pointer, pointer> && (!is_array_v<U>)
            && ((is_reference_v<D> && is_same_v<E, D>) || (!is_reference_v<D> && is_convertible_v<E, D>))
            && ((!is_reference_v<E> && is_nothrow_constructible_v<D, E>) || (is_reference_v<E> && is_nothrow_constructible_v<D, E&>))
        unique_ptr(unique_ptr<U, E>&& u) noexcept : p(forward<unique_ptr<U, E>::pointer>(u.get())), d(forward<E>(u.get_deleter())) {
            u.p = nullptr;
        }

        constexpr unique_ptr(nullptr_t) noexcept requires (!is_pointer_v<deleter_type>) && is_default_constructible_v<deleter_type>
            : unique_ptr() {}

        /* 20.11.1.3.3 Destructor */
        ~unique_ptr() noexcept requires requires { get_deleter()(get()); } { 
            if (get() != nullptr) get_deleter()(get());
        }

        /* 20.11.1.3.4 Assignment */
        unique_ptr& operator=(unique_ptr&& u) noexcept requires is_move_assignable_v<D>
            && ((!is_reference_v<D> && is_nothrow_move_assignable_v<D>) || (is_reference_v<D> && is_nothrow_copy_assignable_v<remove_reference_t<D>>)) {
            reset(u.release());
            get_deleter() = forward<D>(u.get_deleter());
            return *this;
        }

        template<class U, class E> requires is_convertible_v<typename unique_ptr<U, E>::pointer, pointer>
            && (!is_array_v<U>) && is_assignable_v<D&, E&&>
            && ((!is_reference_v<E> && is_nothrow_assignable_v<D&, E>) || (is_reference_v<E> && is_nothrow_assignable_v<D&, E&>))
        unique_ptr& operator=(unique_ptr<U, E>&& u) noexcept {
            reset(u.release());
            get_deleter() = forward<E>(u.get_deleter());
            return *this;
        }

        unique_ptr& operator=(nullptr_t) noexcept { 
            reset();
            return *this;
        }

        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;
    };

    /* 20.11.1.4 unique_ptr for array objects with a runtime length */
    template<class T, class D> 
        requires (is_function_v<D> || (!is_rvalue_reference_v<D> && is_object_v<remove_reference_t<D>> 
            && requires { declval<D>()(declval<typename __internal::__unique_ptr_pointer_type<T, D>::type>()); } ))
    class unique_ptr<T[], D> {
    public:
        using pointer = typename __internal::__unique_ptr_pointer_type<T, D>::type;
        using element_type = T;
        using deleter_type = D;
    private:
        pointer p;
        deleter_type d;
    public:
        /* 20.11.1.4.4 Observers */
        pointer get() const noexcept { return p; }
        deleter_type& get_deleter() noexcept { return d; }
        const deleter_type& get_deleter() const noexcept { return d; }
        explicit operator bool() const noexcept { return get() != nullptr; }
        T& operator[](size_t i) const { return get()[i]; };

        /* 20.11.1.4.5 Modifiers */
        pointer release() noexcept { 
            pointer curr = get();
            p = nullptr;
            return curr;
        }

        void reset(nullptr_t p = nullptr) noexcept { reset(pointer()); }

        template<class U> requires is_same_v<U, pointer> || (is_same_v<pointer, element_type*> && is_pointer_v<U> && is_convertible_v<U, element_type*>)
        void reset(U p) noexcept requires requires { get_deleter()(get()); } {
            pointer old = get();
            this->p = p;
            if (old != nullptr) get_deleter()(get());
        }

        void swap(unique_ptr& u) noexcept requires is_nothrow_swappable_v<decltype(get_deleter())> {
            swap(get_deleter(), u.get_deleter());
            swap(p, u.p);
        }

        /* 20.11.1.4.2 Constructors */
        constexpr unique_ptr() noexcept requires (!is_pointer_v<deleter_type>) && is_nothrow_default_constructible_v<deleter_type> = default;

        template<class U> requires (is_same_v<U, pointer> || (is_same_v<pointer, element_type*> && is_pointer_v<U> && is_convertible_v<U, element_type*>))
            && (!is_pointer_v<deleter_type>) && is_nothrow_default_constructible_v<deleter_type>
        explicit unique_ptr(type_identity_t<U> p) noexcept : p(p), d() {} 

        template<class U> requires is_reference_v<D> || is_nothrow_copy_constructible_v<D>
            && (is_same_v<U, pointer> || is_null_pointer_v<U> || (is_same_v<pointer, element_type*> && is_pointer_v<U> && is_convertible_v<U, element_type*>))
        unique_ptr(type_identity_t<U> p, const D& d) noexcept : p(p), d(d) {}

        template<class U> requires is_reference_v<D>
        unique_ptr(type_identity_t<U> p, remove_reference_t<D>&& d) = delete;

        template<class U> requires (!is_reference_v<D>) && is_nothrow_move_constructible_v<D> 
            && (is_same_v<U, pointer> || is_null_pointer_v<U> || (is_same_v<pointer, element_type*> && is_pointer_v<U> && is_convertible_v<U, element_type*>))
        unique_ptr(type_identity_t<U> p, D&& d) noexcept : p(p), d(move(d)) {}

        unique_ptr(unique_ptr&& u) noexcept requires is_move_constructible_v<D> && (is_reference_v<D> || is_nothrow_move_constructible_v<D>)
            : p(forward<pointer>(u.get())), d(forward<D>(u.get_deleter())) {
            u.p = nullptr;
        }

        template<class U, class E> requires is_array_v<U>
            && is_same_v<pointer, typename unique_ptr<U, E>::element_type*>
            && is_same_v<typename unique_ptr<U, E>::pointer, typename unique_ptr<U, E>::element_type*>
            && is_convertible_v<typename unique_ptr<U, E>::element_type*, element_type*>
            && ((is_reference_v<D> && is_same_v<E, D>) || (!is_reference_v<D> && is_convertible_v<E, D>))
        unique_ptr(unique_ptr<U, E>&& u) noexcept : p(forward<unique_ptr<U, E>::pointer>(u.get())), d(forward<E>(u.get_deleter())) {
            u.p = nullptr;
        }

        constexpr unique_ptr(nullptr_t) noexcept requires (!is_pointer_v<deleter_type>) && is_default_constructible_v<deleter_type>
            : unique_ptr() {}

        ~unique_ptr() noexcept requires requires { get_deleter()(get()); } { 
            if (get() != nullptr) get_deleter()(get());
        }

        /* 20.11.1.4.3 Assignment */
        unique_ptr& operator=(unique_ptr&& u) noexcept requires is_move_assignable_v<D>
            && ((!is_reference_v<D> && is_nothrow_move_assignable_v<D>) || (is_reference_v<D> && is_nothrow_copy_assignable_v<remove_reference_t<D>>)) {
            reset(u.release());
            get_deleter() = forward<D>(u.get_deleter());
            return *this;
        }

        template<class U, class E> requires is_array_v<U>
            && is_same_v<pointer, typename unique_ptr<U, E>::element_type*>
            && is_same_v<typename unique_ptr<U, E>::pointer, typename unique_ptr<U, E>::element_type*>
            && is_assignable_v<D&, E&&>
        unique_ptr& operator=(unique_ptr<U, E>&& u) noexcept {
            reset(u.release());
            get_deleter() = forward<E>(u.get_deleter());
            return *this;
        }

        unique_ptr& operator=(nullptr_t) noexcept { 
            reset();
            return *this;
        }

        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;
    };

    /* 20.11.1.5 Creation */
    template<class T, class ...Args> requires (!is_array_v<T>)
    unique_ptr<T> make_unique(Args&& ...args) {
        return unique_ptr<T>(new T(forward<Args>(args)...));
    }

    template<class T, class ...Args> requires is_unbounded_array_v<T>
    unique_ptr<T> make_unique(size_t n) {
        return unique_ptr<T>(new remove_extent_t<T>[n]());
    }

    template<class T, class ...Args> requires is_bounded_array_v<T>
    auto make_unique(Args&&...) = delete;

    template<class T> requires (!is_array_v<T>)
    unique_ptr<T> make_unique_for_overwrite() { return unique_ptr<T>(new T); }

    template<class T, class ...Args> requires is_unbounded_array_v<T>
    unique_ptr<T> make_unique_for_overwrite(size_t n) { return unique_ptr<T>(new remove_extent_t<T>[n]); }

    template<class T, class ...Args> requires is_bounded_array_v<T>
    auto make_unique_for_overwrite(Args&&...) = delete;

    /* 20.11.1.6 Specialized algorithms */
    template<class T, class D> requires is_swappable_v<D>
    void swap(unique_ptr<T, D>& x, unique_ptr<T, D>& y) noexcept { x.swap(y); }

    template<class T1, class D1, class T2, class D2>
    bool operator==(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) { return x.get() == y.get(); }

    template<class T1, class D1, class T2, class D2> 
        requires requires { typename common_type_t<typename unique_ptr<T1, D1>::pointer, typename unique_ptr<T2, D2>::pointer>; }
    bool operator<(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) { 
        return less<common_type_t<typename unique_ptr<T1, D1>::pointer, typename unique_ptr<T2, D2>::pointer>>()(x.get(), y.get());
    }

    template<class T1, class D1, class T2, class D2>
    bool operator>(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) { return y < x; }

    template<class T1, class D1, class T2, class D2>
    bool operator<=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) { return !(y < x); }

    template<class T1, class D1, class T2, class D2>
    bool operator>=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) { return !(x < y); }

    template<class T1, class D1, class T2, class D2>
    requires three_way_comparable_with<typename unique_ptr<T1, D1>::pointer, typename unique_ptr<T2, D2>::pointer>
    compare_three_way_result_t<typename unique_ptr<T1, D1>::pointer, typename unique_ptr<T2, D2>::pointer>
    operator<=>(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
        return compare_three_way()(x.get(), y.get());
    }

    template<class T, class D> bool operator==(const unique_ptr<T, D>& x, nullptr_t) noexcept { return !x; }
    template<class T, class D> bool operator<(const unique_ptr<T, D>& x, nullptr_t) {
        return less<typename unique_ptr<T, D>::pointer>()(x.get(), nullptr);
    }
    template<class T, class D> bool operator<(nullptr_t, const unique_ptr<T, D>& x) {
        return less<typename unique_ptr<T, D>::pointer>()(nullptr, x.get());
    }

    template<class T, class D> bool operator>(const unique_ptr<T, D>& x, nullptr_t) { return nullptr < x; }
    template<class T, class D> bool operator>(nullptr_t, const unique_ptr<T, D>& x) { return x < nullptr; }

    template<class T, class D> bool operator<=(const unique_ptr<T, D>& x, nullptr_t) { return !(nullptr < x); }
    template<class T, class D> bool operator<=(nullptr_t, const unique_ptr<T, D>& x) { return !(x < nullptr); }

    template<class T, class D> bool operator>=(const unique_ptr<T, D>& x, nullptr_t) { return !(x < nullptr); }
    template<class T, class D> bool operator>=(nullptr_t, const unique_ptr<T, D>& x) { return !(nullptr < x); }

    template<class T, class D> requires three_way_comparable_with<typename unique_ptr<T, D>::pointer, nullptr_t>
    compare_three_way_result_t<typename unique_ptr<T, D>::pointer, nullptr_t>
    operator<=>(const unique_ptr<T, D>& x, nullptr_t) {
        return compare_three_way()(x.get(), nullptr);
    }

    /* 20.11.1.7 I/O */
    // TODO: Include "iostream.hpp" & enable this code after implementing it.
    /*
    template<class E, class T, class Y, class D> requires requires (basic_ostream<E, T>& os) { os << p.get(); }
    basic_ostream<E, T>& operator<<(basic_ostream<E, T>& os, const unique_ptr<Y, D>& p) { return os << p.get(); } */

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