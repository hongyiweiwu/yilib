#pragma once

#include "exception.hpp"
#include "stdatomic.h"
#include "cstddef.hpp"
#include "functional.hpp"
#include "util/always_false.hpp"
#include "new.hpp"

#include "memory/unique_ptr.hpp"
#include "memory/pointer_util.hpp"
#include "memory/allocators.hpp"

namespace std {
    /* 20.11.2 Class bad_weak_ptr */
    class bad_weak_ptr : public exception {
    public:
        const char* what() const noexcept override;
    };

    /* 20.11.3 Class template shared_ptr */
    namespace __internal {
        struct __ctrl {
        protected:
            // Current number of weak_ptrs holding this block, plus 1 if there's any shared_ptr holding the block.
            _Atomic long weak_count;
            // Number of shared_ptrs holding this block.
            _Atomic long shared_count;
        public:
            // A block is always constructed by a shared_ptr, so we always initialize both counters by 1.
            __ctrl() noexcept {
                atomic_init(&weak_count, 1);
                atomic_init(&shared_count, 1);
            }

            /* Returns whether this control block still holds an object. */
            virtual bool is_empty() const noexcept = 0;

            /* Returns the weak_count. */
            long get_weak_count() const noexcept { return atomic_load(&weak_count); }
            /* Returns the shared_count */
            long get_shared_count() const noexcept { return atomic_load(&shared_count); }

            /* Decrements the shared_count by 1. If the shared_count reaches 0, decrements weak_count as well (and triggers all consequences in decrement_weak_count).
             * The operation on each counter is atomic, but this function is not. */
            long decrement_shared_count() noexcept {
                if (is_empty()) return 0;

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

            virtual ~__ctrl() noexcept {}
        };

        template<class T>
        struct __ctrl_ptr : public __ctrl {
        protected:
            T* ptr;
        public:
            __ctrl_ptr(T* ptr) noexcept : __ctrl(), ptr(ptr) {}

            bool is_empty() const noexcept override { return !ptr; }
            void delete_content() noexcept override { delete ptr; }
        };

        template<class T, class D>
        struct __ctrl_ptr_with_deleter : public __ctrl_ptr<T> {
        private:
            D deleter;
        public:
            __ctrl_ptr_with_deleter(T* ptr, D&& deleter) noexcept
                : __ctrl_ptr<T>(ptr), deleter(move(deleter)) {}

            void delete_content() noexcept override { 
                deleter(__ctrl_ptr<T>::ptr);
            }
        };

        template<class T, class Alloc, class D>
        struct __ctrl_ptr_with_deleter_alloc : public __ctrl_ptr_with_deleter<T, D> {
        private:
            Alloc alloc;
        public:
            using allocator_type = typename allocator_traits<Alloc>::template rebind_alloc<__ctrl_ptr_with_deleter_alloc>;

            __ctrl_ptr_with_deleter_alloc(T* ptr, D&& deleter, Alloc&& alloc) noexcept
                : __ctrl_ptr_with_deleter<T, D>(ptr, move(deleter)), alloc(move(alloc)) {}

            void delete_block() noexcept {
                allocator_type temp(alloc);
                allocator_traits<allocator_type>::destroy(temp, this);
                allocator_traits<allocator_type>::deallocate(temp, this, 1);
            }
        };

        template<class T>
        struct __ctrl_obj : public __ctrl {
        private:
            aligned_storage_t<sizeof(T), alignof(T)> storage;
            _Atomic bool is_freed;
        public:
            template<class ...Args> // requires is_constructible_v<T, Args...>
            __ctrl_obj(Args&& ...args) noexcept : __ctrl(), storage() {
                new (static_cast<void*>(&storage)) T(forward<Args>(args)...);
                atomic_init(&is_freed, false);
            }

            void delete_content() noexcept override {
                atomic_store(&is_freed, true);

                if constexpr (is_array_v<T>) {
                    destroy_at(launder(reinterpret_cast<T*>(&storage)));
                } else {
                    launder(reinterpret_cast<T*>(&storage))->~T();
                }
            }

            bool is_empty() const noexcept override {
                return atomic_load(&is_freed);
            }

            remove_extent_t<T>* get_object_ptr() noexcept { 
                return launder(reinterpret_cast<remove_extent_t<T>*>(&storage)); 
            }
        };

        template<class T, class Alloc>
        struct __ctrl_obj_with_alloc : public __ctrl_obj<T> {
        private:
            Alloc a;
        public:
            using allocator_type = typename allocator_traits<Alloc>::template rebind_alloc<__ctrl_ptr_with_deleter_alloc>;

            template<class ...Args> requires is_constructible_v<T, Args...>
            __ctrl_obj_with_alloc(Alloc&& a, Args&& ...args) noexcept 
                : __ctrl_obj<T>(forward<Args>(args)...), a(move(a)) {}

            void delete_block() noexcept {
                allocator_type temp(a);
                allocator_traits<allocator_type>::destroy(temp, this);
                allocator_traits<allocator_type>::deallocate(temp, this, 1);
            }
        };
    }


    // Forward declaration. Implemented directly below.
    template<class T> class weak_ptr;

    template<class T> class shared_ptr {
    public:
        using element_type = remove_extent_t<T>;
        using weak_type = weak_ptr<T>;
    private:
        template<class Y>
        friend class weak_ptr;

        element_type* ptr;
        __internal::__ctrl* ctrl;

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
                ctrl = new __internal::__ctrl_ptr_with_deleter<T, void (*)(element_type*)>(p, [](element_type* p) { delete[] p; });
            } catch (const exception& e) {
                delete[] p;
                throw;
            }
        }

        template<class Y> requires __internal::is_complete<Y>::value && (!is_array_v<T>)
            && is_convertible_v<Y*, T*> && requires (Y* p) { { delete p } noexcept; }
        explicit shared_ptr(Y* p) : ptr(p) {
            try {
                ctrl = new __internal::__ctrl_ptr<T>(p);
            } catch (const exception& e) {
                delete p;
                throw;
            }
        }

        template<class Y, class D> requires is_nothrow_move_constructible_v<D> 
            && ((is_unbounded_array_v<T> && is_nothrow_convertible_v<Y(*)[], T*>) 
                || (is_bounded_array_v<T> && is_nothrow_convertible_v<Y(*)[extent_v<T>], T*>)
                || (is_nothrow_convertible_v<Y*, T*>))
            && requires (Y* p, D d) { { d(p) } noexcept; }
        shared_ptr(Y* p, D d) : ptr(p) {
            try {
                ctrl = new __internal::__ctrl_ptr_with_deleter<T, D>(p, move(d));
            } catch (const exception& e) {
                d(p);
                throw;
            }
        }

        template<class Y, class D, class A> requires is_nothrow_move_constructible_v<D> 
            && ((is_unbounded_array_v<T> && is_nothrow_convertible_v<Y(*)[], T*>) 
                || (is_bounded_array_v<T> && is_nothrow_convertible_v<Y(*)[extent_v<T>], T*>)
                || (is_nothrow_convertible_v<Y*, T*>))
            && requires (Y* p, D d) { { d(p) } noexcept; }
        shared_ptr(Y* p, D d, A a) : ptr(p) {
            try {
                using allocator_type = typename __internal::__ctrl_ptr_with_deleter_alloc<T, A, D>::allocator_type;
                allocator_type alloc = a;

                auto ptr = allocator_traits<allocator_type>::allocate(alloc, 1);
                allocator_traits<allocator_type>::construct(alloc, ptr, p, move(d), move(a));

                ctrl = ptr;
            } catch (const exception& e) {
                d(p);
                throw;
            }
        }

        template<class D> requires is_nothrow_move_constructible_v<D> && requires (nullptr_t p, D d) { { d(p) } noexcept; }
        shared_ptr(nullptr_t p, D d) : ptr(p) {
            try {
                ctrl = new __internal::__ctrl_ptr_with_deleter<T, D>(p, move(d));
            } catch (const exception& e) {
                d(p);
                throw;
            }
        }

        template<class D, class A> requires is_nothrow_move_constructible_v<D> && requires (nullptr_t p, D d) { { d(p) } noexcept; }
        shared_ptr(nullptr_t p, D d, A a) : ptr(p) {
            try {
                using allocator_type = typename __internal::__ctrl_ptr_with_deleter_alloc<T, A, D>::allocator_type;
                allocator_type alloc = a;

                auto ptr = allocator_traits<allocator_type>::allocate(alloc, 1);
                allocator_traits<allocator_type>::construct(alloc, ptr, p, move(d), move(a));

                ctrl = ptr;
            } catch (const exception& e) {
                d(p);
                throw;
            }
        }

        template<class Y> shared_ptr(const shared_ptr<Y>& r, element_type* p) noexcept
            : ptr(p), ctrl(r.ctrl) { if (ctrl) ctrl->increment_shared_count(); }

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
                ctrl = new __internal::__ctrl_ptr_with_deleter<T, D>(ptr, r.get_deleter());
                ptr = r.release();
            } else {
                ctrl = new __internal::__ctrl_ptr_with_deleter<T, D>(ptr, ref(r.get_deleter()));
                ptr = r.release();
            }
        }

        /* Make a new shared_ptr by simply assigning its member variables with the provided ones. This is only used internally for implementing the
         * make_shared series of functions. */
        static shared_ptr<T> __make_shared(__internal::__ctrl_obj<T>* ctrl) {
            auto result = shared_ptr<T>();
            result.ptr = ctrl->get_object_ptr();
            result.ctrl = ctrl;
            return result;
        }

        /* 20.11.3.3 Destructor */
        ~shared_ptr() {
            if (ctrl) ctrl->decrement_shared_count();
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

    /* 20.11.3.7 Creation */
    // TODO: Implement support for U[].
    template<class T, class ...Args> requires (!is_array_v<T>)
    shared_ptr<T> make_shared(Args&&... args) {
        return shared_ptr<T>::__make_shared(new __internal::__ctrl_obj<T>(forward<Args>(args)...));
    }

    template<class T, class A, class ...Args> requires (!is_array_v<T>)
    shared_ptr<T> allocate_shared(const A& a, Args&& ...args) {
        using allocator_type = typename __internal::__ctrl_obj_with_alloc<T, A>::allocator_type;
        allocator_type alloc = a;

        auto ctrl = allocator_traits<allocator_type>::allocate(alloc, 1);
        allocator_traits<allocator_type>::construct(alloc, ctrl, forward<Args>(args)...);
        return shared_ptr<T>::__make_shared(ctrl);
    }

    template<class T> requires is_bounded_array_v<T>
    shared_ptr<T> make_shared() {
        // The array should be value initialized in the ctrl_obj constructor.
        return shared_ptr<T>::__make_shared(new __internal::__ctrl_obj<T>());
    }

    template<class T, class A> requires is_bounded_array_v<T>
    shared_ptr<T> allocate_shared(const A& a) {
        using allocator_type = typename __internal::__ctrl_obj_with_alloc<T, A>::allocator_type;
        allocator_type alloc = a;

        auto ctrl = allocator_traits<allocator_type>::allocate(alloc, 1);
        allocator_traits<allocator_type>::construct(alloc, ctrl);
        return shared_ptr<T>::__make_shared(ctrl);
    }

    template<class T> requires is_bounded_array_v<T>
    shared_ptr<T> make_shared(const remove_extent_t<T>& u) {
        auto ctrl = new __internal::__ctrl_obj<T>();

        for (auto curr = ctrl->get_object_ptr(); curr != ctrl->get_object_ptr() + sizeof(T); curr++) {
            construct_at(curr, u);
        }

        return shared_ptr<T>::__make_shared(ctrl);
    }

    template<class T, class A> requires is_bounded_array_v<T>
    shared_ptr<T> make_shared(const A& a, const remove_extent_t<T>& u) {
        using allocator_type = typename __internal::__ctrl_obj_with_alloc<T, A>::allocator_type;
        allocator_type alloc = a;

        auto ctrl = allocator_traits<allocator_type>::allocate(alloc, 1);
        for (auto curr = ctrl->get_object_ptr(); curr != ctrl->get_object_ptr() + sizeof(T); curr++) {
            construct_at(curr, u);
        }

        return shared_ptr<T>::__make_shared(ctrl);
    }

    template<class T> requires (!is_unbounded_array_v<T>)
    shared_ptr<T> make_shared_for_overwrite() { return make_shared<T>(); }

    template<class T, class A> requires (!is_bounded_array_v<T>)
    shared_ptr<T> allocate_shared_for_overwrite(const A& a) { return make_shared<T>(a); }
}