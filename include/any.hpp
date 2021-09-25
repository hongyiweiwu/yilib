#pragma once

#include "cstddef.hpp"
#include "cstdlib.hpp"
#include "initializer_list.hpp"
#include "memory/construct_destroy.hpp"
#include "new.hpp"
#include "type_traits.hpp"
#include "typeinfo.hpp"
#include "util/utility_traits.hpp"
#include "utility.hpp"

namespace std {
    /* 20.8.3 Class bad_any_cast */
    class bad_any_cast : public bad_cast {
    public:
        const char* what() const noexcept override;
    };

    /* 20.8.4 Class any */
    class any {
    private:
        /* Returns whether a given type is a specialization of in_place_type_t. */
        template<class T>
        struct is_specialization_of_in_place_type : public false_type {};

        template<class T>
        struct is_specialization_of_in_place_type<in_place_type_t<T>> : public true_type {};

        /* The largest size for an object to be allocated on the stack. */
        static constexpr std::size_t stack_object_size_limit = 2 * sizeof(void*);

        /* Internal storage for objects of size smaller than the stack object limit. */
        struct stack_obj_storage {
        private:
            void (*copy_constructor)(stack_obj_storage&, const stack_obj_storage&) = nullptr;
            void (*move_constructor)(stack_obj_storage&, stack_obj_storage&&) = nullptr;

            template<class T>
            static void copy_construct(stack_obj_storage& self, const stack_obj_storage& other) {
                construct_at(reinterpret_cast<decay_t<T>*>(&self.data), *reinterpret_cast<const decay_t<T>*>(&other.data));
            }

            template<class T>
            static void move_construct(stack_obj_storage& self, stack_obj_storage&& other) {
                construct_at(reinterpret_cast<decay_t<T>*>(&self.data), move(*reinterpret_cast<decay_t<T>*>(&other.data)));
            }

        public:
            aligned_storage_t<stack_object_size_limit> data;

            template<class T, class... Args>
            stack_obj_storage(__internal::type_list<T>, Args&&... args) : copy_constructor(&copy_construct<T>), move_constructor(&move_construct<T>) {
                construct_at(reinterpret_cast<decay_t<T>*>(&data), forward<Args>(args)...);
            }

            stack_obj_storage(const stack_obj_storage& other) : copy_constructor(other.copy_constructor), move_constructor(other.move_constructor) {
                copy_constructor(*this, other);
            }

            stack_obj_storage(stack_obj_storage&& other) : copy_constructor(other.copy_constructor), move_constructor(other.move_constructor) {
                move_constructor(*this, move(other));
            }

            // Explicitly define the destructor to make the struct non-trivial, and hence preventing the
            // the __Storage union from automatically destructing this.
            ~stack_obj_storage() {}

            template<class T>
            static void destroy(any& self) {
                destroy_at(reinterpret_cast<decay_t<T>*>(&self.storage.stack_obj.data));
            }
        };

        /* Internal storage for objects of size larger than the stack object limit. */
        struct heap_obj_storage {
        private:
            void (*copy_constructor)(heap_obj_storage&, const heap_obj_storage&) = nullptr;
            void (*move_constructor)(heap_obj_storage&, heap_obj_storage&&) = nullptr;

            template<class T>
            static void copy_construct(heap_obj_storage& self, const heap_obj_storage& other) {
                self.data = operator new(sizeof(decay_t<T>), align_val_t(alignof(decay_t<T>)));
                construct_at(reinterpret_cast<decay_t<T>*>(self.data), *reinterpret_cast<const decay_t<T>*>(other.data));
            }

            template<class T>
            static void move_construct(heap_obj_storage& self, heap_obj_storage&& other) {
                self.data = operator new(sizeof(decay_t<T>), align_val_t(alignof(decay_t<T>)));
                construct_at(reinterpret_cast<decay_t<T>*>(&self.data), move(*reinterpret_cast<decay_t<T>*>(&other.data)));
            }

        public:
            void* data = nullptr;

            template<class T, class... Args>
            heap_obj_storage(__internal::type_list<T>, Args&&... args)
                : copy_constructor(&copy_construct<T>), move_constructor(&move_construct<T>), data(operator new(sizeof(decay_t<T>), align_val_t(alignof(decay_t<T>)))) {
                construct_at(reinterpret_cast<decay_t<T>*>(data), forward<Args>(args)...);
            }

            heap_obj_storage(const heap_obj_storage& other) : copy_constructor(other.copy_constructor), move_constructor(other.move_constructor) {
                copy_constructor(*this, other);
            }

            heap_obj_storage(heap_obj_storage&& other) : copy_constructor(other.copy_constructor), move_constructor(other.move_constructor) {
                move_constructor(*this, move(other));
            }

            // Explicitly define the destructor to make the struct non-trivial, and hence preventing the
            // the _Storage union from automatically destructing this.
            ~heap_obj_storage() {}

            template<class T>
            static void destroy(any& self) {
                destroy_at(static_cast<decay_t<T>*>(self.storage.heap_obj.data));
                delete static_cast<decay_t<T>*>(self.storage.heap_obj.data);
            }
        };

        /* The state of the storage: if it is holding an object, and if so, where is it stored. */
        enum class State { HEAP, STACK, NONE } state = State::NONE;

        /* The storage for the object in this. */
        union Storage {
            stack_obj_storage stack_obj;
            heap_obj_storage heap_obj;
            nullptr_t no_obj;

            ~Storage() {}
        } storage = {.no_obj = nullptr};

        /* A pointer to the type_info representing the type of the stored object. nullptr if this is empty. */
        const type_info* typeinfo = nullptr;

        /* A pointer to a function that destroys the stored object. If the object is allocated on the heap, the memory space is freed. */
        void (*destroyer)(any&) = nullptr;

    public:
        constexpr any() noexcept = default;

        any(const any& other);
        any(any&& other) noexcept;

        template<class T>
        requires (!is_specialization_of_in_place_type<decay_t<T>>::value) && (!is_same_v<any, decay_t<T>>) && is_copy_constructible_v<decay_t<T>>
        any(T&& value) : any(in_place_type<T>, forward<T>(value)) {}

        template<class T, class... Args>
        requires is_copy_constructible_v<decay_t<T>> && is_constructible_v<decay_t<T>, Args...>
        explicit any(in_place_type_t<T>, Args&&... args) : typeinfo(&typeid(decay_t<T>)) {
            if constexpr (sizeof(decay_t<T>) <= stack_object_size_limit && is_nothrow_move_constructible_v<decay_t<T>>) {
                // Use stack for storage.
                new (&storage.stack_obj) stack_obj_storage(__internal::type_list<T>{}, forward<Args>(args)...);
                state = State::STACK;
                destroyer = &stack_obj_storage::destroy<T>;
            } else {
                // Use heap for storage.
                new (&storage.heap_obj) heap_obj_storage(__internal::type_list<T>{}, forward<Args>(args)...);
                state = State::HEAP;
                destroyer = &heap_obj_storage::destroy<T>;
            }
        }

        template<class T, class U, class... Args>
        requires is_copy_constructible_v<decay_t<T>> && is_constructible_v < decay_t<T>, initializer_list<U>
        &, Args... > explicit any(in_place_type_t<T>, initializer_list<U> il, Args&&... args) : any(in_place_type<T>, il, forward<Args>(args)...) {}

        ~any();

        any& operator=(const any& rhs);
        any& operator=(any&& rhs) noexcept;

        template<class T>
        requires(!is_same_v<any, decay_t<T>>) && is_copy_constructible_v<decay_t<T>> any& operator=(T&& rhs) {
            any(forward<T>(rhs)).swap(*this);
            return *this;
        }

        template<class T, class... Args>
        requires is_copy_constructible_v<decay_t<T>> && is_constructible_v<decay_t<T>, Args...> decay_t<T>
        &emplace(Args&&... args) {
            reset();
            if constexpr (sizeof(decay_t<T>) <= stack_object_size_limit && is_nothrow_move_constructible_v<decay_t<T>>) {
                // Use stack for storage.
                new (&storage.stack_obj) stack_obj_storage(__internal::type_list<T>{}, forward<Args>(args)...);
                state = State::STACK;
                destroyer = &stack_obj_storage::destroy<T>;
            } else {
                // Use heap for storage.
                new (&storage.heap_obj) heap_obj_storage(__internal::type_list<T>{}, forward<Args>(args)...);
                state = State::HEAP;
                destroyer = &heap_obj_storage::destroy<T>;
            }

            typeinfo = &typeid(decay_t<T>);

            return *any_cast<decay_t<T>>(this);
        }

        template<class T, class U, class... Args>
        requires is_copy_constructible_v<decay_t<T>> && is_constructible_v<decay_t<T>, initializer_list<U>&, Args...>
        decay_t<T>& emplace(initializer_list<U> il, Args&&... args) {
            return emplace(il, forward<Args>(args)...);
        }

        void reset() noexcept;
        void swap(any& rhs) noexcept;

        bool has_value() const noexcept;
        const type_info& type() const noexcept;

        template<class T>
        friend const T* any_cast(const any* operand) noexcept;

        template<class T>
        friend T* any_cast(any* operand) noexcept;
    };

    /* 20.8.5 Non-member functions */
    void swap(any& x, any& y) noexcept;

    template<class T, class... Args>
    any make_any(Args&&... args) {
        return any(in_place_type<T>, forward<Args>(args)...);
    }

    template<class T, class U, class... Args>
    any make_any(initializer_list<U> il, Args&&... args) {
        return any(in_place_type<T>, il, forward<Args>(args)...);
    }

    template<class T>
    requires is_constructible_v<T, const remove_cvref_t<T>&>
    T any_cast(const any& operand) {
        if (typeid(remove_reference_t<T>) != operand.type()) {
            throw bad_any_cast();
        }
        return static_cast<T>(*any_cast<remove_cvref_t<T>>(&operand));
    }

    template<class T>
    requires is_constructible_v<T, remove_cvref_t<T>&>
    T any_cast(any& operand) {
        if (typeid(remove_reference_t<T>) != operand.type()) {
            throw bad_any_cast();
        }
        return static_cast<T>(*any_cast<remove_cvref_t<T>>(&operand));
    }

    template<class T>
    requires is_constructible_v<T, remove_cvref_t<T>>
    T any_cast(any&& operand) {
        if (typeid(remove_reference_t<T>) != operand.type()) {
            throw bad_any_cast();
        }
        return static_cast<T>(move(*any_cast<remove_cvref_t<T>>(&operand)));
    }

    template<class T>
    const T* any_cast(const any* operand) noexcept {
        if (operand && operand->type() == typeid(T)) {
            switch (operand->state) {
                case any::State::HEAP:
                    return reinterpret_cast<T*>(operand->storage.heap_obj.data);
                case any::State::STACK:
                    return reinterpret_cast<T*>(&operand->storage.stack_obj.data);
                default: // This will never happen.
                    break;
            }
        }

        return nullptr;
    }

    template<class T>
    T* any_cast(any* operand) noexcept {
        if (operand && operand->type() == typeid(T)) {
            switch (operand->state) {
                case any::State::HEAP:
                    return reinterpret_cast<T*>(operand->storage.heap_obj.data);
                case any::State::STACK:
                    return reinterpret_cast<T*>(&operand->storage.stack_obj.data);
                default: // This will never happen.
                    break;
            }
        }

        return nullptr;
    }
} // namespace std