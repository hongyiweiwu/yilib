#pragma once

#include "cstddef.hpp"
#include "memory.hpp"
#include "iosfwd.hpp"
#include "cwchar.hpp"
#include "compare.hpp"
#include "cstdio.hpp"
#include "new.hpp"
#include "cwctype.hpp"
#include "cstdint.hpp"
#include "iterator.hpp"
#include "utility.hpp"
#include "algorithm.hpp"
#include "string_view.hpp"
#include "limits.hpp"
#include "stdexcept.hpp"
#include "memory_resource.hpp"
#include "functional.hpp"

// TODO: Add iostream related functions.
namespace std {
    /* 21.2 Character traits */
    template<class charT> struct char_traits {};

    template<> struct char_traits<char> {
        using char_type = char;
        using int_type = int;
        using off_type = streamoff;
        using pos_type = streampos;
        using state_type = std::mbstate_t;
        using comparison_category = strong_ordering;

        static constexpr void assign(char_type& c1, const char_type& c2) noexcept {
            c1 = c2;
        }

        static constexpr bool eq(char_type c1, char_type c2) noexcept {
            return c1 == c2;
        }

        static constexpr bool lt(char_type c1, char_type c2) noexcept {
            return static_cast<unsigned char>(c1) < static_cast<unsigned char>(c2);
        }

        static constexpr int compare(const char_type* s1, const char_type* s2, std::size_t n) {
            for (std::size_t i = 0; i < n; i++) {
                if (lt(s1[i], s2[i])) {
                    return -1;
                } else if (lt(s2[i], s1[i])) {
                    return 1;
                }
            }
            return 0;
        }

        static constexpr std::size_t length(const char_type* s) {
            for (std::size_t i = 0; ; i++) {
                if (eq(s[i], char_type())) {
                    return i;
                }
            }
        }

        static constexpr const char_type* find(const char_type* s, std::size_t n, const char_type& a) {
            for (std::size_t i = 0; i < n; i++) {
                if (eq(s[i], a)) {
                    return s + i;
                }
            }
            return 0;
        }

        static constexpr char_type* move(char_type* s1, const char_type* s2, std::size_t n) {
            char_type* tmp = new char_type[n];
            for (std::size_t i = 0; i < n; i++) {
                tmp[i] = s2[i];
            } 
            for (std::size_t i = 0; i < n; i++) {
                s1[i] = tmp[i];
            }
            delete[] tmp;
            return s1;
        }

        static constexpr char_type* copy(char_type* s1, const char_type* s2, std::size_t n) {
            for (std::size_t i = 0; i < n; i++) {
                assign(s1[i], s2[i]);
            }
            return s1;
        }

        static constexpr char_type* assign(char_type* s, std::size_t n, char_type a) {
            for (std::size_t i = 0; i < n; i++) {
                assign(s[i], a);
            }
            return s;
        }

        static constexpr int not_eof(int_type c) noexcept {
            return eq_int_type(c, eof()) ? 0 : c;
        }

        static constexpr char_type to_char_type(int_type c) noexcept {
            return static_cast<char_type>(c);
        }

        static constexpr int_type to_int_type(char_type c) noexcept {
            return static_cast<int_type>(static_cast<unsigned char>(c));
        }

        static constexpr bool eq_int_type(int_type c1, int_type c2) noexcept {
            return c1 == c2;
        }

        static constexpr int_type eof() noexcept {
            return static_cast<int_type>(EOF);
        }
    };

    template<> struct char_traits<char8_t> {
        using char_type = char8_t;
        using int_type = unsigned int;
        using off_type = streamoff;
        using pos_type = u8streampos;
        using state_type = std::mbstate_t;
        using comparison_category = strong_ordering;

        static constexpr void assign(char_type& c1, const char_type& c2) noexcept {
            c1 = c2;
        }

        static constexpr bool eq(char_type c1, char_type c2) noexcept {
            return c1 == c2;
        }

        static constexpr bool lt(char_type c1, char_type c2) noexcept {
            return c1 < c2;
        }

        static constexpr int compare(const char_type* s1, const char_type* s2, std::size_t n) {
            for (std::size_t i = 0; i < n; i++) {
                if (lt(s1[i], s2[i])) {
                    return -1;
                } else if (lt(s2[i], s1[i])) {
                    return 1;
                }
            }
            return 0;
        }

        static constexpr std::size_t length(const char_type* s) {
            for (std::size_t i = 0; ; i++) {
                if (eq(s[i], char_type())) {
                    return i;
                }
            }
        }

        static constexpr const char_type* find(const char_type* s, std::size_t n, const char_type& a) {
            for (std::size_t i = 0; i < n; i++) {
                if (eq(s[i], a)) {
                    return s + i;
                }
            }
            return 0;
        }

        static constexpr char_type* move(char_type* s1, const char_type* s2, std::size_t n) {
            char_type* tmp = new char_type[n];
            for (std::size_t i = 0; i < n; i++) {
                tmp[i] = s2[i];
            } 
            for (std::size_t i = 0; i < n; i++) {
                s1[i] = tmp[i];
            }
            delete[] tmp;
            return s1;
        }

        static constexpr char_type* copy(char_type* s1, const char_type* s2, std::size_t n) {
            for (std::size_t i = 0; i < n; i++) {
                assign(s1[i], s2[i]);
            }
            return s1;
        }

        static constexpr char_type* assign(char_type* s, std::size_t n, char_type a) {
            for (std::size_t i = 0; i < n; i++) {
                assign(s[i], a);
            }
            return s;
        }

        static constexpr int not_eof(int_type c) noexcept {
            return eq_int_type(c, eof()) ? 0 : c;
        }

        static constexpr char_type to_char_type(int_type c) noexcept {
            return static_cast<char_type>(c);
        }

        static constexpr int_type to_int_type(char_type c) noexcept {
            return static_cast<int_type>(c);
        }

        static constexpr bool eq_int_type(int_type c1, int_type c2) noexcept {
            return c1 == c2;
        }

        static constexpr int_type eof() noexcept {
            return static_cast<int_type>(-1);
        }
    };

    template<> struct char_traits<char16_t> {
        using char_type = char16_t;
        using int_type = std::uint_least16_t;
        using off_type = streamoff;
        using pos_type = u16streampos;
        using state_type = std::mbstate_t;
        using comparison_category = strong_ordering;

        static constexpr void assign(char_type& c1, const char_type& c2) noexcept {
            c1 = c2;
        }

        static constexpr bool eq(char_type c1, char_type c2) noexcept {
            return c1 == c2;
        }

        static constexpr bool lt(char_type c1, char_type c2) noexcept {
            return c1 < c2;
        }

        static constexpr int compare(const char_type* s1, const char_type* s2, std::size_t n) {
            for (std::size_t i = 0; i < n; i++) {
                if (lt(s1[i], s2[i])) {
                    return -1;
                } else if (lt(s2[i], s1[i])) {
                    return 1;
                }
            }
            return 0;
        }

        static constexpr std::size_t length(const char_type* s) {
            for (std::size_t i = 0; ; i++) {
                if (eq(s[i], char_type())) {
                    return i;
                }
            }
        }

        static constexpr const char_type* find(const char_type* s, std::size_t n, const char_type& a) {
            for (std::size_t i = 0; i < n; i++) {
                if (eq(s[i], a)) {
                    return s + i;
                }
            }
            return 0;
        }

        static constexpr char_type* move(char_type* s1, const char_type* s2, std::size_t n) {
            char_type* tmp = new char_type[n];
            for (std::size_t i = 0; i < n; i++) {
                tmp[i] = s2[i];
            } 
            for (std::size_t i = 0; i < n; i++) {
                s1[i] = tmp[i];
            }
            delete[] tmp;
            return s1;
        }

        static constexpr char_type* copy(char_type* s1, const char_type* s2, std::size_t n) {
            for (std::size_t i = 0; i < n; i++) {
                assign(s1[i], s2[i]);
            }
            return s1;
        }

        static constexpr char_type* assign(char_type* s, std::size_t n, char_type a) {
            for (std::size_t i = 0; i < n; i++) {
                assign(s[i], a);
            }
            return s;
        }

        static constexpr int not_eof(int_type c) noexcept {
            return eq_int_type(c, eof()) ? 0 : c;
        }

        static constexpr char_type to_char_type(int_type c) noexcept {
            return static_cast<char_type>(c);
        }

        static constexpr int_type to_int_type(char_type c) noexcept {
            return static_cast<int_type>(c);
        }

        static constexpr bool eq_int_type(int_type c1, int_type c2) noexcept {
            return c1 == c2;
        }

        static constexpr int_type eof() noexcept {
            return static_cast<int_type>(-1);
        }
    };

    template<> struct char_traits<char32_t> {
        using char_type = char32_t;
        using int_type = std::uint_least32_t;
        using off_type = streamoff;
        using pos_type = u32streampos;
        using state_type = std::mbstate_t;
        using comparison_category = strong_ordering;

        static constexpr void assign(char_type& c1, const char_type& c2) noexcept {
            c1 = c2;
        }

        static constexpr bool eq(char_type c1, char_type c2) noexcept {
            return c1 == c2;
        }

        static constexpr bool lt(char_type c1, char_type c2) noexcept {
            return c1 < c2;
        }

        static constexpr int compare(const char_type* s1, const char_type* s2, std::size_t n) {
            for (std::size_t i = 0; i < n; i++) {
                if (lt(s1[i], s2[i])) {
                    return -1;
                } else if (lt(s2[i], s1[i])) {
                    return 1;
                }
            }
            return 0;
        }

        static constexpr std::size_t length(const char_type* s) {
            for (std::size_t i = 0; ; i++) {
                if (eq(s[i], char_type())) {
                    return i;
                }
            }
        }

        static constexpr const char_type* find(const char_type* s, std::size_t n, const char_type& a) {
            for (std::size_t i = 0; i < n; i++) {
                if (eq(s[i], a)) {
                    return s + i;
                }
            }
            return 0;
        }

        static constexpr char_type* move(char_type* s1, const char_type* s2, std::size_t n) {
            char_type* tmp = new char_type[n];
            for (std::size_t i = 0; i < n; i++) {
                tmp[i] = s2[i];
            } 
            for (std::size_t i = 0; i < n; i++) {
                s1[i] = tmp[i];
            }
            delete[] tmp;
            return s1;
        }

        static constexpr char_type* copy(char_type* s1, const char_type* s2, std::size_t n) {
            for (std::size_t i = 0; i < n; i++) {
                assign(s1[i], s2[i]);
            }
            return s1;
        }

        static constexpr char_type* assign(char_type* s, std::size_t n, char_type a) {
            for (std::size_t i = 0; i < n; i++) {
                assign(s[i], a);
            }
            return s;
        }

        static constexpr int not_eof(int_type c) noexcept {
            return eq_int_type(c, eof()) ? 0 : c;
        }

        static constexpr char_type to_char_type(int_type c) noexcept {
            return static_cast<char_type>(c);
        }

        static constexpr int_type to_int_type(char_type c) noexcept {
            return static_cast<int_type>(c);
        }

        static constexpr bool eq_int_type(int_type c1, int_type c2) noexcept {
            return c1 == c2;
        }

        static constexpr int_type eof() noexcept {
            return static_cast<int_type>(-1);
        }
    };

    template<> struct char_traits<wchar_t> {
        using char_type = wchar_t;
        using int_type = std::wint_t;
        using off_type = streamoff;
        using pos_type = wstreampos;
        using state_type = std::mbstate_t;
        using comparison_category = strong_ordering;

        static constexpr void assign(char_type& c1, const char_type& c2) noexcept {
            c1 = c2;
        }

        static constexpr bool eq(char_type c1, char_type c2) noexcept {
            return c1 == c2;
        }

        static constexpr bool lt(char_type c1, char_type c2) noexcept {
            return c1 < c2;
        }

        static constexpr int compare(const char_type* s1, const char_type* s2, std::size_t n) {
            for (std::size_t i = 0; i < n; i++) {
                if (lt(s1[i], s2[i])) {
                    return -1;
                } else if (lt(s2[i], s1[i])) {
                    return 1;
                }
            }
            return 0;
        }

        static constexpr std::size_t length(const char_type* s) {
            for (std::size_t i = 0; ; i++) {
                if (eq(s[i], char_type())) {
                    return i;
                }
            }
        }

        static constexpr const char_type* find(const char_type* s, std::size_t n, const char_type& a) {
            for (std::size_t i = 0; i < n; i++) {
                if (eq(s[i], a)) {
                    return s + i;
                }
            }
            return 0;
        }

        static constexpr char_type* move(char_type* s1, const char_type* s2, std::size_t n) {
            char_type* tmp = new char_type[n];
            for (std::size_t i = 0; i < n; i++) {
                tmp[i] = s2[i];
            } 
            for (std::size_t i = 0; i < n; i++) {
                s1[i] = tmp[i];
            }
            delete[] tmp;
            return s1;
        }

        static constexpr char_type* copy(char_type* s1, const char_type* s2, std::size_t n) {
            for (std::size_t i = 0; i < n; i++) {
                assign(s1[i], s2[i]);
            }
            return s1;
        }

        static constexpr char_type* assign(char_type* s, std::size_t n, char_type a) {
            for (std::size_t i = 0; i < n; i++) {
                assign(s[i], a);
            }
            return s;
        }

        static constexpr int not_eof(int_type c) noexcept {
            return eq_int_type(c, eof()) ? 0 : c;
        }

        static constexpr char_type to_char_type(int_type c) noexcept {
            return static_cast<char_type>(c);
        }

        static constexpr int_type to_int_type(char_type c) noexcept {
            return static_cast<int_type>(c);
        }

        static constexpr bool eq_int_type(int_type c1, int_type c2) noexcept {
            return c1 == c2;
        }

        static constexpr int_type eof() noexcept {
            return WEOF;
        }
    };

    /* 21.3.3 basic_string */
    template<class charT, class traits, class Allocator> 
    class basic_string {
    public:
        using traits_type = traits;
        using value_type = charT;
        using allocator_type = Allocator;
        using size_type = typename allocator_traits<Allocator>::size_type;
        using difference_type = typename allocator_traits<Allocator>::difference_type;
        using pointer = typename allocator_traits<Allocator>::pointer;
        using const_pointer = typename allocator_traits<Allocator>::const_pointer;
        using reference = value_type&;
        using const_reference = const value_type&;

        using iterator = value_type*;
        using const_iterator = const value_type*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    private:
        /* The maximum amount of characters (including the null-terminating character) that can be stored in the stack storage. */
        static constexpr size_type stack_capacity = 20;
        [[no_unique_address]] Allocator alloc;
        union {
            charT stack[stack_capacity];
            struct {
                size_type capacity;
                charT* buf;
            } heap;
            // pair<size_type, charT*> heap;
        };

        size_type len;
        bool is_heap_in_use;

    public:
        static constexpr size_type npos = -1;

        /* 21.3.3.3 Construct/copy/destroy */
        constexpr basic_string() noexcept(noexcept(Allocator())) : alloc(), stack{0}, len(0), is_heap_in_use(false) {}

        constexpr explicit basic_string(const Allocator& a) noexcept : alloc(a), stack{0}, len(0), is_heap_in_use(false) {}

        constexpr basic_string(const basic_string& str) : basic_string(str, allocator_traits<Allocator>::select_on_container_copy_construction(str.alloc)) {}

        constexpr basic_string(basic_string&& str) noexcept : basic_string(move(str), move(str).alloc) {}

        constexpr basic_string(const basic_string& str, size_type pos, const Allocator& a = Allocator()) 
            : basic_string(basic_string_view<charT, traits>(str).substr(pos, npos), a) {}

        constexpr basic_string(const basic_string& str, size_type pos, size_type n, const Allocator& a = Allocator()) 
            : basic_string(basic_string_view<charT, traits>(str).substr(pos, n), a) {}

        template<class T> 
        requires is_convertible_v<const T&, basic_string_view<charT, traits>>
        constexpr basic_string(const T& t, size_type pos, size_type n, const Allocator& a = Allocator()) 
            : basic_string(basic_string_view<charT, traits>(t).substr(pos, n), a) {}

        template<class T> 
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr explicit basic_string(const T& t, const Allocator& a = Allocator())
            : basic_string(basic_string_view<charT, traits>(t).data(), basic_string_view<charT, traits>(t).size(), a) {}

        constexpr basic_string(const charT* s, size_type n, const Allocator& a = Allocator())
            : basic_string(s, s + n, a) {}

        constexpr basic_string(const charT* s, const Allocator& a = Allocator())
            : basic_string(s, traits::length(s), a) {}

        constexpr basic_string(size_type n, charT c, const Allocator& a = Allocator())
            : alloc(a), stack(), len(n), is_heap_in_use(n + 1 > stack_capacity) {
            if (is_heap_in_use) {
                const size_type capacity = n + 1;
                charT* const buf = allocator_traits<Allocator>::template rebind_traits<charT>::allocate(alloc, capacity);
                traits::assign(buf, n, c);
                traits::assign(buf[n], 0);
                heap = { .capacity = capacity, .buf = buf };
            } else {
                traits::assign(stack, n, c);
                traits::assign(stack[n], 0);
            }
        }

        template<__internal::legacy_input_iterator InputIterator>
        constexpr basic_string(InputIterator begin, InputIterator end, const Allocator& a = Allocator())
            : alloc(a), stack(), len(0), is_heap_in_use(false) {
            /* Always points to the next address to write character to. */
            charT* it = stack;
            const auto add_character = [&](charT c) {
                if (!is_heap_in_use && len == stack_capacity) [[unlikely]] {
                    // Need to switch from stack to heap.
                    const size_type capacity = stack_capacity * 2;
                    charT* const new_buf = allocator_traits<Allocator>::template rebind_traits<charT>::allocate(alloc, capacity);
                    traits::copy(new_buf, stack, stack_capacity);
                    heap = { .capacity = capacity, .buf = new_buf };
                    
                    is_heap_in_use = true;
                    it = heap.buf + stack_capacity;
                } else if (is_heap_in_use && len == heap.capacity) [[unlikely]] {
                    // Need to allocate a larger buffer on the heap.
                    const size_type new_capacity = stack_capacity * 2;
                    charT* const new_buf = allocator_traits<Allocator>::template rebind_traits<charT>::allocate(alloc, new_capacity);
                    traits::copy(new_buf, heap.buf, len);
                    allocator_traits<Allocator>::template rebind_traits<charT>::deallocate(alloc, heap.buf, heap.capacity);

                    it = new_buf + heap.capacity;
                    heap = { .capacity = new_capacity, .buf = new_buf };
                }

                traits::assign(*it, c);
                len++;
                it++;
            };

            while (begin != end) {
                add_character(*begin);
            }

            add_character(0);
        }

        constexpr basic_string(initializer_list<charT> il, const Allocator& a = Allocator()) 
            : basic_string(il.begin(), il.end(), a) {}

        constexpr basic_string(const basic_string& str, const Allocator& a)
            : alloc(a), stack(), len(str.len), is_heap_in_use(len + 1 > stack_capacity) {
            if (is_heap_in_use) {
                charT* const new_addr = allocator_traits<Allocator>::template rebind_traits<charT>::allocate(alloc, str.heap.capacity);
                traits::copy(new_addr, str.heap.buf, len + 1);
                heap = { .capacity = str.heap.capacity, .buf = new_addr };
            } else {
                traits::copy(stack, str.stack, len + 1);
            }
        }

        constexpr basic_string(basic_string&& str, const Allocator& a) : alloc(a), stack(), len(str.len), is_heap_in_use(str.is_heap_in_use) {
            if (is_heap_in_use) {
                heap = str.heap;
                str.stack = {0};
                str.is_heap_in_use = false;
            } else {
                traits::copy(stack, str.stack, len + 1);
            }

            str.len = 0;
        }

        constexpr ~basic_string() {
            if (is_heap_in_use) {
                allocator_traits<Allocator>::template rebind_traits<charT>::deallocate(alloc, heap.buf, heap.capacity);
            }
        }

        constexpr basic_string& operator=(const basic_string& str) {
            if (*this != str) {
                if (is_heap_in_use) {
                    allocator_traits<Allocator>::template rebind_traits<charT>::deallocate(alloc, heap.buf, heap.capacity);
                }

                if constexpr (allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
                    alloc = str.alloc;
                }

                len = str.len;
                is_heap_in_use = str.is_heap_in_use;
                if (is_heap_in_use) {
                    charT* const this_buf = allocator_traits<Allocator>::template rebind_traits<charT>::allocate(alloc, str.heap.capacity);
                    traits::copy(this_buf, str.heap.buf, len + 1);
                    heap = { .capacity = str.heap.capacity, .buf = this_buf };
                } else {
                    traits::copy(stack, str.stack, len + 1);
                }
            }

            return *this;
        }

        constexpr basic_string& operator=(basic_string&& str)
        noexcept(allocator_traits<Allocator>::propagate_on_container_move_assignment::value || allocator_traits<Allocator>::is_always_equal::value) {
            if (is_heap_in_use) {
                allocator_traits<Allocator>::template rebind_traits<charT>::deallocate(alloc, heap.buf, heap.capacity);
            }
            
            len = str.len;
            is_heap_in_use = str.is_heap_in_use;

            if constexpr (allocator_traits<Allocator>::propagate_on_container_move_assignment::value) {
                alloc = str.alloc;
            }
            
            if (is_heap_in_use) {
                if constexpr (!allocator_traits<Allocator>::propagate_on_container_move_assignment::value && !allocator_traits<Allocator>::is_always_equal::value) {
                    charT* const this_buf = allocator_traits<Allocator>::template rebind_traits<charT>::allocate(alloc, str.heap.capacity);
                    traits::copy(this_buf, str.heap.buf, len + 1);
                    allocator_traits<Allocator>::template rebind_traits<charT>::deallocate(str.alloc, str.heap.buf, str.heap.capacity);
                    heap = { .capacity = str.heap.capacity, .buf = this_buf };
                } else {
                    heap = str.heap;
                }

                str.stack = {0};
                str.is_heap_in_use = false;
                str.len = 0;
            } else {
                traits::copy(stack, str.stack, len + 1);
            }

            return *this;
        }
        
        template<class T>
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr basic_string& operator=(const T& t) {
            return assign(basic_string_view<charT, traits>(t));
        }

        constexpr basic_string& operator=(const charT* s) {
            return *this = basic_string_view<charT, traits>(s);
        }

        constexpr basic_string& operator=(charT c) {
            return *this = basic_string_view<charT, traits>(addressof(c), 1);
        }

        constexpr basic_string& operator=(initializer_list<charT> il) {
            return *this = basic_string_view<charT, traits>(il.begin(), il.end());
        }

        /* 21.3.3.4 Iterators */
        constexpr iterator begin() noexcept {
            return data();
        }

        constexpr const_iterator begin() const noexcept{
            return data();
        }

        constexpr iterator end() noexcept {
            return begin() + size();
        }

        constexpr const_iterator end() const noexcept {
            return begin() + size();
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

        constexpr const_iterator cbegin() noexcept {
            return begin();
        }

        constexpr const_iterator cend() const noexcept {
            return end();
        }

        constexpr const_reverse_iterator crbegin() noexcept {
            return rbegin();
        }

        constexpr const_reverse_iterator crend() const noexcept {
            return rend();
        }

        /* 21.3.3.5 Capacity */
        constexpr size_type size() const noexcept {
            return len;
        }

        constexpr size_type length() const noexcept {
            return len;
        }

        constexpr size_type max_size() const noexcept {
            return numeric_limits<size_type>::max();
        }

        constexpr void resize(size_type n, charT c) {
            if (n == size()) {
                return;
            } else if (n > size()) {
                append(n - size(), c);
            }

            const bool new_buf_on_stack = n + 1 <= stack_capacity;

            len = n;
            if (new_buf_on_stack && !is_heap_in_use) {
                // Storage remains on the stack. So we just need to add a null-terminating character at the appropriate spot.
                traits::assign(stack[n], 0);
            } else if (!new_buf_on_stack && is_heap_in_use) {
                // Storage remains on the heap. We don't want to release overused capacity, so similarly add a null-terminating
                // character at the appropriate spot.
                traits::assign(heap.buf[n], 0);
            } else {
                // Storage needs to change from heap to stack.
                traits::copy(stack, heap.buf, n);
                traits::assign(stack[n], 0);
                is_heap_in_use = false;
                allocator_traits<Allocator>::template rebind_traits<charT>::deallocate(alloc, heap.buf, heap.capacity);
            }
        }

        constexpr void resize(size_type n) {
            resize(n, charT());
        }

        constexpr size_type capacity() const noexcept {
            // In our data structures we store the capacity including the null-terminating character, so need to subtract that from the result.
            return is_heap_in_use ? heap.first - 1 : stack_capacity - 1;
        }

        constexpr void reserve(size_type res_arg) {
            if (capacity() >= res_arg) {
                return;
            }

            // At this point, the new storage is guaranteed to be on the heap, with capacity res_arg.
            charT* const new_buf = allocator_traits<Allocator>::template rebind_traits<charT>::allocate(alloc, res_arg + 1);
            if (is_heap_in_use) {
                traits::copy(new_buf, heap.buf, len + 1);
                allocator_traits<Allocator>::template rebind_traits<charT>::deallocate(alloc, heap.buf, len + 1);
            } else {
                traits::copy(new_buf, stack, len + 1);
                is_heap_in_use = true;
            }
            heap = { .capacity = res_arg + 1, .buf = new_buf };
        }

        constexpr void shrink_to_fit() {
            resize(size());
        }

        constexpr void clear() noexcept {
            erase(begin(), end());
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return size() == 0;
        }

        /* 21.3.3.6 Element access */
        constexpr const_reference operator[](size_type pos) const noexcept {
            return begin()[pos];
        }

        constexpr reference operator[](size_type pos) noexcept {
            return begin()[pos];
        }

        constexpr const_reference at(size_type n) const {
            if (n >= size()) {
                throw out_of_range("Invalid arguments when calling basic_string::at.");
            }
            return operator[](n);
        }

        constexpr reference at(size_type n) {
            if (n >= size()) {
                throw out_of_range("Invalid arguments when calling basic_string::at.");
            }
            return operator[](n);
        }

        constexpr const charT& front() const {
            return operator[](0);
        }

        constexpr charT& front() {
            return operator[](0);
        }

        constexpr const charT& back() const {
            return operator[](size() - 1);
        }

        constexpr charT& back() {
            return operator[](size() - 1);
        }

        /* 21.3.3.7 Modifiers */
        constexpr basic_string& operator+=(const basic_string& str) {
            return append(str);
        }

        template<class T>
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr basic_string& operator+=(const T& t) {
            return append(basic_string_view<charT, traits>(t));
        }

        constexpr basic_string& operator+=(const charT* s) {
            return append(s);
        }

        constexpr basic_string& operator+=(charT c) {
            return append(size_type(1), c);
        }

        constexpr basic_string& operator+=(initializer_list<charT> il) {
            return append(il);
        }

        constexpr basic_string& append(const basic_string& str) {
            return append(str.data(), str.size());
        }

        constexpr basic_string& append(const basic_string& str, size_type pos, size_type n = npos) {
            return append(basic_string_view<charT, traits>(str).substr(pos, n));
        }

        template<class T>
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr basic_string& append(const T& t) {
            basic_string_view<charT, traits> sv = t;
            return append(sv.data(), sv.size());
        }

        template<class T>
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr basic_string& append(const T& t, size_type pos, size_type n = npos) {
            return append(basic_string_view<charT, traits>(t).substr(pos, n));
        }

        constexpr basic_string& append(const charT* s, size_type n) {
            const bool new_buf_on_heap = len + n + 1 > stack_capacity;
            if (new_buf_on_heap && is_heap_in_use) {
                // We stay on the heap.
                const auto [old_capacity, old_buf] = heap;
                if (size() + n + 1 > heap.capacity) {
                    // Needs to expand the capacity.
                    const size_type new_capacity = (size() + n) * 2;
                    charT* const new_buf = allocator_traits<Allocator>::template rebind_traits<charT>::allocate(alloc, new_capacity);
                    traits::copy(new_buf, heap.buf, len);
                    traits::copy(new_buf + len, s, n);
                    traits::assign(new_buf[len + n], 0);
                    len = size() + n;
                    allocator_traits<Allocator>::template rebind_traits<charT>::deallocate(alloc, heap.buf, heap.capacity);
                    heap = { .capacity = new_capacity, .buf = new_buf };
                } else {
                    traits::copy(heap.buf + len, s, n);
                    traits::assign(heap.buf[len + n], 0);
                    len = size() + n;
                }
            } else if (!new_buf_on_heap && !is_heap_in_use) {
                // We stay on the stack.
                traits::copy(stack + len, s, n);
                traits::assign(stack[len + n], 0);
                len = size() + n;
            } else {
                // Need to switch from stack to heap.
                const size_type new_capacity = (size() + n) * 2;
                charT* const new_buf = allocator_traits<Allocator>::template rebind_traits<charT>::allocate(alloc, new_capacity);
                traits::copy(new_buf, stack, len);
                traits::copy(new_buf + len, s, n);
                traits::assign(new_buf[len + n], 0);
                len = size() + n;
                heap = { .capacity = new_capacity, .buf = new_buf };
                is_heap_in_use = true;
            }
        }

        constexpr basic_string& append(const charT* s) {
            return append(s, traits::length(s));
        }

        constexpr basic_string& append(size_type n, charT c) {
            return append(basic_string(n, c, get_allocator()));
        }

        template<__internal::legacy_input_iterator InputIterator>
        constexpr basic_string& append(InputIterator first, InputIterator last) {
            return append(basic_string(first, last, get_allocator()));
        }

        constexpr basic_string& append(initializer_list<charT> il) {
            return append(il.begin(), il.end());
        }

        constexpr void push_back(charT c) {
            append(size_type(1), c);
        }

        constexpr basic_string& assign(const basic_string& str) {
            return *this = str;
        }

        constexpr basic_string& assign(basic_string&& str)
        noexcept(allocator_traits<Allocator>::propagate_on_container_move_assignment::value || allocator_traits<Allocator>::is_always_equal::value) {
            return *this = move(str);
        }

        constexpr basic_string& assign(const basic_string& str, size_type pos, size_type n = npos) {
            return assign(basic_string_view<charT, traits>(str).substr(pos, n));
        }

        template<class T>
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr basic_string& assign(const T& t) {
            basic_string_view<charT, traits> sv = t;
            return assign(sv.data(), sv.size());
        }

        template<class T>
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr basic_string& assign(const T& t, size_type pos, size_type n = npos) {
            return assign(basic_string_view<charT, traits>(t).substr(pos, n));
        }

        constexpr basic_string& assign(const charT* s, size_type n) {
            return *this = basic_string(s, n, get_allocator());
        }

        constexpr basic_string& assign(const charT* s) {
            return assign(s, traits::length(s));
        }

        constexpr basic_string& assign(size_type n, charT c) {
            const size_type curr_size = len;
            resize(n, c);

            if (is_heap_in_use) {
                traits::assign(heap.buf, min(n, curr_size), c);
            } else {
                traits::assign(stack, min(n, curr_size), c);
            }

            return *this;
        }

        template<__internal::legacy_input_iterator InputIterator>
        constexpr basic_string& assign(InputIterator first, InputIterator last) {
            return *this = basic_string(first, last, get_allocator());
        }

        constexpr basic_string& assign(initializer_list<charT> il) {
            return assign(il.begin(), il.size());
        }

        constexpr basic_string& insert(size_type pos, const basic_string& str) {
            return insert(pos, str.data(), str.size());
        }

        constexpr basic_string& insert(size_type pos1, const basic_string& str, size_type pos2, size_type n = npos) {
            return insert(pos1, basic_string_view<charT, traits>(str), pos2, n);
        }

        template<class T>
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr basic_string& insert(size_type pos, const T& t) {
            basic_string_view<charT, traits> sv = t;
            return insert(pos, sv.data(), sv.size());
        }

        template<class T>
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr basic_string& insert(size_type pos1, const T& t, size_type pos2, size_type n = npos) {
            return insert(pos1, basic_string_view<charT, traits>(t).substr(pos2, n));
        }

        constexpr basic_string& insert(size_type pos, const charT* s, size_type n) {
            if (pos > len) {
                throw out_of_range("Invalid argument when calling basic_string::insert.");
            } else if (n > max_size() - len) {
                throw length_error("Invalid argument when calling basic_string::insert.");
            }

            reserve(len + n);
            charT* const buf = is_heap_in_use ? heap.buf : stack;
            traits::copy(buf + pos + n, buf + pos, len - pos);
            traits::copy(buf + pos, s, n);
            len += n;
            traits::assign(buf[len], 0);
            return *this;
        }

        constexpr basic_string& insert(size_type pos, const charT* s) {
            return insert(pos, s, traits::length(s));
        }

        constexpr basic_string& insert(size_type pos, size_type n, charT c) {
            if (pos > len) {
                throw out_of_range("Invalid argument when calling basic_string::insert.");
            } else if (n > max_size() - len) {
                throw length_error("Invalid argument when calling basic_string::insert.");
            }

            reserve(len + n);
            charT* const buf = is_heap_in_use ? heap.buf : stack;
            traits::copy(buf + pos + n, buf + pos, len - pos);
            traits::assign(buf + pos, n, c);
            len += n;
            traits::assign(buf[len], 0);
            return *this;
        }

        constexpr iterator insert(const_iterator p, charT c) {
            const size_type pos = p - begin();
            insert(pos, 1, c);
            return begin() + pos;
        }

        constexpr iterator insert(const_iterator p, size_type n, charT c) {
            const size_type pos = p - begin();
            insert(pos, n, c);
            return begin() + pos;
        }

        template<__internal::legacy_input_iterator InputIterator>
        constexpr iterator insert(const_iterator p, InputIterator first, InputIterator last) {
            return insert(p - begin(), basic_string(first, last, get_allocator()));
        }

        constexpr iterator insert(const_iterator p, initializer_list<charT> il) {
            return insert(p, il.begin(), il.end());
        }

        constexpr basic_string& erase(size_type pos = 0, size_type n = npos) {
            if (pos > size()) {
                throw out_of_range("Invalid argument when calling basic_string::erase.");
            }

            const size_type xlen = min(n, size() - pos);
            charT* const buf = is_heap_in_use ? heap.buf : stack;
            traits::copy(buf + pos, buf + pos + xlen, len - pos - xlen);
            resize(len - xlen);
            traits::assign(buf[len], 0);

            return *this;
        }

        constexpr iterator erase(const_iterator p) {
            const size_type pos = p - begin();
            erase(p - begin(), 1);
            return begin() + pos;
        }

        constexpr iterator erase(const_iterator first, const_iterator last) {
            const size_type pos = first - begin();
            erase(first - begin(), last - first);
            return begin() + pos;
        }

        constexpr void pop_back() {
            erase(end() - 1);
        }

        constexpr basic_string& replace(size_type pos1, size_type n1, const basic_string& str) {
            return replace(pos1, n1, str.data(), str.size());
        }

        constexpr basic_string& replace(size_type pos1, size_type n1, const basic_string& str, size_type pos2, size_type n2 = npos) {
            return replace(pos1, n1, basic_string_view<charT, traits>(str).substr(pos2, n2));
        }

        template<class T>
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr basic_string& replace(size_type pos1, size_type n1, const T& t) {
            basic_string_view<charT, traits> sv = t;
            return replace(pos1, n1, sv.data(), sv.size());
        }

        template<class T>
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr basic_string& replace(size_type pos1, size_type n1, const T& t, size_type pos2, size_type n2 = npos) {
            return replace(pos1, n1, basic_string_view<charT, traits>(t).substr(pos2, n2));
        }

        constexpr basic_string& replace(size_type pos, size_type n1, const charT* s, size_type n2) {
            /* This is the length of the string to be removed. */
            if (pos > len) {
                throw out_of_range("Invalid argument when calling basic_string::replace.");
            }
            const size_type xlen = min(n1, size() - pos);
            if (size() - xlen >= max_size() - n2) {
                throw length_error("Invalid argument when calling basic_string::replace.");
            }

            const size_type new_len = len - xlen + n2;
            reserve(new_len);

            charT* const buf = is_heap_in_use ? heap.buf : stack;
            // Move the segment after the removed part to where it should be.
            traits::move(buf + pos + n2, buf + pos + xlen, len - pos - xlen);
            traits::copy(buf + pos, s, n2);
            len = new_len;
            traits::assign(buf[len], 0);
        }

        constexpr basic_string& replace(size_type pos, size_type n1, const charT* s) {
            return replace(pos, n1, s, traits::length(s));
        }

        constexpr basic_string& replace(size_type pos, size_type n1, size_type n2, charT c) {
            /* This is the length of the string to be removed. */
            if (pos > len) {
                throw out_of_range("Invalid argument when calling basic_string::replace.");
            }
            const size_type xlen = min(n1, size() - pos);
            if (size() - xlen >= max_size() - n2) {
                throw length_error("Invalid argument when calling basic_string::replace.");
            }

            const size_type new_len = len - xlen + n2;
            reserve(new_len);

            charT* const buf = is_heap_in_use ? heap.buf : stack;
            // Move the segment after the removed part to where it should be.
            traits::move(buf + pos + n2, buf + pos + xlen, len - pos - xlen);
            traits::assign(buf + pos, n2, c);
            len = new_len;
            traits::assign(buf[len], 0);
        }

        constexpr basic_string& replace(const_iterator i1, const_iterator i2, const basic_string& str) {
            return replace(i1, i2, basic_string_view<charT, traits>(str));
        }

        template<class T>
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr basic_string& replace(const_iterator i1, const_iterator i2, const T& t) {
            basic_string_view<charT, traits> sv = t;
            return replace(i1 - begin(), i2 - i1, sv.data(), sv.size());
        }

        constexpr basic_string& replace(const_iterator i1, const_iterator i2, const charT* s, size_type n) {
            return replace(i1, i2, basic_string_view<charT, traits>(s, n));
        }

        constexpr basic_string& replace(const_iterator i1, const_iterator i2, const charT* s) {
            return replace(i1, i2, basic_string_view<charT, traits>(s));
        }

        constexpr basic_string& replace(const_iterator i1, const_iterator i2, size_type n, charT c) {
            return replace(i1 - begin(), i2 - i1, n, c);
        }

        template<__internal::legacy_input_iterator InputIterator>
        constexpr basic_string& replace(const_iterator i1, const_iterator i2, InputIterator j1, InputIterator j2) {
            return replace(i1, i2, basic_string(j1, j2, get_allocator()));
        }

        constexpr basic_string& replace(const_iterator i1, const_iterator i2, initializer_list<charT> il) {
            return replace(i1, i2, il.begin(), il.size());
        }

        constexpr size_type copy(charT* s, size_type n, size_type pos = 0) const {
            return basic_string_view<charT, traits>(*this).copy(s, n, pos);
        }

        constexpr void swap(basic_string& str)
        noexcept(allocator_traits<Allocator>::propagate_on_container_swap::value || allocator_traits<Allocator>::is_always_equal::value) {
            using std::swap;
            if constexpr (allocator_traits<Allocator>::propagate_on_container_swap::value) {
                swap(alloc, str.alloc);
            } else if (!allocator_traits<Allocator>::is_always_equal::value) {
                // Triggers undefined behavior. Here we don't do anything.
                return;
            }

            swap(len, str.len);
            if (is_heap_in_use && str.is_heap_in_use) {
                swap(heap, str.heap);
            } else if (!is_heap_in_use && !str.is_heap_in_use) {
                swap(stack, str.stack);
            } else if (is_heap_in_use && !str.is_heap_in_use) {
                auto heap_info = heap;
                stack = { 0 };
                traits::copy(stack, str.stack, stack_capacity);
                str.heap = heap_info;
            } else {
                auto heap_info = str.heap;
                str.stack = { 0 };
                traits::copy(str.stack, stack, stack_capacity);
                heap = heap_info;
            }

            swap(is_heap_in_use, str.is_heap_in_use);
        }

        /* 21.3.3.8 String operations */
        constexpr const charT* c_str() const noexcept {
            return data();
        }

        constexpr const charT* data() const noexcept {
            return is_heap_in_use ? heap.buf : stack;
        }

        constexpr charT* data() noexcept {
            return is_heap_in_use ? heap.buf : stack;
        }

        constexpr operator basic_string_view<charT, traits>() const noexcept {
            return basic_string_view<charT, traits>(data(), size());
        }

        constexpr allocator_type get_allocator() const noexcept {
            return alloc;
        }

        template<class T> 
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr size_type find(const T& t, size_type pos = 0) const noexcept(is_nothrow_convertible_v<const T&, basic_string_view<charT, traits>>) {
            return basic_string_view<charT, traits>(*this).find(basic_string_view<charT, traits>(t), pos);
        }

        constexpr size_type find(const basic_string& str, size_type pos = 0) const noexcept {
            return find(basic_string_view<charT, traits>(str), pos);
        }

        constexpr size_type find(const charT* s, size_type pos, size_type n) const {
            return find(basic_string_view<charT, traits>(s, n), pos);
        }

        constexpr size_type find(const charT* s, size_type pos = 0) const {
            return find(basic_string_view<charT, traits>(s), pos);
        }

        constexpr size_type find(charT c, size_type pos = 0) const noexcept {
            return find(basic_string_view<charT, traits>(addressof(c), 1), pos);
        }

        template<class T> 
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr size_type rfind(const T& t, size_type pos = 0) const noexcept(is_nothrow_convertible_v<const T&, basic_string_view<charT, traits>>) {
            return basic_string_view<charT, traits>(*this).rfind(basic_string_view<charT, traits>(t), pos);
        }

        constexpr size_type rfind(const basic_string& str, size_type pos = 0) const noexcept {
            return rfind(basic_string_view<charT, traits>(str), pos);
        }

        constexpr size_type rfind(const charT* s, size_type pos, size_type n) const {
            return rfind(basic_string_view<charT, traits>(s, n), pos);
        }

        constexpr size_type rfind(const charT* s, size_type pos = 0) const {
            return rfind(basic_string_view<charT, traits>(s), pos);
        }

        constexpr size_type rfind(charT c, size_type pos = 0) const noexcept {
            return rfind(basic_string_view<charT, traits>(addressof(c), 1), pos);
        }

        template<class T> 
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr size_type find_first_of(const T& t, size_type pos = 0) const noexcept(is_nothrow_convertible_v<const T&, basic_string_view<charT, traits>>) {
            return basic_string_view<charT, traits>(*this).find_first_of(basic_string_view<charT, traits>(t), pos);
        }

        constexpr size_type find_first_of(const basic_string& str, size_type pos = 0) const noexcept {
            return find_first_of(basic_string_view<charT, traits>(str), pos);
        }

        constexpr size_type find_first_of(const charT* s, size_type pos, size_type n) const {
            return find_first_of(basic_string_view<charT, traits>(s, n), pos);
        }

        constexpr size_type find_first_of(const charT* s, size_type pos = 0) const {
            return find_first_of(basic_string_view<charT, traits>(s), pos);
        }

        constexpr size_type find_first_of(charT c, size_type pos = 0) const noexcept {
            return find_first_of(basic_string_view<charT, traits>(addressof(c), 1), pos);
        }

        template<class T> 
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr size_type find_last_of(const T& t, size_type pos = 0) const noexcept(is_nothrow_convertible_v<const T&, basic_string_view<charT, traits>>) {
            return basic_string_view<charT, traits>(*this).find_last_of(basic_string_view<charT, traits>(t), pos);
        }

        constexpr size_type find_last_of(const basic_string& str, size_type pos = 0) const noexcept {
            return find_last_of(basic_string_view<charT, traits>(str), pos);
        }

        constexpr size_type find_last_of(const charT* s, size_type pos, size_type n) const {
            return find_last_of(basic_string_view<charT, traits>(s, n), pos);
        }

        constexpr size_type find_last_of(const charT* s, size_type pos = 0) const {
            return find_last_of(basic_string_view<charT, traits>(s), pos);
        }

        constexpr size_type find_last_of(charT c, size_type pos = 0) const noexcept {
            return find_last_of(basic_string_view<charT, traits>(addressof(c), 1), pos);
        }

        template<class T> 
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr size_type find_first_not_of(const T& t, size_type pos = 0) const noexcept(is_nothrow_convertible_v<const T&, basic_string_view<charT, traits>>) {
            return basic_string_view<charT, traits>(*this).find_first_not_of(basic_string_view<charT, traits>(t), pos);
        }

        constexpr size_type find_first_not_of(const basic_string& str, size_type pos = 0) const noexcept {
            return find_first_not_of(basic_string_view<charT, traits>(str), pos);
        }

        constexpr size_type find_first_not_of(const charT* s, size_type pos, size_type n) const {
            return find_first_not_of(basic_string_view<charT, traits>(s, n), pos);
        }

        constexpr size_type find_first_not_of(const charT* s, size_type pos = 0) const {
            return find_first_not_of(basic_string_view<charT, traits>(s), pos);
        }

        constexpr size_type find_first_not_of(charT c, size_type pos = 0) const noexcept {
            return find_first_not_of(basic_string_view<charT, traits>(addressof(c), 1), pos);
        }

        template<class T> 
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr size_type find_last_not_of(const T& t, size_type pos = 0) const noexcept(is_nothrow_convertible_v<const T&, basic_string_view<charT, traits>>) {
            return basic_string_view<charT, traits>(*this).find_last_not_of(basic_string_view<charT, traits>(t), pos);
        }

        constexpr size_type find_last_not_of(const basic_string& str, size_type pos = 0) const noexcept {
            return find_last_not_of(basic_string_view<charT, traits>(str), pos);
        }

        constexpr size_type find_last_not_of(const charT* s, size_type pos, size_type n) const {
            return find_last_not_of(basic_string_view<charT, traits>(s, n), pos);
        }

        constexpr size_type find_last_not_of(const charT* s, size_type pos = 0) const {
            return find_last_not_of(basic_string_view<charT, traits>(s), pos);
        }

        constexpr size_type find_last_not_of(charT c, size_type pos = 0) const noexcept {
            return find_last_not_of(basic_string_view<charT, traits>(addressof(c), 1), pos);
        }

        constexpr basic_string substr(size_type pos = 0, size_type n = npos) const {
            if (pos > size()) {
                throw out_of_range("Invalid argument when calling basic_string::substr.");
            }
            return basic_string(data() + pos, min(n, size(), - pos));
        }

        template<class T>
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr int compare(const T& t) const noexcept(is_nothrow_convertible_v<const T&, basic_string_view<charT, traits>>) {
            return basic_string_view<charT, traits>(*this).compare(t);
        }

        template<class T>
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr int compare(size_type pos1, size_type n1, const T& t) const {
            return basic_string_view<charT, traits>(*this).substr(pos1, n1).compare(t);
        }

        template<class T>
        requires is_convertible_v<const T&, basic_string_view<charT, traits>> && (!is_convertible_v<const T&, const charT*>)
        constexpr int compare(size_type pos1, size_type n1, const T& t, size_type pos2, size_type n2 = npos) const {
            return basic_string_view<charT, traits>(*this).substr(pos1, n1).compare(basic_string_view<charT, traits>(t).substr(pos2, n2));
        }

        constexpr int compare(const basic_string& str) const noexcept {
            return compare(basic_string_view<charT, traits>(str));
        }

        constexpr int compare(size_type pos1, size_type n1, const basic_string& str) const {
            return compare(pos1, n1, basic_string_view<charT, traits>(str));
        }

        constexpr int compare(size_type pos1, size_type n1, const basic_string& str, size_type pos2, size_type n2 = npos) const {
            return compare(pos1, n1, basic_string_view<charT, traits>(str), pos2, n2);
        }

        constexpr int compare(const charT* s) const noexcept {
            return compare(basic_string_view<charT, traits>(s));
        }

        constexpr int compare(size_type pos1, size_type n1, const charT* s) const {
            return compare(pos1, n1, basic_string_view<charT, traits>(s));
        }

        constexpr int compare(size_type pos1, size_type n1, const charT* s, size_type pos2, size_type n2 = npos) const {
            return compare(pos1, n1, basic_string_view<charT, traits>(s), pos2, n2);
        }

        constexpr bool starts_with(basic_string_view<charT, traits> x) const noexcept {
            return basic_string_view<charT, traits>(data(), size()).starts_with(x);
        }

        constexpr bool starts_with(charT x) const noexcept {
            return basic_string_view<charT, traits>(data(), size()).starts_with(x);
        }

        constexpr bool starts_with(const charT* x) const {
            return basic_string_view<charT, traits>(data(), size()).starts_with(x);
        }

        constexpr bool ends_with(basic_string_view<charT, traits> x) const noexcept {
            return basic_string_view<charT, traits>(data(), size()).ends_with(x);
        }

        constexpr bool ends_with(charT x) const noexcept {
            return basic_string_view<charT, traits>(data(), size()).ends_with(x);
        }

        constexpr bool ends_with(const charT* x) const {
            return basic_string_view<charT, traits>(data(), size()).ends_with(x);
        }
    };

    template<class InputIterator, class Allocator = allocator<typename iterator_traits<InputIterator>::value_type>>
    basic_string(InputIterator, InputIterator, Allocator = Allocator()) 
        -> basic_string<typename iterator_traits<InputIterator>::value_type, char_traits<typename iterator_traits<InputIterator>::value_type>, Allocator>;

    template<class charT, class traits, class Allocator = allocator<charT>>
    explicit basic_string(basic_string_view<charT, traits>, const Allocator& = Allocator())
        -> basic_string<charT, traits, Allocator>;

    template<class charT, class traits, class Allocator = allocator<charT>>
    basic_string(basic_string_view<charT, traits>, typename allocator_traits<Allocator>::size_type, typename allocator_traits<Allocator>::size_type, const Allocator& = Allocator())
        -> basic_string<charT, traits, Allocator>;

    template<class charT, class traits, class Allocator>
    constexpr basic_string<charT, traits, Allocator> operator+(const basic_string<charT, traits, Allocator>& lhs, const basic_string<charT, traits, Allocator>& rhs) {
        basic_string<charT, traits, Allocator> r = lhs;
        r.append(rhs);
        return r;
    }

    template<class charT, class traits, class Allocator>
    constexpr basic_string<charT, traits, Allocator> operator+(basic_string<charT, traits, Allocator>&& lhs, const basic_string<charT, traits, Allocator>& rhs) {
        lhs.append(rhs);
        return move(lhs);
    }

    template<class charT, class traits, class Allocator>
    constexpr basic_string<charT, traits, Allocator> operator+(const basic_string<charT, traits, Allocator>& lhs, basic_string<charT, traits, Allocator>&& rhs) {
        rhs.insert(0, lhs);
        return move(rhs);
    }

    template<class charT, class traits, class Allocator>
    constexpr basic_string<charT, traits, Allocator> operator+(basic_string<charT, traits, Allocator>&& lhs, basic_string<charT, traits, Allocator>&& rhs) {
        lhs.append(rhs);
        return move(lhs);
    }

    template<class charT, class traits, class Allocator>
    constexpr basic_string<charT, traits, Allocator> operator+(const charT* lhs, const basic_string<charT, traits, Allocator>& rhs) {
        basic_string<charT, traits, Allocator> r = rhs;
        r.insert(0, lhs);
        return r;
    }

    template<class charT, class traits, class Allocator>
    constexpr basic_string<charT, traits, Allocator> operator+(const charT* lhs, basic_string<charT, traits, Allocator>&& rhs) {
        rhs.insert(0, lhs);
        return move(rhs);
    }

    template<class charT, class traits, class Allocator>
    constexpr basic_string<charT, traits, Allocator> operator+(charT lhs, const basic_string<charT, traits, Allocator>& rhs) {
        basic_string<charT, traits, Allocator> r = rhs;
        r.insert(r.begin(), lhs);
        return r;
    }

    template<class charT, class traits, class Allocator>
    constexpr basic_string<charT, traits, Allocator> operator+(charT lhs, basic_string<charT, traits, Allocator>&& rhs) {
        rhs.insert(rhs.begin(), lhs);
        return move(rhs);
    }

    template<class charT, class traits, class Allocator>
    constexpr basic_string<charT, traits, Allocator> operator+(const basic_string<charT, traits, Allocator>& lhs, const charT* rhs) {
        basic_string<charT, traits, Allocator> r = lhs;
        r.append(rhs);
        return r;
    }

    template<class charT, class traits, class Allocator>
    constexpr basic_string<charT, traits, Allocator> operator+(basic_string<charT, traits, Allocator>&& lhs, const charT* rhs) {
        lhs.append(rhs);
        return move(lhs);
    }

    template<class charT, class traits, class Allocator>
    constexpr basic_string<charT, traits, Allocator> operator+(const basic_string<charT, traits, Allocator>& lhs, charT rhs) {
        basic_string<charT, traits, Allocator> r = lhs;
        r.push_back(rhs);
        return r;
    }

    template<class charT, class traits, class Allocator>
    constexpr basic_string<charT, traits, Allocator> operator+(basic_string<charT, traits, Allocator>&& lhs, charT rhs) {
        lhs.push_back(rhs);
        return move(lhs);
    }

    template<class charT, class traits, class Allocator>
    constexpr bool operator==(const basic_string<charT, traits, Allocator>& lhs, const basic_string<charT, traits, Allocator>& rhs) {
        return basic_string_view<charT, traits>(lhs) == basic_string_view<charT, traits>(rhs);
    }

    template<class charT, class traits, class Allocator>
    constexpr bool operator==(const basic_string<charT, traits, Allocator>& lhs, const charT* rhs) {
        return basic_string_view<charT, traits>(lhs) == basic_string_view<charT, traits>(rhs);
    }

    template<class charT, class traits, class Allocator>
    constexpr auto operator<=>(const basic_string<charT, traits, Allocator>& lhs, const basic_string<charT, traits, Allocator>& rhs) {
        return basic_string_view<charT, traits>(lhs) <=> basic_string_view<charT, traits>(rhs);
    }

    template<class charT, class traits, class Allocator>
    constexpr auto operator<=>(const basic_string<charT, traits, Allocator>& lhs, const charT* rhs) {
        return basic_string_view<charT, traits>(lhs) <=> basic_string_view<charT, traits>(rhs);
    }

    template<class charT, class traits, class Allocator>
    constexpr void swap(basic_string<charT, traits, Allocator>& lhs, basic_string<charT, traits, Allocator>& rhs)
    noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }

    template<class charT, class traits, class Allocator, class U>
    constexpr typename basic_string<charT, traits, Allocator>::size_type erase(basic_string<charT, traits, Allocator>& c, const U& value) {
        charT* const it = remove(c.begin(), c.end(), value);
        typename basic_string<charT, traits, Allocator>::size_type r = distance(it, c.end());
        c.erase(it, c.end());
        return r;
    }
    
    template<class charT, class traits, class Allocator, class Predicate>
    constexpr typename basic_string<charT, traits, Allocator>::size_type erase_if(basic_string<charT, traits, Allocator>& c, Predicate pred) {
        charT* const it = remove_if(c.begin(), c.end(), pred);
        typename basic_string<charT, traits, Allocator>::size_type r = distance(it, c.end());
        c.erase(it, c.end());
        return r;
    }

    /* 21.3.5 Numeric conversions */
    int stoi(const string& str, std::size_t* idx = nullptr, int base = 10);
    long stol(const string& str, std::size_t* idx = nullptr, int base = 10);
    unsigned long stoul(const string& str, std::size_t* idx = nullptr, int base = 10);
    long long stoll(const string& str, std::size_t* idx = nullptr, int base = 10);
    unsigned long long stoull(const string& str, std::size_t* idx = nullptr, int base = 10);
    float stof(const string& str, std::size_t* idx = nullptr);
    double stod(const string& str, std::size_t* idx = nullptr);
    long double stold(const string& str, std::size_t* idx = nullptr);
    
    string to_string(int val);
    string to_string(unsigned int val);
    string to_string(long val);
    string to_string(unsigned long val);
    string to_string(long long val);
    string to_string(unsigned long long val);
    string to_string(float val);
    string to_string(double val);
    string to_string(long double val);
    
    int stoi(const wstring& str, std::size_t* idx = nullptr, int base = 10);
    long stol(const wstring& str, std::size_t* idx = nullptr, int base = 10);
    unsigned long stoul(const wstring& str, std::size_t* idx = nullptr, int base = 10);
    long long stoll(const wstring& str, std::size_t* idx = nullptr, int base = 10);
    unsigned long long stoull(const wstring& str, std::size_t* idx = nullptr, int base = 10);
    float stof(const wstring& str, std::size_t* idx = nullptr);
    double stod(const wstring& str, std::size_t* idx = nullptr);
    long double stold(const wstring& str, std::size_t* idx = nullptr);
    
    wstring to_wstring(int val);
    wstring to_wstring(unsigned int val);
    wstring to_wstring(long val);
    wstring to_wstring(unsigned long val);
    wstring to_wstring(long long val);
    wstring to_wstring(unsigned long long val);
    wstring to_wstring(float val);
    wstring to_wstring(double val);
    wstring to_wstring(long double val);
    namespace pmr {
        template<class charT, class traits = char_traits<charT>>
        using basic_string = std::basic_string<charT, traits, polymorphic_allocator<charT>>;

        using string = basic_string<char>;
        using u8string = basic_string<char8_t>;
        using u16string = basic_string<char16_t>;
        using u32string = basic_string<char32_t>;
        using wstring = basic_string<wchar_t>;
    }

    /* 21.3.6 Hash support */
    template<> struct hash<string> {
        std::size_t operator()(const string& key) const noexcept;
    };

    template<> struct hash<u8string> {
        std::size_t operator()(const u8string& key) const noexcept;
    };

    template<> struct hash<u16string> {
        std::size_t operator()(const u16string& key) const noexcept;
    };

    template<> struct hash<u32string> {
        std::size_t operator()(const u32string& key) const noexcept;
    };

    template<> struct hash<wstring> {
        std::size_t operator()(const wstring& key) const noexcept;
    };

    template<> struct hash<pmr::string> {
        std::size_t operator()(const pmr::string& key) const noexcept;
    };

    template<> struct hash<pmr::u8string> {
        std::size_t operator()(const pmr::u8string& key) const noexcept;
    };

    template<> struct hash<pmr::u16string> {
        std::size_t operator()(const pmr::u16string& key) const noexcept;
    };

    template<> struct hash<pmr::u32string> {
        std::size_t operator()(const pmr::u32string& key) const noexcept;
    };

    template<> struct hash<pmr::wstring> {
        std::size_t operator()(const pmr::wstring& key) const noexcept;
    };

    inline namespace literals {
        inline namespace string_literals {
            inline string operator""s(const char* str, std::size_t len) {
                return {str, len};
            }

            inline u8string operator""s(const char8_t* str, std::size_t len) {
                return {str, len};
            }

            inline u16string operator""s(const char16_t* str, std::size_t len) {
                return {str, len};
            }

            inline u32string operator""s(const char32_t* str, std::size_t len) {
                return {str, len};
            }

            inline wstring operator""s(const wchar_t* str, std::size_t len) {
                return {str, len};
            }
        }
    }
}