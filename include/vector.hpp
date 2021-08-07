#pragma once

#include "compare.hpp"
#include "initializer_list.hpp"
#include "memory.hpp"
#include "iterator.hpp"
#include "bit.hpp"
#include "type_traits.hpp"
#include "optional.hpp"
#include "limits.hpp"
#include "memory_resource.hpp"
#include "stdexcept.hpp"
#include "algorithm.hpp"

// TODO: Implement vector<bool>.

namespace std {
    template<class T, class Allocator = allocator<T>>
    requires is_same_v<typename Allocator::value_type, T>
    class vector {
    private:
        using traits_type = allocator_traits<Allocator>;
        using rebound_traits = typename allocator_traits<Allocator>::template rebind_traits<T>;
    public:
        using value_type = T;
        using allocator_type = Allocator;
        using pointer = typename allocator_traits<Allocator>::pointer;
        using const_pointer = typename allocator_traits<Allocator>::const_pointer;
        using reference = value_type&;
        using const_reference = const value_type&;
        using size_type = typename allocator_traits<Allocator>::size_type;
        using difference_type = typename allocator_traits<Allocator>::difference_type;
        using iterator = pointer;
        using const_iterator = const_pointer;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        constexpr vector() noexcept(noexcept(Allocator()))
        requires is_default_constructible_v<Allocator> : vector(Allocator()) {}

        constexpr explicit vector(const Allocator& alloc) noexcept : alloc(alloc), len(0), cap(0), buf(rebound_traits::allocate(this->alloc, 0)) {}

        constexpr explicit vector(size_type n, const Allocator& alloc = Allocator())
        requires requires (Allocator a, T* p) { rebound_traits::construct(a, p); } 
            : alloc(alloc), len(n), cap(n ? bit_ceil(n) : 0), buf(rebound_traits::allocate(this->alloc, cap)) {
            for (pointer p = buf; p < p + len; p++) {
                rebound_traits::construct(this->alloc, p);
            }
        }

        constexpr vector(size_type n, const T& value, const Allocator& alloc = Allocator())
        requires requires (Allocator a, T* p, const T& v) { rebound_traits::construct(a, p, v); }
            : alloc(alloc), len(n), cap(n ? bit_ceil(n) : 0), buf(rebound_traits::allocate(this->alloc, cap)) {
            for (pointer p = buf; p < p + len; p++) {
                rebound_traits::construct(this->alloc, p, value);
            }
        }

        template<__internal::legacy_input_iterator InputIterator>
        constexpr vector(InputIterator first, InputIterator last, const Allocator& alloc = Allocator())
            : alloc(alloc), len(0), cap(0), buf(rebound_traits::allocate(this->alloc, 0)) {
            if (first == last) [[unlikely]] {
                return;
            }

            if constexpr (__internal::legacy_random_access_iterator<InputIterator>) {
                cap = static_cast<size_type>(last - first);
            } else if constexpr (__internal::legacy_forward_iterator<InputIterator>) {
                for (InputIterator it = first; it != last; it++) {
                    cap++;
                }
            } else {
                cap = 1;
            }

            cap = bit_ceil(cap);
            rebound_traits::deallocate(this->alloc, buf, 0);
            buf = rebound_traits::allocate(alloc, cap);
            
            while (first != last) {
                if (len == cap) {
                    cap *= 2;
                    pointer new_buf = rebound_traits::allocate(alloc, cap);
                    for (std::size_t i = 0; i < len; i++) {
                        rebound_traits::construct(alloc, new_buf + i, buf[i]);
                        rebound_traits::destroy(alloc, buf + i);
                    }
                    rebound_traits::deallocate(alloc, buf);
                    buf = new_buf;
                }

                rebound_traits::construct(alloc, buf + len, *first);
                first++;
            }
        }

        constexpr vector(const vector& x) : alloc(allocator_traits<allocator_type>::select_on_container_copy_construction(x.alloc)), len(x.len),
            cap(x.cap), buf(rebound_traits::allocate(alloc, cap)) {
            for (std::size_t i = 0; i < len; i++) {
                rebound_traits::construct(alloc, buf + i, x.buf[i]);
            }
        }

        constexpr vector(vector&& x) noexcept : alloc(move(x).alloc), len(x.len), cap(x.cap), buf(x.buf) {
            x.buf = nullptr;
            x.len = 0;
            x.cap = 0;
        }   

        constexpr vector(const vector& x, const Allocator& alloc) 
        requires requires (Allocator alloc, T* p, const T& v) {
            traits_type::construct(alloc, p, declval<T>()); 
            traits_type::construct(alloc, p, v); 
        } : alloc(alloc), len(x.len), cap(x.cap), buf(rebound_traits::allocate(this->alloc, cap)) {
            for (std::size_t i = 0; i < len; i++) {
                rebound_traits::construct(this->alloc, buf + i, x.buf[i]);
            }
        }

        constexpr vector(vector&& x, const Allocator& alloc)
        requires requires (Allocator alloc, T* p) { traits_type::construct(alloc, p, declval<T>()); }
            : alloc(alloc), len(x.len), cap(x.cap), buf(x.buf) {
            if (x.alloc != this->alloc) {
                buf = rebound_traits::allocate(this->alloc, cap);
                for (std::size_t i = 0; i < len; i++) {
                    rebound_traits::construct(this->alloc, buf + i, move(x.buf[i]));
                }
            } else {
                x.buf = nullptr;
                x.len = 0;
                x.cap = 0;
            }
        }

        constexpr vector(initializer_list<T> il, const Allocator& alloc = Allocator()) : vector(il.begin(), il.end(), alloc) {}

        constexpr ~vector() {
            for (pointer p = buf; p != p + len; p++) {
                rebound_traits::destroy(alloc, p);
            }

            rebound_traits::deallocate(alloc, buf, cap);
        }

        constexpr vector& operator=(const vector& x)
        requires is_copy_assignable_v<T> && requires (Allocator alloc, T* p, const T& v) {
            allocator_traits<Allocator>::construct(alloc, p, declval<T>()); 
            allocator_traits<Allocator>::construct(alloc, p, v); 
        } {
            ~vector();
            if constexpr (allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
                alloc = x.alloc;
            }

            len = x.len;
            cap = x.cap;
            buf = rebound_traits::allocate(alloc, cap);
            for (std::size_t i = 0; i < len; i++) {
                rebound_traits::construct(alloc, buf + i, x.buf[i]);
            }

            return *this;
        }

        constexpr vector& operator=(vector&& x)
        noexcept(allocator_traits<Allocator>::propagate_on_container_move_assignment::value || allocator_traits<Allocator>::is_always_equal::value)
        requires allocator_traits<Allocator>::propagate_on_container_move_assignment::value || (is_move_assignable_v<T> && requires (Allocator alloc, T* p) {
            allocator_traits<Allocator>::construct(alloc, p, declval<T>());
        }) {
            ~vector();
            len = 0;
            cap = 0;
            std::swap(len, x.len);
            std::swap(cap, x.cap);

            if constexpr (allocator_traits<Allocator>::is_always_equal::value) {
                buf = x.buf;
                x.buf = rebound_traits::allocate(alloc, 0);
                std::swap(buf, x.buf);
                return;
            } else if constexpr (allocator_traits<Allocator>::propagate_on_container_move_assignment::value) {
                alloc = move(x).alloc;
            }

            allocator_traits<Allocator>::template rebind_traits<T>::allocate(alloc, cap);
            for (std::size_t i = 0; i < len; i++) {
                rebound_traits::construct(alloc, buf + i, move(x.buf[i]));
            }
            x.buf = rebound_traits::allocate(alloc, 0);

            return *this;
        }

        constexpr vector& operator=(initializer_list<T> il) {
            return *this = vector(il, alloc);
        }

        template<__internal::legacy_input_iterator InputIterator>
        requires requires (Allocator alloc, T* p, InputIterator it, T& lv) { 
            traits_type::construct(alloc, p, *it);
            lv = *it;
        } && (__internal::legacy_forward_iterator<InputIterator> || requires (Allocator alloc, T* p) {
            traits_type::construct(alloc, p, declval<T>());
        })
        constexpr void assign(InputIterator first, InputIterator last) {
            if constexpr (__internal::legacy_forward_iterator<InputIterator>) {
                const typename iterator_traits<InputIterator>::difference_type new_len = [&]() {
                    if constexpr (__internal::legacy_random_access_iterator<InputIterator>) {
                        return last - first;
                    } else {
                        typename iterator_traits<InputIterator>::difference_type new_cap = 0;
                        while (first != last) {
                            new_cap++;
                            first++;
                        }
                        return new_cap;
                    }
                }();

                reserve(new_len);

                // For the first stretch of array (new and old segment overlaps)
                for (std::size_t i = 0; i < min(new_len, len); i++) {
                    buf[i] = *first;
                    first++;
                }

                if (new_len < len) {
                    // If there's more old elements than new ones.
                    for (std::size_t i = new_len; i < len; i++) {
                        rebound_traits::destroy(alloc, buf + i);
                    }
                } else if (new_len > len) {
                    // If there's more new elements than old ones.
                    for (std::size_t i = len; i < new_len; i++, first++) {
                        rebound_traits::construct(alloc, buf + i, *first);
                    }
                }

                len = new_len;
            } else {
                clear();
                while (first != last) {
                    emplace_back(*first);
                    first++;
                }
            }
        }

        constexpr void assign(size_type n, const T& u)
        requires is_copy_assignable_v<T> && requires (Allocator alloc, T* p, const T& v) { traits_type::construct(alloc, p, v); } {
            resize(n, u);
        }

        constexpr void assign(initializer_list<T> il) {
            assign(il.begin(), il.end());
        }

        constexpr allocator_type get_allocator() const noexcept {
            return alloc;
        }

        constexpr iterator begin() noexcept {
            return buf;
        }

        constexpr const_iterator begin() const noexcept {
            return buf;
        }

        constexpr iterator end() noexcept {
            return buf + len;
        }

        constexpr const_iterator end() const noexcept {
            return buf + len;
        }

        constexpr reverse_iterator rbegin() noexcept {
            return reverse_iterator(end());
        }

        constexpr const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator(end());
        }

        constexpr reverse_iterator rend() noexcept {
            return reverse_iterator(begin());
        }

        constexpr const_reverse_iterator rend() const noexcept {
            return const_reverse_iterator(begin());
        }

        constexpr const_iterator cbegin() const noexcept {
            return begin();
        }

        constexpr const_iterator cend() const noexcept {
            return end();
        }

        constexpr const_reverse_iterator crbegin() const noexcept {
            return rbegin();
        }

        constexpr const_reverse_iterator crend() const noexcept {
            return rend();
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return len == 0;
        }

        constexpr size_type size() const noexcept {
            return len;
        }

        constexpr size_type max_size() const noexcept {
            return numeric_limits<std::size_t>::max();
        }

        constexpr size_type capacity() const noexcept {
            return cap;
        }

        constexpr void resize(size_type sz)
        requires requires (Allocator alloc, T* p) { 
            traits_type::construct(alloc, p, declval<T>()); 
            traits_type::construct(alloc, p);
        } {
            if (sz < len) {
                for (std::size_t i = sz; i < len; i++) {
                    rebound_traits::destroy(alloc, buf + i);
                }

                len = sz;
            } else if (sz == len) {
                return;
            } else {
                const std::size_t new_cap = bit_ceil(sz);
                T* const new_buf = rebound_traits::allocate(alloc, new_cap);
                for (std::size_t i = 0; i < len; i++) {
                    rebound_traits::construct(alloc, new_buf + i, move(buf[i]));
                }

                for (std::size_t i = len; i < sz; i++) {
                    rebound_traits::construct(alloc, new_buf + i);
                }

                for (std::size_t i = 0; i < len; i++) {
                    rebound_traits::destroy(alloc, buf[i]);
                }

                rebound_traits::deallocate(alloc, buf, cap);
                buf = new_buf;
                cap = new_cap;
                len = sz;
            }
        }

        constexpr void resize(size_type sz, const T& c)
        requires requires (Allocator alloc, T* p) { traits_type::construct(alloc, p, c); } {
            if (sz < len) {
                for (std::size_t i = sz; i < len; i++) {
                    rebound_traits::destroy(alloc, buf + i);
                }

                len = sz;
            } else if (sz == len) {
                return;
            } else {
                const std::size_t new_cap = bit_ceil(sz);
                T* const new_buf = rebound_traits::allocate(alloc, new_cap);
                for (std::size_t i = 0; i < len; i++) {
                    rebound_traits::construct(alloc, new_buf + i, move(buf[i]));
                }

                for (std::size_t i = len; i < sz; i++) {
                    rebound_traits::construct(alloc, new_buf + i, c);
                }

                for (std::size_t i = 0; i < len; i++) {
                    rebound_traits::destroy(alloc, buf[i]);
                }

                rebound_traits::deallocate(alloc, buf, cap);
                buf = new_buf;
                cap = new_cap;
                len = sz;
            }
        }

        constexpr void reserve(size_type n)
        requires requires (Allocator alloc, T* p) { traits_type::construct(alloc, p, declval<T>()); } {
            if (cap >= n) {
                return;
            } else if (n > max_size()) [[unlikely]] {
                throw length_error("Invalid argument to vector::reserve.");
            }

            const size_type new_cap = bit_ceil(n);
            T* const new_buf = rebound_traits::allocate(alloc, new_cap);
            for (std::size_t i = 0; i < len; i++) {
                rebound_traits::construct(alloc, new_buf + i, move(buf[i]));
                rebound_traits::destroy(alloc, buf[i]);
            }

            rebound_traits::deallocate(alloc, buf, cap);
            buf = new_buf;
            cap = new_cap;
        }

        constexpr void shrink_to_fit()
        requires requires (Allocator alloc, T* p) { traits_type::construct(alloc, p, declval<T>()); } {
            if (cap == len) {
                return;
            }

            T* const new_buf = rebound_traits::allocate(alloc, len);
            for (std::size_t i = 0; i < len; i++) {
                rebound_traits::construct(alloc, new_buf + i, move(buf[i]));
                rebound_traits::destroy(alloc, buf[i]);
            }

            rebound_traits::deallocate(alloc, buf, cap);
            buf = new_buf;
            cap = len;
        }

        constexpr reference operator[](size_type n) {
            return buf[n];
        }

        constexpr const_reference operator[](size_type n) const {
            return buf[n];
        }

        constexpr const_reference at(size_type n) const {
            if (n < 0 || n >= len) [[unlikely]] {
                throw out_of_range("Invalid argument to vector::at.");
            } else {
                return buf[n];
            }
        }

        constexpr reference at(size_type n) {
            if (n < 0 || n >= len) [[unlikely]] {
                throw out_of_range("Invalid argument to vector::at.");
            } else {
                return buf[n];
            }
        }

        constexpr reference front() {
            return *buf;
        }

        constexpr const_reference front() const {
            return *buf;
        }

        constexpr reference back() {
            return buf[len - 1];
        }

        constexpr const_reference back() const {
            return buf[len - 1];
        }

        constexpr T* data() noexcept {
            return buf;
        }

        constexpr const T* data() const noexcept {
            return buf;
        }

        template<class ...Args>
        requires requires (Allocator& alloc, T* p, Args&& ...args) { 
            traits_type::construct(alloc, p, forward<Args>(args)...);
            traits_type::construct(alloc, p, declval<T>()); 
        }
        constexpr reference emplace_back(Args&& ...args) {
            reserve(len + 1);
            rebound_traits::construct(alloc, buf + len, forward<Args>(args)...);
            len++;
            return buf[len - 1];
        }

        constexpr void push_back(const T& x) {
            resize(len + 1, x);
        }

        constexpr void push_back(T&& x) {
            reserve(len + 1);
            rebound_traits::construct(alloc, buf + len, move(x));
            len++;
        }

        constexpr void pop_back() {
            len--;
            rebound_traits::destroy(alloc, buf + len);
        }

        template<class ...Args>
        constexpr iterator emplace(const_iterator position, Args&& ...args) 
        requires is_move_assignable_v<T> && requires (Allocator alloc, T* p, Args&& ...args) { 
            traits_type::construct(alloc, p, declval<T>()); 
            traits_type::construct(alloc, p, forward<Args>(args)...);
        } {
            reserve(len + 1);
            if (len == 0) {
                traits_type::construct(alloc, buf, forward<Args>(args)...);
                len++;
                return end();
            } else {
                traits_type::construct(alloc, buf + len, move(buf[len - 1]));
                iterator it = buf + len - 1;
                while (it != position) {
                    *it = move(*(--it));
                }

                *it = T(forward<Args>(args)...);
                len++;
                return next(it);
            }
        }

        constexpr iterator insert(const_iterator position, const T& x)
        requires is_copy_assignable_v<T> && requires (Allocator alloc, T* p, const T& v) { traits_type::construct(alloc, p, v); } {
            return insert(position, 1, x);
        }

        constexpr iterator insert(const_iterator position, T&& x) 
        requires is_move_assignable_v<T> && requires (Allocator alloc, T* p) { traits_type::construct(alloc, p, declval<T>()); } {
            reserve(len + 1);
            if (len == 0) {
                traits_type::construct(alloc, buf, move(x));
                len++;
                return end();
            } else {
                traits_type::construct(alloc, buf + len, move(buf[len - 1]));
                iterator it = buf + len - 1;
                while (it != position) {
                    *it = move(*(--it));
                }

                *it = move(x);
                len++;
                return next(it);
            }
        }

        constexpr iterator insert(const_iterator position, size_type n, const T& x)
        requires is_copy_assignable_v<T> && requires (Allocator alloc, T* p, const T& v) { traits_type::construct(alloc, p, v); } {
            reserve(len + n);
            if (len == 0) {
                for (std::size_t i = 0; i < n; i++) {
                    traits_type::construct(alloc, buf + i, x);
                    len++;
                }

                return end();
            } else {
                traits_type::construct(alloc, buf + len + n - 1, buf[buf + len - 1]);
                iterator it = buf + len + n - 1;
                while (it != position + n - 1) {
                    *it = *(it - n);
                    it--;
                }

                while (it != position) {
                    *it = x;
                    it--;
                }

                len += n;
                return it + n;
            }
        }

        template<__internal::legacy_input_iterator InputIterator>
        requires requires (Allocator alloc, T* p, InputIterator i) { 
            traits_type::construct(alloc, p, *i);
            traits_type::construct(alloc, p, declval<T>());
        } && is_move_assignable_v<T> && is_move_constructible_v<T> && is_swappable_v<T>
        constexpr iterator insert(const_iterator position, InputIterator first, InputIterator last) {
            const iterator old_end = end();
            while (first != last) {
                emplace_back(*first);
                first++;
            }
            
            rotate(position, old_end, end());
            return position;
        }

        constexpr iterator insert(const_iterator position, initializer_list<T> il) {
            return insert(position, il.begin(), il.end());
        }

        constexpr iterator erase(const_iterator position)
        requires is_move_assignable_v<T> {
            return erase(position, next(position));
        }

        constexpr iterator erase(const_iterator first, const_iterator last)
        requires is_move_assignable_v<T> {
            if (first == last) {
                return last;
            }

            iterator overwrite_it = first;
            iterator write_it = last;

            while (write_it != end() && overwrite_it != last) {
                *overwrite_it = move(*write_it);
                write_it++;
                overwrite_it++;
                len--;
            }

            while (write_it != end()) {
                *overwrite_it = move(*write_it);
                write_it++;
                overwrite_it++;
            }

            return first;
        }

        constexpr void swap(vector& other)
        noexcept(traits_type::propagate_on_container_swap::value || traits_type::is_always_equal::value) {
            std::swap(len, other.len);
            std::swap(cap, other.cap);
            std::swap(buf, other.buf);

            if constexpr (traits_type::propagate_on_container_swap::value) {
                using std::swap;
                swap(alloc, other.alloc);
            }
        }

        constexpr void clear() noexcept {
            for (std::size_t i = 0; i < len; i++) {
                rebound_traits::destroy(alloc, buf + i);
            }
            len = 0;
        }

    private:
        [[no_unique_address]] Allocator alloc;
        size_type len;
        size_type cap;
        pointer buf;
    };

    template<__internal::legacy_input_iterator InputIterator, class Allocator = allocator<typename iterator_traits<InputIterator>::value_type>>
    vector(InputIterator, InputIterator, Allocator = Allocator()) -> vector<typename iterator_traits<InputIterator>::value_type, Allocator>;

    template<class T, class Allocator>
    requires requires (const T& t1, const T& t2) { { t1 == t2 } -> convertible_to<bool>; }
    constexpr bool operator==(const vector<T, Allocator>& x, const vector<T, Allocator>& y) {
        if (x.size() != y.size()) {
            return false;
        }

        for (std::size_t i = 0; i < x.size(); i++) {
            if (x[i] != y[i]) {
                return false;
            }
        }

        return true;
    }

    template<class T, class Allocator>
    requires requires (const T& t1, const T& t2) { __internal::synth_three_way(t1, t2); }
    constexpr __internal::synth_three_way_result<T> operator<=>(const vector<T, Allocator>& x, const vector<T, Allocator>& y) {
        return lexicographical_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend(), __internal::synth_three_way);
    }

    template<class T, class Allocator>
    constexpr void swap(vector<T, Allocator>& x, vector<T, Allocator>& y) noexcept(noexcept(x.swap(y))) {
        x.swap(y);
    }

    template<class T, class Allocator, class U>
    constexpr typename vector<T, Allocator>::size_type erase(vector<T, Allocator>& c, const U& value) {
        const typename vector<T, Allocator>::iterator it = remove(c.begin(), c.end(), value);
        const typename vector<T, Allocator>::size_type r = distance(it, c.end());
        c.erase(it, c.end());
        return r;
    }

    template<class T, class Allocator, class Predicate>
    requires requires (const T& elem) { { pred(elem) } -> convertible_to<bool>; }
    constexpr typename vector<T, Allocator>::size_type erase_if(vector<T, Allocator>& c, Predicate pred) {
        const typename vector<T, Allocator>::iterator it = remove_if(c.begin(), c.end(), pred);
        const typename vector<T, Allocator>::size_type r = distance(it, c.end());
        c.erase(it, c.end());
        return r;
    }

    namespace pmr {
        template<class T>
        using vector = std::vector<T, polymorphic_allocator<T>>;
    }
}