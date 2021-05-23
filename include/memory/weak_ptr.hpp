#pragma once

#include "memory/shared_ptr.hpp"
#include "type_traits.hpp"

namespace std {
    /* 20.11.4 Class template weak_ptr */
    template<class T>
    class weak_ptr {
    private:
        __internal::__ctrl* ctrl;
        shared_ptr<T>* parent;
    public:
        using element_type = remove_extent_t<T>;

        /* 20.11.4.2 Constructors */
        constexpr weak_ptr() noexcept : ctrl(nullptr), parent(nullptr) {}
    };

    /* 20.11.6 Class template enable_shared_from_this */
    template<class T> class enable_shared_from_this {
    protected:
        constexpr enable_shared_from_this() noexcept : weak_this() {};
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
}