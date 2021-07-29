#pragma once

#include "exception.hpp"
#include "cstddef.hpp"
#include "functional.hpp"
#include "util/utility_traits.hpp"
#include "compare.hpp"
#include "new.hpp"
#include "utility.hpp"

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
            long weak_count;
            // Number of shared_ptrs holding this block.
            long shared_count;
        public:
            // A block is always constructed by a shared_ptr, so we always initialize both counters by 1.
            __ctrl() noexcept;

            /* Returns whether this control block still holds an object. */
            virtual bool is_null() const noexcept = 0;

            /* Returns the deleter that this block controls if it exists, nullptr otherwise. */
            virtual void* get_deleter() const noexcept;

            /* Returns the weak_count. */
            long get_weak_count() const noexcept;
            /* Returns the shared_count */
            long get_shared_count() const noexcept;

            /* Decrements the shared_count by 1. If the shared_count reaches 0, decrements weak_count as well (and triggers all consequences in decrement_weak_count).
             * The operation on each counter is atomic, but this function is not. */
            long decrement_shared_count() noexcept;

            /* Decrements the weak_count by 1. If the weak_count reaches 0, the control block is self-deleted and deallocated. */
            long decrement_weak_count() noexcept;

            /* Increments shared_count by 1 if the current block still holds an object. Returns the result shared_count. */
            virtual long increment_shared_count() noexcept;

            /* Increments weak_count by 1. */
            long increment_weak_count() noexcept;

            /* Deletes the content held by the pointers holding this control block. This function is executed immediately when shared_count hits 0. Depending on what
             * type of control block this is, the behavior of this function is different. */
            virtual void delete_content() noexcept = 0;
            /* Deletes the entire block. The block will be nonexistent after this call and its memory freed. This is called when weak_count hits 0. */
            virtual void delete_block() noexcept;

            virtual ~__ctrl() noexcept = default;
        };

        template<class T>
        struct __ctrl_ptr : public __ctrl {
        protected:
            T* ptr;
        public:
            __ctrl_ptr(T* ptr) noexcept : __ctrl(), ptr(ptr) {}

            bool is_null() const noexcept override { return !ptr; }
            // We don't have to check for nullptr here, because no shared_ptr can be constructed with a nullptr without a deleter, which will
            // use the overload from __ctrl_ptr_with_deleter.
            void delete_content() noexcept override { delete ptr; }
        };

        template<class T, class D>
        struct __ctrl_ptr_with_deleter : public __ctrl_ptr<T> {
        private:
            D deleter;
        public:
            __ctrl_ptr_with_deleter(T* ptr, D&& deleter) noexcept
                : __ctrl_ptr<T>(ptr), deleter(move(deleter)) {}

            D* get_deleter() const noexcept override { return addressof(deleter); }

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
        public:
            /* Tag used in constructors to specify that the constructor shouldn't construct the object in storage. */
            struct noop_t { explicit noop_t() = default; };
        private:
            aligned_storage_t<sizeof(T), alignof(T)> storage;
            bool is_freed;
        public:
            template<class ...Args>
            __ctrl_obj(Args&& ...args) noexcept : __ctrl(), storage() {
                ::new (static_cast<void*>(&storage)) T(forward<Args>(args)...);
                __atomic_store_n(&is_freed, false, __ATOMIC_SEQ_CST);
            }

            /* Noop constructor that does not attempt to construct the storage in anyway. The caller needs to get a pointer to the storage by get_object_ptr()
             * and construct the object explicitly. */
            explicit __ctrl_obj(noop_t) noexcept : storage() {
                __atomic_store_n(&is_freed, false, __ATOMIC_SEQ_CST);
            }

            void delete_content() noexcept override {
                __atomic_store_n(&is_freed, true, __ATOMIC_SEQ_CST);
                destroy_at(launder(reinterpret_cast<T*>(&storage)));
            }

            bool is_null() const noexcept override {
                return __atomic_load_n(&is_freed, __ATOMIC_SEQ_CST);
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

            template<class ...Args>
            __ctrl_obj_with_alloc(Alloc&& a, Args&& ...args) noexcept 
                : __ctrl_obj<T>(typename __ctrl_obj<T>::noop_t{}), a(move(a)) {
                if constexpr (!is_array_v<T>) {
                    // It's the responsible of the caller to initialize arrays.
                    using allocator_type = typename allocator_traits<Alloc>::template rebind_alloc<remove_cv_t<T>>;
                    allocator_type temp(a);
                    allocator_traits<allocator_type>::construct(temp, reinterpret_cast<remove_cv_t<T>*>(&__ctrl_obj<T>::storage), forward<Args>(args)...);
                }
            }

            __ctrl_obj_with_alloc(typename __ctrl_obj<T>::noop_t) noexcept : __ctrl_obj<T>(typename __ctrl_obj<T>::noop_t{}), a(move(a)) {}

            void delete_content() noexcept override {
                atomic_store(&__ctrl_obj<T>::is_freed, true);
                if constexpr (is_array_v<T>) {
                    destroy_at(launder(reinterpret_cast<T*>(&__ctrl_obj<T>::storage)));
                } else {
                    using allocator_type = typename allocator_traits<Alloc>::template rebind_alloc<remove_cv_t<T>>;
                    allocator_type temp(a);
                    allocator_traits<allocator_type>::destroy(temp, launder(reinterpret_cast<remove_cv<T>*>(&__ctrl_obj<T>::storage)));
                }
            }

            void delete_block() noexcept {
                allocator_type temp(a);
                allocator_traits<allocator_type>::destroy(temp, this);
                allocator_traits<allocator_type>::deallocate(temp, this, 1);
            }
        };
    }

    // Forward declaration. Implemented directly below.
    template<class T> class weak_ptr;
    template<class T> class enabled_shared_from_this;

    namespace __internal {
        template<class T> auto __enable_shared_from_this_test(enabled_shared_from_this<T>&) -> true_type;
        template<class ...> auto __enable_shared_from_this_test(...) -> false_type;
    }

    template<class T> class shared_ptr {
    public:
        using element_type = remove_extent_t<T>;
        using weak_type = weak_ptr<T>;
    private:
        template<class Y>
        friend class weak_ptr;

        element_type* ptr;
        __internal::__ctrl* ctrl;

        /* If U has an unambiguous and accessible base class that is a specialization of std::enable_shared_from_this, replaces the weak_this member
         * of the pointer with a newly constructed alias shared_ptr that shares a control block with this, but "owns" ptr. */
        template<class U> void __enable_shared_from_this(U* ptr) noexcept {
            if constexpr (is_object_v<U> && decltype(__internal::__enable_shared_from_this_test(declval<U&>()))::value) {
                if (ptr != nullptr && ptr->weak_this.expired()) {
                    ptr->weak_this = shared_ptr<remove_cv_t<U>>(*this, const_cast<remove_cv_t<U>*>(ptr));
                }
            }
        }
    public:
        /* 20.11.3.5 Modifiers */
        void swap(shared_ptr& r) noexcept {
            std::swap(ptr, r.ptr);
            std::swap(ctrl, r.ctrl);
        }

        void reset() noexcept { shared_ptr().swap(*this); }
        template<class Y> void reset(Y* p) { shared_ptr(p).swap(*this); }
        template<class Y, class D> void reset(Y* p, D d) { shared_ptr(p, d).swap(*this); }
        template<class Y, class D, class A> void reset(Y* p, D d, A a) { shared_ptr(p, d, a).swap(*this); }

        /* 20.11.3.6 Observers */
        element_type* get() const noexcept { return ptr; }

        T& operator*() const noexcept { 
            if constexpr (is_void_v<T> || is_array_v<T>) {
                static_assert(__internal::always_false<T>, "*shared_ptr<T> can only be called when T is not an array type or cv void.");
            } else {
                return *get();
            }
        }

        T* operator->() const noexcept {
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
            return ctrl ? ctrl->get_shared_count() : 0;
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
        constexpr shared_ptr(nullptr_t) noexcept : ptr(nullptr), ctrl(nullptr) {}

        template<class Y> requires __internal::is_complete<Y>::value && is_array_v<T>
            && ((is_unbounded_array_v<T> && is_convertible_v<Y(*)[], T*> && requires (Y* p) { { delete[] p } noexcept; })
                || (is_bounded_array_v<T> && is_convertible_v<Y(*)[extent_v<T>], T*> && requires (Y* p) { { delete[] p } noexcept; }))
        explicit shared_ptr(Y* p) : ptr(p) {
            try {
                ctrl = new __internal::__ctrl_ptr_with_deleter<T, void (*)(element_type*)>(p, [](element_type* p) { delete[] p; });
                __enable_shared_from_this(p);
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
                __enable_shared_from_this(p);
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
                __enable_shared_from_this(p);
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
                __enable_shared_from_this(p);
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
                __enable_shared_from_this(ptr);
            } else {
                ctrl = new __internal::__ctrl_ptr_with_deleter<T, D>(ptr, ref(r.get_deleter()));
                ptr = r.release();
                __enable_shared_from_this(ptr);
            }
        }

        /* Make a new shared_ptr by simply assigning its member variables with the provided ones. This is only used internally for implementing the
         * make_shared series of functions. */
        static shared_ptr<T> __make_shared(__internal::__ctrl_obj<T>* ctrl, bool enable_shared_from_this = false) {
            shared_ptr<T> result;
            result.ptr = ctrl->get_object_ptr();
            result.ctrl = ctrl;

            if (enable_shared_from_this) {
                result.__enable_shared_from_this(result.ptr);
            }

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

        template<class D>
        friend D* get_deleter(const shared_ptr& p) noexcept;
    };

    template<class T> shared_ptr(weak_ptr<T>) -> shared_ptr<T>;
    template<class T, class D> shared_ptr(unique_ptr<T, D>) -> shared_ptr<T>;

    /* 20.11.3.7 Creation */
    // TODO: Implement support for U[].
    template<class T, class ...Args> requires (!is_array_v<T>)
    shared_ptr<T> make_shared(Args&&... args) {
        return shared_ptr<T>::__make_shared(new __internal::__ctrl_obj<T>(forward<Args>(args)...), true);
    }

    template<class T, class A, class ...Args> requires (!is_array_v<T>)
    shared_ptr<T> allocate_shared(const A& a, Args&& ...args) {
        using allocator_type = typename __internal::__ctrl_obj_with_alloc<T, A>::allocator_type;
        allocator_type alloc = a;

        auto ctrl = allocator_traits<allocator_type>::allocate(alloc, 1);
        allocator_traits<allocator_type>::construct(alloc, ctrl, forward<Args>(args)...);
        return shared_ptr<T>::__make_shared(ctrl, true);
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
    shared_ptr<T> make_shared_for_overwrite() {
        if constexpr (is_array_v<T>) return make_shared<T>();

        auto ctrl = new __internal::__ctrl_obj<T>(typename __internal::__ctrl_obj<T>::noop_t{});
        ::new (static_cast<void*>(ctrl->get_object_ptr())) T;

        return shared_ptr<T>::__make_shared(ctrl);
    }

    template<class T, class A> requires (!is_bounded_array_v<T>)
    shared_ptr<T> allocate_shared_for_overwrite(const A& a) {
        if constexpr (is_array_v<T>) return allocate_shared<T>(a);

        auto ctrl = new __internal::__ctrl_obj<T>(typename __internal::__ctrl_obj<T>::noop_t{});
        ::new (static_cast<void*>(ctrl->get_object_ptr())) T;

        return shared_ptr<T>::__make_shared(ctrl);
    }

    /* 20.11.3.8 Comparison */
    template<class T, class U> bool operator==(const shared_ptr<T>& a, const shared_ptr<U>& b) noexcept { return a.get() == b.get(); }
    template<class T> bool operator==(const shared_ptr<T>& a, nullptr_t) noexcept { return !a; }
    template<class T, class U> strong_ordering operator<=>(const shared_ptr<T>& a, const shared_ptr<T>& b) noexcept {
        return compare_three_way()(a.get(), b.get());
    }
    template<class T> strong_ordering operator<=>(const shared_ptr<T>& a, nullptr_t) noexcept {
        return compare_three_way()(a.get(), nullptr);
    }

    /* 20.11.3.9 Specialized algorithms */
    template<class T> void swap(shared_ptr<T>& a, shared_ptr<T>& b) noexcept { a.swap(b); }

    /* 20.11.3.10 Casts */
    template<class T, class U> requires requires { static_cast<T*>((U*) nullptr); }
    shared_ptr<T> static_pointer_cast(const shared_ptr<U>& r) noexcept {
        return shared_ptr<T>(r, static_cast<typename shared_ptr<T>::element_type*>(r.get()));
    }

    template<class T, class U> requires requires { static_cast<T*>((U*) nullptr); }
    shared_ptr<T> static_pointer_cast(shared_ptr<U>&& r) noexcept {
        return shared_ptr<T>(move(r), static_cast<typename shared_ptr<T>::element_type*>(r.get()));
    }

    template<class T, class U> requires requires { dynamic_cast<T*>((U*) nullptr); }
    shared_ptr<T> dynamic_pointer_cast(const shared_ptr<U>& r) noexcept {
        if (auto ptr = static_cast<typename shared_ptr<T>::element_type*>(r.get()); ptr) {
            return shared_ptr<T>(r, ptr);
        } else {
            return shared_ptr<T>();
        }
    }

    template<class T, class U> requires requires { dynamic_cast<T*>((U*) nullptr); }
    shared_ptr<T> dynamic_pointer_cast(shared_ptr<U>&& r) noexcept {
        if (auto ptr = static_cast<typename shared_ptr<T>::element_type*>(r.get()); ptr) {
            return shared_ptr<T>(move(r), ptr);
        } else {
            return shared_ptr<T>();
        }
    }


    template<class T, class U> requires requires { const_cast<T*>((U*) nullptr); }
    shared_ptr<T> const_pointer_cast(const shared_ptr<U>& r) noexcept {
        return shared_ptr<T>(r, const_cast<typename shared_ptr<T>::element_type*>(r.get()));
    }

    template<class T, class U> requires requires { const_cast<T*>((U*) nullptr); }
    shared_ptr<T> const_pointer_cast(shared_ptr<U>&& r) noexcept {
        return shared_ptr<T>(move(r), const_cast<typename shared_ptr<T>::element_type*>(r.get()));
    }


    template<class T, class U> requires requires { reinterpret_cast<T*>((U*) nullptr); }
    shared_ptr<T> reinterpret_pointer_cast(const shared_ptr<U>& r) noexcept {
        return shared_ptr<T>(r, reinterpret_cast<typename shared_ptr<T>::element_type*>(r.get()));
    }

    template<class T, class U> requires requires { reinterpret_cast<T*>((U*) nullptr); }
    shared_ptr<T> reinterpret_pointer_cast(shared_ptr<U>&& r) noexcept {
        return shared_ptr<T>(move(r), reinterpret_cast<typename shared_ptr<T>::element_type*>(r.get()));
    }

    /* 20.11.3.11 get_deleter */
    template<class D, class T>
    D* get_deleter(const shared_ptr<T>& p) noexcept {
        return p.ctrl ? p.ctrl->get_deleter() : nullptr;
    }

    /* 20.11.3.12 I/O */
    template<class E, class T, class Y>
    basic_ostream<E, T>& operator<<(basic_ostream<E, T>& os, const shared_ptr<Y>& p) { 
        return os << p.get(); 
    }
}