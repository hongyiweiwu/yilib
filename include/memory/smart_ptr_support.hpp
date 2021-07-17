#pragma once

#include "memory/shared_ptr.hpp"
#include "memory/weak_ptr.hpp"
#include "memory/unique_ptr.hpp"

#include "functional.hpp"
#include "utility.hpp"
#include "type_traits.hpp"

namespace std {
    /* 20.11.5 Class template owner_less */
    template<class T = void> struct owner_less;

    template<class T> struct owner_less<shared_ptr<T>> {
        bool operator()(const shared_ptr<T>& x, const shared_ptr<T>& y) const noexcept { return x.owner_before(y); }
        bool operator()(const shared_ptr<T>& x, const weak_ptr<T>& y) const noexcept { return x.owner_before(y); }
        bool operator()(const weak_ptr<T>& x, const shared_ptr<T>& y) const noexcept { return x.owner_before(y); }
    };

    template<class T> struct owner_less<weak_ptr<T>> {
        bool operator()(const weak_ptr<T>& x, const weak_ptr<T>& y) const noexcept { return x.owner_before(y); }
        bool operator()(const shared_ptr<T>& x, const weak_ptr<T>& y) const noexcept { return x.owner_before(y); }
        bool operator()(const weak_ptr<T>& x, const shared_ptr<T>& y) const noexcept { return x.owner_before(y); }
    };

    template<> struct owner_less<void> {
        template<class T, class U> bool operator()(const shared_ptr<T>& x, const shared_ptr<U>& y) const noexcept { return x.owner_before(y); }
        template<class T, class U> bool operator()(const shared_ptr<T>& x, const weak_ptr<U>& y) const noexcept { return x.owner_before(y); }
        template<class T, class U> bool operator()(const weak_ptr<T>& x, const shared_ptr<U>& y) const noexcept { return x.owner_before(y); }
        template<class T, class U> bool operator()(const weak_ptr<T>& x, const weak_ptr<U>& y) const noexcept { return x.owner_before(y); }

        using is_transparent = void;
    };

    /* 20.11.6 Class template enable_shared_from_this */
    template<class T> class enable_shared_from_this {
    protected:
        constexpr enable_shared_from_this() noexcept = default;
        enable_shared_from_this(const enable_shared_from_this&) noexcept : weak_this() {};
        enable_shared_from_this& operator=(enable_shared_from_this&) noexcept { return *this; }
        ~enable_shared_from_this() = default;

    public:
        shared_ptr<T> shared_from_this() { return shared_ptr<T>(weak_this); }
        shared_ptr<T const> shared_from_this() const { return shared_ptr<T>(weak_this); }
        weak_ptr<T> weak_from_this() noexcept { return weak_this; }
        weak_ptr<T const> weak_from_this() const noexcept { return weak_this; }

    private:
        mutable weak_ptr<T> weak_this;
    };

    /* 20.11.7 Smart pointer hash support */
    template<class T, class D> requires is_default_constructible_v<hash<typename unique_ptr<T, D>::pointer>>
    struct hash<unique_ptr<T, D>> : hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const unique_ptr<T, D>& ptr) const {
            return hash<typename unique_ptr<T, D>::pointer>()(ptr.get());
        }
    };

    template<class T> struct hash<shared_ptr<T>> : hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const shared_ptr<T>& ptr) const {
            return hash<typename shared_ptr<T>::element_type*>()(ptr.get());
        } 
    };
}