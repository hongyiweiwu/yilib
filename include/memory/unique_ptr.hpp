#pragma once

#include "type_traits.hpp"
#include "compare.hpp"
#include "utility.hpp"
#include "cstddef.hpp"

/* 20.11.1 Class template unique_ptr */
namespace std {
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
}