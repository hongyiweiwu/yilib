#pragma once

#include "compare.hpp"
#include "initializer_list.hpp"
#include "memory.hpp"
#include "type_traits.hpp"
#include "iterator.hpp"

namespace std {
    template<class T, class Allocator = allocator<T>>
    class forward_list {
    private:
        /* A deleter for an unique_ptr that uses the supplied allocator to deallocate objects, but still uses the object's destructor to destroy the object. */
        struct allocator_aware_deleter {
        private:
            Allocator* alloc;

        public:
            constexpr void operator()(T* ptr) const noexcept {
                ptr->~T();
                allocator_traits<Allocator>::template rebind_traits<T>::deallocate(*alloc, ptr, 1);
            }
        };

    public:
        struct node {
            unique_ptr<T, allocator_aware_deleter> data;
            node* next;
        };

        struct immutable_iterator {
        private:
            /* Pointer to the node that this iterator points to. Equals to nullptr if this is the past-the-end iterator. */
            node* ptr;
        public:
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using reference = const T&;
            using pointer = const T*;
            using iterator_category = forward_iterator_tag;

            constexpr immutable_iterator(node* ptr = nullptr) : ptr(ptr) {}

            /* Returns the data stored in the node pointed to by this iterator. Triggers undefined behavior if this is the past-the-end iterator. */
            constexpr T& operator*() const noexcept {
                return *ptr->data;
            }

            /* Moves this iterator so it points to the next node. No-op if this is the past-the-end pointer. */
            constexpr immutable_iterator& operator++() noexcept {
                ptr = ptr ? ptr->next : ptr;
                return *this;
            }

            /* Moves this pointer so it points to the next node. No-op if this is the past-the-end pointer. */
            constexpr immutable_iterator operator++(int) noexcept {
                const immutable_iterator temp = *this;
                ++*this;
                return temp;
            }

            constexpr bool operator==(const immutable_iterator& other) const noexcept {
                return ptr == other.ptr;
            }

            constexpr bool operator!=(const immutable_iterator& other) const noexcept {
                return ptr != other.ptr;
            }

            inline static constinit immutable_iterator end;
        };

        struct mutable_iterator {
        private:
            /* Pointer to the node that this iterator points to. Equals to nullptr if this is the past-the-end iterator. */
            node* ptr;
        public:
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using reference = T&;
            using pointer = T*;
            using iterator_category = forward_iterator_tag;

            constexpr mutable_iterator(node* ptr = nullptr) noexcept : ptr(ptr) {}

            /* Returns the data stored in the node pointed to by this iterator. Triggers undefined behavior if this is the past-the-end iterator. */
            constexpr T& operator*() const noexcept {
                return *ptr->data;
            }

            /* Moves this iterator so it points to the next node. No-op if this is the past-the-end pointer. */
            constexpr mutable_iterator& operator++() noexcept {
                ptr = ptr ? ptr->next : ptr;
                return *this;
            }

            /* Moves this pointer so it points to the next node. No-op if this is the past-the-end pointer. */
            constexpr mutable_iterator operator++(int) noexcept {
                const mutable_iterator temp = *this;
                ++*this;
                return temp;
            }

            constexpr bool operator==(const mutable_iterator& other) const noexcept {
                return ptr == other.ptr;
            }

            constexpr bool operator!=(const mutable_iterator& other) const noexcept {
                return ptr != other.ptr;
            }

            constexpr operator immutable_iterator() const noexcept {
                return immutable_iterator(ptr);
            }

            inline static constinit mutable_iterator end;
        };

        /* Makes the front sentinel node of the forward list. */
        static constexpr node make_sentinel() noexcept {
            return node{ .data = unique_ptr<T, allocator_aware_deleter>(), .next = nullptr };
        }


        /* Construct a node at the given address with the given arguments for its data. */
        template<class ...Args>
        constexpr void construct_node_at(node* ptr, Args&& ...args) noexcept {
            T* const elem_adr = allocator_traits<Allocator>::template rebind_traits<T>::allocate(alloc, 1);
            allocator_traits<Allocator>::template rebind_traits<T>::construct(elem_adr, forward<Args>(args)...);
            construct_at(ptr, { .data = unique_ptr<T, allocator_aware_deleter>(elem_adr, allocator_aware_deleter{ addressof(alloc) }), .next = nullptr });
        }
    public:
        using value_type = T;
        using allocator_type = Allocator;
        using pointer = typename allocator_traits<Allocator>::pointer;
        using const_pointer = typename allocator_traits<Allocator>::const_pointer;
        using reference = value_type&;
        using const_reference = const value_type&;
        using size_type = typename allocator_traits<Allocator>::size_type;
        using difference_type = typename allocator_traits<Allocator>::difference_type;
        using iterator = mutable_iterator;
        using const_iterator = immutable_iterator;

        forward_list() : forward_list(Allocator()) {}

        explicit forward_list(const Allocator& alloc) : head(make_sentinel()), alloc(alloc), node_alloc(this->alloc) {}

        explicit forward_list(size_type n, const Allocator& alloc = Allocator()) 
        requires is_default_constructible_v<T> : head(make_sentinel()), alloc(alloc), node_alloc(this->alloc) {
            node* curr = &head;
            for (std::size_t i = 0; i < n; i++) {
                curr->next = allocator_traits<Allocator>::template rebind_traits<node>::allocate(node_alloc, 1);
                construct_node_at(curr->next);
                curr = curr->next;
            }
        }

        forward_list(size_type n, const T& value, const Allocator& alloc = Allocator())
        requires requires (Allocator alloc, T* ptr) { allocator_traits<Allocator>::construct(alloc, ptr, value); } 
            : head(make_sentinel()), alloc(alloc), node_alloc(this->alloc) {
            node* curr = &head;
            for (std::size_t i = 0; i < n; i++) {
                curr->next = allocator_traits<Allocator>::template rebind_traits<node>::allocate(node_alloc, 1);
                construct_node_at(curr->next, value);
                curr = curr->next;
            }
        }

        template<__internal::legacy_input_iterator InputIterator>
        forward_list(InputIterator first, InputIterator last, const Allocator& alloc = Allocator()) 
        requires requires (Allocator alloc, T* ptr) { allocator_traits<Allocator>::construct(alloc, ptr, *first); } 
            : head(make_sentinel()), alloc(alloc), node_alloc(this->alloc) {
            node* curr = &head;
            for (; first != last; first++) {
                curr->next = allocator_traits<Allocator>::template rebind_traits<node>::allocate(node_alloc, 1);
                construct_node_at(curr->next, *first);
                curr = curr->next;
            }
        }

        forward_list(const forward_list& x)
        requires requires (Allocator alloc, T* ptr, const T& v) { allocator_traits<Allocator>::construct(alloc, ptr, v); } 
            : forward_list(x, allocator_traits<Allocator>::select_on_container_copy_construction(x.alloc)) {}

        forward_list(forward_list&& x);

        forward_list(const forward_list& x, const Allocator& alloc)
        requires requires (Allocator alloc, T* ptr, const T& v) { allocator_traits<Allocator>::construct(alloc, ptr, v); } 
            : head(make_sentinel()), alloc(alloc), node_alloc(this->alloc) {
            node* curr = &head;
            const node* other_curr = &x.head;

            while (other_curr->next != nullptr) {
                curr->next = allocator_traits<Allocator>::template rebind_traits<node>::allocate(node_alloc, 1);
                construct_node_at(curr->next, *curr->next->data);
                curr = curr->next;
                other_curr = other_curr->next;
            }
        }

        forward_list(forward_list&& x, const Allocator& alloc);

        forward_list(initializer_list<T> il, const Allocator& alloc = Allocator()) : forward_list(il.begin(), il.end(), alloc) {}

        ~forward_list() {
            const auto helper = [&](node* curr, const auto& helper) -> void {
                if (curr->next != nullptr) {
                    helper(curr->next, helper);
                    curr->next->~node();
                    allocator_traits<Allocator>::template rebind_traits<node>::deallocate(node_alloc, curr->next, 1);
                }
            };

            helper(&head, helper);
        }


    private:
        node head;
        [[no_unique_address]] Allocator alloc;
        [[no_unique_address]] typename allocator_traits<Allocator>::template rebind_alloc<node> node_alloc;
    };
}

int main() {
    std::forward_list<int> a;
    std::forward_list<int> b(a);
}