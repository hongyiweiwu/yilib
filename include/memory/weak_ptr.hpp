#pragma once

#include "memory/shared_ptr.hpp"
#include "type_traits.hpp"
#include "utility.hpp"

namespace std {
    /* 20.11.4 Class template weak_ptr */
    template<class T>
    class weak_ptr {
    public:
        template<class U> friend class shared_ptr;

        using element_type = remove_extent_t<T>;

    private:
        __internal::__ctrl* ctrl;
        element_type* ptr;

    public:
        /* 20.11.4.2 Constructors */
        constexpr weak_ptr() noexcept : ctrl(nullptr), ptr(nullptr) {}
        template<class Y> requires is_convertible_v<Y*, T*> || (is_bounded_array_v<Y> && is_same_v<remove_cv_t<T>, remove_cv_t<remove_all_extents_t<Y>>[]>)
        weak_ptr(const weak_ptr<Y>& r) noexcept : ctrl(r.ctrl) {
            if (ctrl) {
                ptr = r.ptr;
                ctrl->increment_weak_count();
            }
        }

        weak_ptr(const weak_ptr& r) noexcept : ctrl(r.ctrl) {
            if (ctrl) {
                ptr = r.ptr;
                ctrl->increment_weak_count();
            }
        }

        template<class Y> requires is_convertible_v<Y*, T*> || (is_bounded_array_v<Y> && is_same_v<remove_cv_t<T>, remove_cv_t<remove_all_extents_t<Y>>[]>)
        weak_ptr(const shared_ptr<Y>& r) noexcept : ctrl(r.ctrl) {
            if (ctrl) {
                ptr = r.ptr;
                ctrl->increment_weak_count();
            }
        }

        weak_ptr(weak_ptr&& r) noexcept : ptr(r.ptr), ctrl(r.ctrl) {
            r.ptr = nullptr;
            r.ctrl = nullptr;
        }

        template<class Y> requires is_convertible_v<Y*, T*> || (is_bounded_array_v<Y> && is_same_v<remove_cv_t<T>, remove_cv_t<remove_all_extents_t<Y>>[]>)
        weak_ptr(weak_ptr<Y>&& r) noexcept : ptr(r.ptr), ctrl(r.ctrl) {
            r.ptr = nullptr;
            r.ctrl = nullptr;
        }

        /* 20.11.4.3 Destructor */
        ~weak_ptr() { ctrl->decrement_weak_count(); }

        /* 20.11.4.4 Assignment */
        weak_ptr& operator=(const weak_ptr& r) noexcept {
            weak_ptr(r).swap(*this);
            return *this;
        }

        template<class Y> weak_ptr& operator=(const weak_ptr<Y>& r) noexcept {
            weak_ptr(r).swap(*this);
            return *this;
        }

        template<class Y> weak_ptr& operator=(const shared_ptr<Y>& r) noexcept {
            weak_ptr(r).swap(*this);
            return *this;
        }

        weak_ptr& operator=(weak_ptr&& r) noexcept {
            weak_ptr(move(r)).swap(*this);
            return *this;
        }

        template<class Y> weak_ptr& operator=(weak_ptr<Y>&& r) noexcept {
            weak_ptr(move(r)).swap(*this);
            return *this;
        }

        /* 20.11.4.5 Modifiers */
        void swap(weak_ptr& r) noexcept {
            std::swap(ptr, r.ptr);
            std::swap(ctrl, r.ctrl);
        }

        void reset() noexcept { weak_ptr().swap(*this); }

        /* 20.11.4.6 Observers */
        long use_count() const noexcept { 
            return ctrl ? ctrl->get_shared_count() : 0;
        }

        bool expired() const noexcept { return use_count() == 0; }
        shared_ptr<T> lock() const noexcept {
            // TODO: Make this function atomic.
            return expired() ? shared_ptr<T>() : shared_ptr<T>(*this);
        }

        template<class U> bool owner_before(const shared_ptr<U>& b) const noexcept {
            return ctrl < b.ctrl;
        }

        template<class U> bool owner_before(const weak_ptr<U>& b) const noexcept {
            return ctrl < b.ctrl;
        }
    };

    template<class T> weak_ptr(shared_ptr<T>) -> weak_ptr<T>;

    /* 20.11.4.7 Specialized algorithms */
    template<class T> void swap(weak_ptr<T>& a, weak_ptr<T>& b) noexcept { a.swap(b); }
}