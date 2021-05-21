#pragma once

#include "exception.hpp"
#include "stdatomic.h"
#include "cstddef.hpp"
#include "functional.hpp"
#include "util/always_false.hpp"

#include "memory/unique_ptr.hpp"
#include "memory/pointer_util.hpp"
#include "memory/allocators.hpp"

#include "stdio.h"

namespace std {
    /* 20.11.2 Class bad_weak_ptr */
    class bad_weak_ptr : public exception {
    public:
        const char* what() const noexcept override;
    };

    /* 20.11.3 Class template shared_ptr */
    // Forward declaration. Implemented directly below.
    template<class T> class weak_ptr;

    template<class T> class shared_ptr {
    public:
        using element_type = remove_extent_t<T>;
        using weak_type = weak_ptr<T>;
    private:
        template<class Y>
        friend class weak_ptr;

        struct __control_block {
        protected:
            // Current number of weak_ptrs holding this block, plus 1 if there's any shared_ptr holding the block.
            _Atomic long weak_count;
            // Number of shared_ptrs holding this block.
            _Atomic long shared_count;
        public:
            // A block is always constructed by a shared_ptr, so we always initialize both counters by 1.
            __control_block() noexcept {
                atomic_init(&weak_count, 1);
                atomic_init(&shared_count, 1);
            }

            /* Returns whether this control block still holds an object. */
            virtual bool is_empty() const noexcept = 0;

            /* Returns the weak_count. */
            long get_weak_count() const noexcept { return atomic_load(&weak_count); }
            /* Returns the shared_count */
            long get_shared_count() const noexcept { return atomic_load(&shared_count); }

            /* Decrements the shared_count by 1. If the shared_count reaches 0, decremenets weak_count as well (and triggers all consequences in decrement_weak_count).
             * The operation on each counter is atomic, but this function is not. */
            long decrement_shared_count() noexcept {
                const auto count = atomic_fetch_sub(&shared_count, 1);
                if (!count) {
                    delete_content();
                    decrement_weak_count();
                }

                return count;
            }

            /* Decrements the weak_count by 1. If the weak_count reaches 0, the control block is self-deleted and deallocated. */
            long decrement_weak_count() noexcept {
                const auto count = atomic_fetch_sub(&weak_count, 1);
                if (!count) delete_block();

                return count;
            }

            /* Increments shared_count by 1 if the current block still holds an object. Returns the result shared_count. */
            virtual long increment_shared_count() noexcept {
                if (!is_empty()) return atomic_fetch_add(&shared_count, 1);
                return 0;
            }

            /* Deletes the content held by the pointers holding this control block. This function is executed immediately when shared_count hits 0. Depending on what
             * type of control block this is, the behavior of this function is different. */
            virtual void delete_content() noexcept = 0;
            /* Deletes the entire block. The block will be nonexistent after this call and its memory freed. This is called when weak_count hits 0. */
            virtual void delete_block() noexcept { delete this; }

            virtual ~__control_block() noexcept {}
        };

        struct __control_block_ptr : public __control_block {
        protected:
            element_type* ptr;
        public:
            __control_block_ptr(element_type* ptr) noexcept : __control_block(), ptr(ptr) {}

            bool is_empty() const noexcept override { return !ptr; }
            void delete_content() noexcept override { delete ptr; }
        };

        template<class D>
        struct __control_block_ptr_with_deleter : public __control_block_ptr {
        private:
            D deleter;
        public:
            __control_block_ptr_with_deleter(element_type* ptr, D&& deleter) noexcept
                : __control_block_ptr(ptr), deleter(move(deleter)) {}

            void delete_content() noexcept override { 
                deleter(__control_block_ptr::ptr);
            }
        };

        template<class Alloc, class D>
        struct __control_block_ptr_with_deleter_alloc : public __control_block_ptr_with_deleter<D> {
        private:
            Alloc alloc;
        public:
            __control_block_ptr_with_deleter_alloc(element_type* ptr, D&& deleter, const Alloc& alloc) noexcept
                : __control_block_ptr_with_deleter<D>(ptr, move(deleter)), alloc(alloc) {}

            void delete_block() noexcept {
                typename allocator_traits<Alloc>::template rebind_alloc<__control_block_ptr_with_deleter_alloc> temp(alloc);
                temp.deallocate(this, 1);
            }
        };

        element_type* ptr;
        __control_block* ctrl;

    public:
        /* 20.11.3.5 Modifiers */
        void swap(shared_ptr& r) noexcept {
            swap(ptr, r.ptr);
            swap(ctrl, r.ctrl);
        }

        void reset() noexcept { shared_ptr().swap(*this); }
        template<class Y> void reset(Y* p) { shared_ptr(p).swap(*this); }
        template<class Y, class D> void reset(Y* p, D d) { shared_ptr(p, d).swap(*this); }
        template<class Y, class D, class A> void reset(Y* p, D d, A a) { shared_ptr(p, d, a).swap(*this); }

        /* 20.11.3.6 Observers */
        element_type* get() const noexcept { return ptr; }

        auto operator*() const noexcept -> decltype(auto) { 
            if constexpr (is_void_v<T> || is_array_v<T>) {
                static_assert(__internal::always_false<T>, "*shared_ptr<T> can only be called when T is not an array type or cv void.");
            } else {
                return *get();
            }
        }

        auto operator->() const noexcept {
            if constexpr (is_array_v<T>) {
                static_assert(__internal::always_false<T>, "shared_ptr<T>-> can only be called when T is not an array type.");
            } else {
                return get();
            }
        }

        element_type& operator[](ptrdiff_t i) const {
            if constexpr (!is_array_v<T>) {
                static_assert(__internal::always_false<T>, "shared_ptr<T>[] can only be called when T is an array type.");
            } else {
                return get()[i];
            }
        }

        long use_count() const noexcept {
            if (!ctrl) { return 0; }
            else return ctrl->get_shared_count();
        }

        explicit operator bool() const noexcept { return get() != nullptr; }

        template<class U> bool owner_before(const shared_ptr<U>& b) const noexcept {
            return ctrl < b.ctrl;
        }

        template<class U> bool owner_before(const weak_ptr<U>& b) const noexcept {
            return ctrl < b.ctrl;
        }
        
        /* 20.11.3.2 Constructors */
        constexpr shared_ptr() noexcept : ptr(nullptr), ctrl(nullptr) {}

        template<class Y> requires __internal::is_complete<Y>::value && is_array_v<T>
            && ((is_unbounded_array_v<T> && is_convertible_v<Y(*)[], T*> && requires (Y* p) { { delete[] p } noexcept; })
                || (is_bounded_array_v<T> && is_convertible_v<Y(*)[extent_v<T>], T*> && requires (Y* p) { { delete[] p } noexcept; }))
        explicit shared_ptr(Y* p) : ptr(p) {
            try {
                ctrl = new __control_block_ptr_with_deleter<void (*)(element_type*)>(p, [](element_type* p) { delete[] p; });
            } catch (const exception& e) {
                delete[] p;
                throw e;
            }
        }

        template<class Y> requires __internal::is_complete<Y>::value && (!is_array_v<T>)
            && is_convertible_v<Y*, T*> && requires (Y* p) { { delete p } noexcept; }
        explicit shared_ptr(Y* p) : ptr(p), ctrl(new __control_block_ptr(p)) {
            try {
                ctrl = new __control_block_ptr(p);
            } catch (const exception& e) {
                delete p;
                throw e;
            }
        }

        template<class Y, class D> requires is_nothrow_move_constructible_v<D> 
            && ((is_unbounded_array_v<T> && is_nothrow_convertible_v<Y(*)[], T*>) 
                || (is_bounded_array_v<T> && is_nothrow_convertible_v<Y(*)[extent_v<T>], T*>)
                || (is_nothrow_convertible_v<Y*, T*>))
            && requires (Y* p, D d) { { d(p) } noexcept; }
        shared_ptr(Y* p, D d) : ptr(p) {
            try {
                ctrl = new __control_block_ptr_with_deleter(p, move(d));
            } catch (const exception& e) {
                d(p);
                throw e;
            }
        }

        template<class Y, class D, class A> requires is_nothrow_move_constructible_v<D> 
            && ((is_unbounded_array_v<T> && is_nothrow_convertible_v<Y(*)[], T*>) 
                || (is_bounded_array_v<T> && is_nothrow_convertible_v<Y(*)[extent_v<T>], T*>)
                || (is_nothrow_convertible_v<Y*, T*>))
            && requires (Y* p, D d) { { d(p) } noexcept; }
        shared_ptr(Y* p, D d, A a) : ptr(p) {
            try {
                ctrl = new __control_block_ptr_with_deleter_alloc(p, move(d), move(a));
            } catch (const exception& e) {
                d(p);
                throw e;
            }
        }

        template<class D> requires is_nothrow_move_constructible_v<D> && requires (nullptr_t p, D d) { { d(p) } noexcept; }
        shared_ptr(nullptr_t p, D d) : ptr(p) {
            try {
                ctrl = new __control_block_ptr_with_deleter(p, move(d));
            } catch (const exception& e) {
                d(p);
                throw e;
            }
        }

        template<class D, class A> requires is_nothrow_move_constructible_v<D> && requires (nullptr_t p, D d) { { d(p) } noexcept; }
        shared_ptr(nullptr_t p, D d, A a) : ptr(p) {
            try {
                ctrl = new __control_block_ptr_with_deleter_alloc(p, move(d), move(a));
            } catch (const exception& e) {
                d(p);
                throw e;
            }
        }

        template<class Y> shared_ptr(const shared_ptr<Y>& r, element_type* p) noexcept
            : ptr(p), ctrl(r.ctrl) { ctrl->increment_shared_count(); }

        template<class Y> shared_ptr(shared_ptr<Y>&& r, element_type* p) noexcept
            : ptr(p), ctrl(r.ctrl) { 
                r.p = nullptr;
                r.ctrl = nullptr;
            }

        shared_ptr(const shared_ptr& r) noexcept : ptr(r.ptr), ctrl(r.ctrl) {
            if (ctrl) ctrl->increment_shared_count();
        }

        template<class Y> requires is_convertible_v<Y*, T*> || (is_bounded_array_v<Y> && is_same_v<remove_cv_t<T>, remove_cv_t<remove_all_extents_t<Y>>[]>)
        shared_ptr(const shared_ptr<Y>& r) noexcept : ptr(r.ptr), ctrl(r.ctrl) {
            if (ctrl) ctrl->increment_shared_count();
        }

        shared_ptr(shared_ptr&& r) noexcept : ptr(r.ptr), ctrl(r.ctrl) {
            r.ptr = nullptr;
            r.ctrl = nullptr;
        }

        template<class Y> requires is_convertible_v<Y*, T*> || (is_bounded_array_v<Y> && is_same_v<remove_cv_t<T>, remove_cv_t<remove_all_extents_t<Y>>[]>)
        shared_ptr(shared_ptr<Y>&& r) noexcept : ptr(r.ptr), ctrl(r.ctrl) {
            r.ptr = nullptr;
            r.ctrl = nullptr;
        }

        template<class Y> requires is_convertible_v<Y*, T*> || (is_bounded_array_v<Y> && is_same_v<remove_cv_t<T>, remove_cv_t<remove_all_extents_t<Y>>[]>)
        explicit shared_ptr(const weak_ptr<Y>& r) : ptr(r.ptr), ctrl(r.ctrl) {
            if (r.expired()) throw std::bad_weak_ptr();
            ctrl->increment_shared_count();
        }

        template<class Y, class D> requires (is_convertible_v<Y*, T*> || (is_bounded_array_v<Y> && is_same_v<remove_cv_t<T>, remove_cv_t<remove_all_extents_t<Y>>[]>))
            && is_convertible_v<typename unique_ptr<Y, D>::pointer, element_type*>
        shared_ptr(unique_ptr<Y, D>&& r) {
            if (!r.get()) {
                ptr = nullptr;
                ctrl = nullptr;
            } else if constexpr (!is_reference_v<D>) {
                ctrl = new __control_block_ptr_with_deleter(ptr, r.get_deleter());
                ptr = r.release();
            } else {
                ctrl = new __control_block_ptr_with_deleter(ptr, ref(r.get_deleter()));
                ptr = r.release();
            }
        }

        /* 20.11.3.3 Destructor */
        ~shared_ptr() {
            if (get() && ctrl) ctrl->decrement_shared_count();
        }

        /* 20.11.3.4 Assignment */
        shared_ptr& operator=(const shared_ptr& r) noexcept { 
            shared_ptr(r).swap(*this); 
            return *this;
        }

        template<class Y> shared_ptr& operator=(const shared_ptr<Y>& r) noexcept {
            shared_ptr(r).swap(*this);
            return *this;
        }

        shared_ptr& operator=(shared_ptr&& r) noexcept {
            shared_ptr(move(r)).swap(*this);
            return *this;
        }

        template<class Y> shared_ptr& operator=(shared_ptr<Y>&& r) noexcept {
            shared_ptr(move(r)).swap(*this);
            return *this;
        }

        template<class Y, class D> shared_ptr& operator=(unique_ptr<Y, D>&& r) {
            shared_ptr(move(r)).swap(*this);
            return *this;
        }
    };

    template<class T> shared_ptr(weak_ptr<T>) -> shared_ptr<T>;
    template<class T, class D> shared_ptr(unique_ptr<T, D>) -> shared_ptr<T>;
}