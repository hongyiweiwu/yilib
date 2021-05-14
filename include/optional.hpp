#pragma once

#include "type_traits.hpp"
#include "utility.hpp"
#include "initializer_list.hpp"
#include "exception.hpp"
#include "new.hpp"
#include "memory.hpp"
#include "concepts.hpp"
#include "compare.hpp"
#include "functional.hpp"

// NOTE: This implementation currently doesn't abide with the triviality requirement of special member functions in optional. This is due to 
// Clang's lack of support for proposal P0848R3 (Conditionally Trivial Special Member Functions). Once Clang adds the support this implementation
// will be compliant.
namespace std {
    /* 20.6.4 No-value state indicator */
    struct nullopt_t {
        // Makes nullopt_t a non-aggregate type containing no default constructors.
        explicit constexpr nullopt_t(int) {}
    };
    inline constexpr nullopt_t nullopt{0};

    /* 20.6.5 Class bad_optional_access */
    class bad_optional_access : public exception {
    public:
        const char* what() const noexcept override;
    };

    /* 20.6.3 Class template optional */
    namespace __internal {
        template<class T> struct __optional_storage {
            struct no_val_t {
                explicit no_val_t() = default;
            };

            static constexpr no_val_t no_val{};

            bool has_value;
            union {
                unsigned char dummy;
                T val;
            };

            explicit constexpr __optional_storage(no_val_t) noexcept : dummy(), has_value(false) {};
    
            template<class ...Args>
            explicit constexpr __optional_storage(Args&& ...args) : val(forward<Args>(args)...), has_value(true) {};

            void reset() {
                if (has_value) val.~T();
                has_value = false;
            }

            template<class ...Args> constexpr void emplace(Args&& ...args) {
                if (!has_value) has_value = true;
                ::new (static_cast<void*>(val)) T(forward<Args>(args)...);
            }

            __optional_storage() requires is_trivially_default_constructible_v<T> = default;
            __optional_storage() requires (!is_trivially_default_constructible_v<T>) : has_value(false), dummy() {}

            ~__optional_storage() requires is_trivially_destructible_v<T> = default;
            ~__optional_storage() requires (!is_trivially_destructible_v<T>) {
                if (has_value) {
                    val.~T();
                }
            }
        };
    }

    template<class T> class optional {
    private:
        __internal::__optional_storage<T> val;
    public:
        using value_type = T;

        constexpr optional() noexcept : val(__internal::__optional_storage<T>::no_val) {};
        constexpr optional(nullopt_t) noexcept : val(__internal::__optional_storage<T>::no_val) {};

        constexpr optional(const optional& rhs) requires is_copy_constructible_v<T> && is_trivially_copy_constructible_v<T> = default;
        constexpr optional(const optional& rhs) requires is_copy_constructible_v<T> && (!is_trivially_copy_constructible_v<T>) {
            if (rhs) val.emplace(*rhs);
        }

        constexpr optional(optional&& rhs) requires is_move_constructible_v<T> && is_trivially_move_constructible_v<T> = default;
        constexpr optional(optional&& rhs) noexcept(noexcept(T(move(*rhs)))) requires is_move_constructible_v<T> && (!is_trivially_move_constructible_v<T>) {
            if (rhs) val.emplace(move(*rhs));
        }

        template<class ...Args> requires is_constructible_v<T, Args...>
        constexpr explicit optional(in_place_t, Args&& ...args) : val(forward<Args>(args)...) {}

        template<class U, class ...Args> requires is_constructible_v<T, initializer_list<U>&, Args...>
        constexpr explicit optional(in_place_t, initializer_list<U> il, Args&& ...args) : val(il, forward<Args>(args)...) {}

        template<class U = T> requires is_constructible_v<T, U> && (!is_same_v<remove_cvref_t<U>, in_place_t>) && (!is_same_v<remove_cvref_t<U>, optional>)
        constexpr explicit(!is_convertible_v<U, T>) optional(U&& v) : val(forward<U>(v)) {}

        template<class U> requires is_constructible_v<T, const U&> && (!is_constructible_v<T, optional<U>&>)
            && (!is_constructible_v<T, optional<U>&&>) && (!is_constructible_v<T, const optional<U>&>)
            && (!is_constructible_v<T, const optional<U>&&>) && (!is_convertible_v<optional<U>&, T>)
            && (!is_convertible_v<optional<U>&&, T>) && (!is_convertible_v<const optional<U>&, T>)
            && (!is_convertible_v<const optional<U>&&, T>)
        explicit(!is_convertible_v<const U&, T>) optional(const optional<U>& rhs) {
            if (rhs) val.emplace(*rhs);
        }

        template<class U> requires is_constructible_v<T, U> && (!is_constructible_v<T, optional<U>&>)
            && (!is_constructible_v<T, optional<U>&&>) && (!is_constructible_v<T, const optional<U>&>)
            && (!is_constructible_v<T, const optional<U>&&>) && (!is_convertible_v<optional<U>&, T>)
            && (!is_convertible_v<optional<U>&&, T>) && (!is_convertible_v<const optional<U>&, T>)
            && (!is_convertible_v<const optional<U>&&, T>)
        explicit(!is_convertible_v<U, T>) optional(optional<U>&& rhs) {
            if (rhs) val.emplace(move(*rhs));
        }

        ~optional() = default;

        optional<T>& operator=(nullopt_t) noexcept {
            val.reset();
            return *this;
        }

        constexpr optional& operator=(const optional& rhs) requires (is_trivially_copy_constructible_v<T> && is_trivially_copy_assignable_v<T> && is_trivially_destructible_v<T>) = default;
        constexpr optional& operator=(const optional& rhs) requires (!is_trivially_copy_constructible_v<T>) || (!is_trivially_copy_assignable_v<T>) || (!is_trivially_destructible_v<T>) {
            if (*this && rhs) val.val = *rhs;
            else if (*this && !rhs) val.reset();
            else if (!*this && rhs) val.emplace(*rhs);

            return *this;
        }

        constexpr optional& operator=(optional&& rhs) requires (is_trivially_move_constructible_v<T> && is_trivially_move_assignable_v<T> && is_trivially_destructible_v<T>) = default;
        constexpr optional& operator=(optional&& rhs) noexcept(noexcept(is_nothrow_move_assignable_v<T> && is_nothrow_move_constructible_v<T>))
            requires is_move_constructible_v<T> && is_move_assignable_v<T> && ((!is_trivially_copy_constructible_v<T>) || (!is_trivially_copy_assignable_v<T>) || (!is_trivially_destructible_v<T>)) {
            if (*this && rhs) val.val = move(*rhs);
            else if (*this && !rhs) val.reset();
            else if (!*this && rhs) val.emplace(move(*rhs));

            return *this;
        }

        template<class U = T> requires (!is_same_v<remove_cvref_t<U>, optional>) && (!conjunction_v<is_scalar<T>, is_same<T, decay_t<U>>>) && is_constructible_v<T, U> && is_assignable_v<T&, U>
        optional<T>& operator=(U&& v) {
            if (*this) val.val = forward<U>(v);
            else val.emplace(forward<U>(v));

            return *this;
        }

        template<class U> requires is_constructible_v<T, U> && is_assignable_v<T&, U>
            && (!is_constructible_v<T, optional<U>&>) && (!is_constructible_v<T, optional<U>&&>) 
            && (!is_constructible_v<T, const optional<U>&>) && (!is_constructible_v<T, const optional<U>&&>) 
            && (!is_convertible_v<optional<U>&, T>) && (!is_convertible_v<optional<U>&&, T>) 
            && (!is_convertible_v<const optional<U>&, T>) && (!is_convertible_v<const optional<U>&&, T>)
            && (!is_assignable_v<T&, optional<U>&>) && (!is_assignable_v<T&, optional<U>&&>)
            && (!is_assignable_v<T&, const optional<U>&>) && (!is_assignable_v<T&, const optional<U>&&>)
        optional<T>& operator=(const optional<U>& rhs) {
            if (*this && rhs) val.val = move(*rhs);
            else if (*this && !rhs) val.reset();
            else if (!*this && rhs) val.emplace(move(*rhs));
    
            return *this;
        }

        template<class ...Args> requires is_constructible_v<T, Args...>
        T& emplace(Args&& ...args) {
            *this = nullopt;
            val.emplace(forward<Args>(args)...);
            return *this;
        }

        template<class U, class ...Args> requires is_constructible_v<T, initializer_list<U>&, Args...>
        T& emplace(initializer_list<U> il, Args&& ...args) {
            *this = nullopt;
            val.emplace(forward<Args>(args)...);
            return *this;
        }

        void swap(optional& rhs) noexcept(noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_swappable_v<T>))
            requires is_swappable_v<T&> {
            if (*this && rhs) swap(*(*this), *rhs);
            else if (*this && !rhs) { 
                rhs.val.emplace(move(*(*this)));
                *this = nullopt;
            } else if (!*this && rhs) {
                val.emplace(move(*rhs));
                rhs = nullopt;
            }
        }

        constexpr const T* operator->() const { return addressof(val.val); }
        constexpr T* operator->() { return addressof(val.val); }
        constexpr const T& operator*() const& { return val.val; }
        constexpr T& operator*() & { return val.val; }
        constexpr const T&& operator*() const&& { return move(val.val); }
        constexpr T&& operator*() && { return move(val.val); }
        constexpr explicit operator bool() const noexcept { return val.has_value; }
        constexpr bool has_value() const noexcept { return val.has_value; }
        constexpr const T& value() const& { return bool(*this) ? val.val : throw bad_optional_access(); }
        constexpr T& value() & { return bool(*this) ? val.val : throw bad_optional_access(); }
        constexpr T&& value() && { return bool(*this) ? move(val.val) : throw bad_optional_access(); }
        constexpr const T&& value() const&& { return bool(*this) ? move(val.val) : throw bad_optional_access(); }
        template<class U> requires is_copy_constructible_v<T> && is_convertible_v<U&&, T>
        constexpr T value_or(U&& v) const& { return bool(*this) ? val.val : static_cast<T>(forward<U>(v)); }
        template<class U> requires is_move_constructible_v<T> && is_convertible_v<U&&, T>
        constexpr T value_or(U&& v) && { return bool(*this) ? move(val.val) : static_cast<T>(forward<U>(v)); }

        void reset() noexcept { val.reset(); }
    };

    template<class T> optional(T) -> optional<T>;

    /* 20.6.6 relational operators */
    template<class T, class U> requires requires (const optional<T>& x, const optional<T>& y) { {*x == *y} -> convertible_to<bool>; }
    constexpr bool operator==(const optional<T>& x, const optional<U>& y) {
        return bool(x) != bool(y) ? false
            : bool(x) == false ? true
            : *x == *y;
    }

    template<class T, class U> requires requires (const optional<T>& x, const optional<T>& y) { {*x != *y} -> convertible_to<bool>; }
    constexpr bool operator!=(const optional<T>& x, const optional<U>& y) {
        return bool(x) != bool(y) ? true
            : bool(x) == false ? false
            : *x != *y;
    }

    template<class T, class U> requires requires (const optional<T>& x, const optional<T>& y) { {*x < *y} -> convertible_to<bool>; }
    constexpr bool operator<(const optional<T>& x, const optional<U>& y) {
        return !y ? false : !x ? true : *x < *y;
    }

    template<class T, class U> requires requires (const optional<T>& x, const optional<T>& y) { {*x > *y} -> convertible_to<bool>; }
    constexpr bool operator>(const optional<T>& x, const optional<U>& y) {
        return !x ? false : !y ? true : *x > *y;
    }

    template<class T, class U> requires requires (const optional<T>& x, const optional<T>& y) { {*x <= *y} -> convertible_to<bool>; }
    constexpr bool operator<=(const optional<T>& x, const optional<U>& y) {
        return !x ? true : !y ? false : *x <= *y;
    }

    template<class T, class U>  requires requires (const optional<T>& x, const optional<T>& y) { {*x >= *y} -> convertible_to<bool>; }
    constexpr bool operator>=(const optional<T>& x, const optional<U>& y) {
        return !y ? true : !x ? false : *x >= *y;
    }

    template<class T, three_way_comparable_with<T> U>
    constexpr compare_three_way_result_t<T, U> operator<=>(const optional<T>& x, const optional<U>& y) {
        return x && y ? *x <=> *y : bool(x) <=> bool(y);
    }

    /* 20.6.7 Comparison with nullopt */
    template<class T> constexpr bool operator==(const optional<T>&x, nullopt_t) noexcept { return !x; }
    template<class T> constexpr strong_ordering operator<=>(const optional<T>& x, nullopt_t) noexcept { return bool(x) <=> false; }

    /* 20.6.8 Comparison with T */
    template<class T, class U> requires requires (const optional<T>& x, const U& v) { {*x == v} -> convertible_to<bool>; }
    constexpr bool operator==(const optional<T>& x, const U& v) { return bool(x) ? *x == v : false; }
    template<class T, class U> requires requires (const optional<T>& x, const U& v) { {v == *x} -> convertible_to<bool>; }
    constexpr bool operator==(const optional<T>& x, const U& v) { return bool(x) ? v == *x : false; }
    template<class T, class U> requires requires (const optional<T>& x, const U& v) { {*x != v} -> convertible_to<bool>; }
    constexpr bool operator!=(const optional<T>& x, const U& v) { return bool(x) ? *x != v : true; }
    template<class T, class U> requires requires (const optional<T>& x, const U& v) { {v != *x} -> convertible_to<bool>; }
    constexpr bool operator!=(const optional<T>& x, const U& v) { return bool(x) ? v != *x : true; }
    template<class T, class U> requires requires (const optional<T>& x, const U& v) { {*x < v} -> convertible_to<bool>; }
    constexpr bool operator<(const optional<T>& x, const U& v) { return bool(x) ? *x < v : true; }
    template<class T, class U> requires requires (const optional<T>& x, const U& v) { {v < *x} -> convertible_to<bool>; }
    constexpr bool operator<(const optional<T>& x, const U& v) { return bool(x) ? v < *x : false; }
    template<class T, class U> requires requires (const optional<T>& x, const U& v) { {*x > v} -> convertible_to<bool>; }
    constexpr bool operator>(const optional<T>& x, const U& v) { return bool(x) ? *x > v : false; }
    template<class T, class U> requires requires (const optional<T>& x, const U& v) { {v > *x} -> convertible_to<bool>; }
    constexpr bool operator>(const optional<T>& x, const U& v) { return bool(x) ? v > *x : true; }
    template<class T, class U> requires requires (const optional<T>& x, const U& v) { {*x <= v} -> convertible_to<bool>; }
    constexpr bool operator<=(const optional<T>& x, const U& v) { return bool(x) ? *x <= v : true; }
    template<class T, class U> requires requires (const optional<T>& x, const U& v) { {v <= *x} -> convertible_to<bool>; }
    constexpr bool operator<=(const optional<T>& x, const U& v) { return bool(x) ? v <= *x : false; }
    template<class T, class U> requires requires (const optional<T>& x, const U& v) { {*x >= v} -> convertible_to<bool>; }
    constexpr bool operator>=(const optional<T>& x, const U& v) { return bool(x) ? *x >= v : false; }
    template<class T, class U> requires requires (const optional<T>& x, const U& v) { {v >= *x} -> convertible_to<bool>; }
    constexpr bool operator>=(const optional<T>& x, const U& v) { return bool(x) ? v >= *x : true; }

    template<class T, three_way_comparable_with<T> U> constexpr compare_three_way_result_t<T, U> operator<=>(const optional<T>& x, const U& v) {
        return bool(x) ? *x <=> v : strong_ordering::less;
    }

    /* 20.6.9 Specialized algorithms */
    template<class T> requires is_move_constructible_v<T> && is_swappable_v<T>
    void swap(optional<T>& x, optional<T>& y) noexcept(noexcept(x.swap(y))) { x.swap(y); }

    template<class T> constexpr optional<decay_t<T>> make_optional(T&& v) { return optional<decay_t<T>>(forward<T>(v)); }

    template<class T, class ...Args> constexpr optional<T> make_optional(Args&& ...args) { return optional<T>(in_place, forward<Args>(args)...); }
    template<class T, class U, class ...Args> constexpr optional<T> make_optional(initializer_list<U> il, Args&& ...args) {
        return optional<T>(in_place, il, forward<Args>(args)...);
    }

    /* 20.6.10 Hash support */
    template<class T> requires is_default_constructible_v<hash<remove_const_t<T>>>
    struct hash<optional<T>> {
        size_t operator()(const optional<T>& key) {
            return key ? hash<remove_const_t<T>>()(*key) : 0;
        }
    };
}