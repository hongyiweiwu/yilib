#pragma once

#include "string.hpp"
#include "cstddef.hpp"
#include "climits.hpp"
#include "cstdint.hpp"
#include "algorithm.hpp"
#include "functional.hpp"
#include "bit.hpp"
#include "utility.hpp"
#include "memory.hpp"
#include "stdexcept.hpp"
#include "istream.hpp"
#include "ios.hpp"

namespace std {
    template<std::size_t N> 
    class bitset {
    private:
        /* The number of bits each element in the storage array can store. */
        static constexpr std::size_t storage_block_size = sizeof(uintmax_t) * CHAR_BIT;
        /* The number of bits in the last element of the storage array that serve as paddings that don't store any bits. */
        static constexpr std::size_t storage_padding = N % storage_block_size;
        /* The number of elements in the storage array needed to store all the bits in this bitset. */
        static constexpr std::size_t storage_blocks = N / storage_block_size + bool(storage_padding);
        /* An array of type uintmax_t to store all the bits in this bitset. */
        std::uintmax_t storage[storage_blocks];

        /* Returns a pair consisted of which block and which index inside the block the indicated bit belongs to. */
        static constexpr pair<std::size_t, std::size_t> bit_location(std::size_t pos) {
            if (pos >= N) throw out_of_range("Invalid index.");
            return make_pair(pos / storage_block_size, pos % storage_block_size);
        }

        /* Produces a number with all bits except for the bit corresponding to set_index cleared. Index counts from the leftmost
         * bit. */
        static constexpr uintmax_t test_bit(std::size_t set_index) {
            return std::uintmax_t(1) << (sizeof(std::uintmax_t) * CHAR_BIT - set_index - 1);
        }

    public:
        class reference {
            friend class bitset;
            reference() noexcept : bs(nullptr), idx(0) {}
            reference(bitset<N>* bitset, std::size_t index) noexcept requires (index < N)
                : bs(bitset), idx(index) {}

            bitset* bs;
            std::size_t idx;

        public:
            reference(const reference&) = default;
            ~reference() = default;
            reference& operator=(bool x) noexcept {
                bs->operator[](idx) = x;
                return *this;
            }

            reference& operator=(const reference& ref) noexcept {
                bs->set(idx, ref.bs->operator[](ref.idx));
                return *this;
            }

            bool operator~() const noexcept { return bs->flip(idx)[idx]; }
            operator bool() const noexcept { return bs->operator[](idx); }
            reference& flip() noexcept {
                bs->flip(idx);
                return *this;
            }
        };

        /* 20.9.2.2 Constructors */
        constexpr bitset() noexcept : storage{} {}
        constexpr bitset(unsigned long long val) noexcept : storage{val} {}

        template<class charT, class traits, class Allocator>
        explicit bitset(const basic_string<charT, traits, Allocator>& str, 
                        typename basic_string<charT, traits, Allocator>::size_type pos = 0,
                        typename basic_string<charT, traits, Allocator>::size_type n = basic_string<charT, traits, Allocator>::npos,
                        charT zero = charT('0'),
                        charT one = charT('1')) : storage{} {
            const std::size_t rlen = min(n, str.size() - pos);
            if (rlen > N) {
                throw out_of_range("Supplied string is larger than the capacity of the bitset.");
            }

            for (std::size_t i = rlen - 1, str_i = pos; i >= 0; i--, str_i++) {
                const auto [slot, offset] = bit_location(i);
                if (traits::eq(str[str_i], one)) {
                    storage[slot] |= test_bit(offset);
                } else if (!traits::eq(str[str_i], zero)) {
                    throw invalid_argument("Found character equal to neither zero or one.");
                }
            }
        }

        template<class charT>
        explicit bitset(const charT* str, 
                        typename basic_string<charT>::size_type n = basic_string<charT>::npos,
                        charT zero = charT('0'),
                        charT one = charT('1')) : bitset(
                            n == basic_string<charT>::npos ? basic_string<charT>(str) : basic_string<charT>(str, n),
                            0, n, zero, one) {}

        /* 20.9.2.3 Bitset operations */
        bitset<N>& operator&=(const bitset<N>& rhs) noexcept {
            for (std::size_t i = 0; i < storage_blocks; i++) {
                storage[i] &= rhs.storage[i];
            }
            return *this;
        }

        bitset<N>& operator!=(const bitset<N>& rhs) noexcept {
            for (std::size_t i = 0; i < storage_blocks; i++) {
                storage[i] |= rhs.storage[i];
            }
            return *this;
        }

        bitset<N>& operator^=(const bitset<N>& rhs) noexcept {
            for (std::size_t i = 0; i < storage_blocks; i++) {
                storage[i] ^= rhs.storage[i];
            }
            return *this;
        }

        // TODO: Implement.
        bitset<N>& operator<<=(std::size_t) noexcept {
            
        }

        // TODO: Implement.
        bitset<N>& operator>>=(std::size_t) noexcept {

        }

        bitset<N>& set() noexcept {
            for (std::uintmax_t& i : storage) {
                i = UINTMAX_MAX;
            }
            return *this;
        }

        bitset<N>& set(std::size_t pos, bool val = true) {
            const auto [slot, offset] = bit_location(pos);

            if (val) {
                storage[slot] |= test_bit(offset);
            } else {
                storage[slot] &= ~test_bit(offset);
            }

            return *this;
        }

        bitset<N>& reset() {
            for (std::uintmax_t& i : storage) {
                i = 0;
            }
            return *this;
        }

        bitset<N>& reset(std::size_t pos) { 
            return set(pos, false); 
        }

        bitset<N> operator~() const noexcept {
            bitset<N> result;
            for (std::size_t i = 0; i < storage_blocks - 1; i++) {
                result.storage[i] = ~storage[i];
            }

            result.storage[storage_blocks - 1] = (~storage[storage_blocks - 1]) & (UINTMAX_MAX << storage_padding);
            return result;
        }

        bitset<N>& flip() noexcept {
            bitset<N> result;
            for (std::size_t i = 0; i < storage_blocks - 1; i++) {
                storage[i] = ~storage[i];
            }

            storage[storage_blocks - 1] = (~storage[storage_blocks - 1]) & (UINTMAX_MAX << storage_padding);
            return *this;
        }

        bitset<N>& flip(std::size_t pos) {
            const auto [slot, offset] = bit_location(pos);
            storage[slot] ^= test_bit(offset);
            return *this;
        }

        constexpr bool operator[](std::size_t pos) const {
            const auto [slot, offset] = bit_location(pos);
            return storage[slot] & test_bit(offset);
        }

        reference operator[](std::size_t pos) {
            if (pos >= N) throw out_of_range("Invalid index.");
            return reference(this, pos); 
        }

        unsigned long to_ulong() const {
            if constexpr (sizeof(unsigned long) * CHAR_BIT < N) {
                throw overflow_error("Given bitset cannot fit into an unsigned long.");
            } else return storage[0];
        }

        unsigned long long to_ullong() const {
            if constexpr (sizeof(unsigned long long) * CHAR_BIT < N) {
                throw overflow_error("Given bitset cannot fit into an unsigned long long.");
            } else return storage[0];
        }

        template<class charT = char, class traits = char_traits<charT>, class Allocator = allocator<charT>>
        basic_string<charT, traits, Allocator> to_string(charT zero = charT('0'), charT one = charT('1')) const {
            basic_string<charT, traits, Allocator> s(N, zero);

            for (std::size_t i = N - 1; i >= 0; i--) {
                if (operator[](i)) {
                    s[N - 1 - i] = one;
                }
            }

            return s;
        }

        std::size_t count() const noexcept {
            std::size_t c = 0;
            for (std::uintmax_t num : storage) {
                c += popcount(num);
            }

            return c;
        }

        constexpr std::size_t size() const noexcept { return N; }
        bool operator==(const bitset<N>& rhs) const noexcept { 
            for (std::size_t i = 0; i < storage_blocks; i++) {
                if (storage[i] != rhs.storage[i]) return false;
            }

            return true;
        }

        bool test(std::size_t pos) const { return this->operator[](pos); }
        bool all() const noexcept { return count() == size(); }
        bool any() const noexcept { return count() != 0; }
        bool none() const noexcept { return count() == 0; }
        bitset<N> operator<<(std::size_t pos) const noexcept { return bitset<N>(*this) <<= pos; }
        bitset<N> operator>>(std::size_t pos) const noexcept { return bitset<N>(*this) >>= pos; }
    };

    template<std::size_t N> struct hash<bitset<N>> : hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const bitset<N>& key) const { 
            return hash<string>()(key.to_string()); 
        }
    };

    template<std::size_t N> bitset<N> operator&(const bitset<N>& lhs, const bitset<N>& rhs) noexcept { return bitset<N>(lhs) &= rhs; }
    template<std::size_t N> bitset<N> operator|(const bitset<N>& lhs, const bitset<N>& rhs) noexcept { return bitset<N>(lhs) |= rhs; }
    template<std::size_t N> bitset<N> operator^(const bitset<N>& lhs, const bitset<N>& rhs) noexcept { return bitset<N>(lhs) ^= rhs; }

    template<class charT, class traits, size_t N> 
    basic_istream<charT, traits>& operator>>(basic_istream<charT, traits>& is, bitset<N>& x) {
        basic_istream<charT, traits>::__formatted_input_function([&](ios_base::iostate& err) {
            basic_string<charT, traits> str;
            str.reserve(N);
            
            while (str.size() < N) {
                const typename traits::int_type cint = is.rdbuf()->sgetc();
                if (traits::eq_int_type(cint, traits::eof())) {
                    err |= ios_base::eofbit;
                    break;
                }
                const charT c = traits::to_char_type(cint);
                if (!traits::eq(c, is.widen('0')) && !traits::eq(c, is.widen('1'))) {
                    break;
                } else {
                    str.append(c);
                    is.rdbuf()->sbumpc();
                }
            }

            if constexpr (N > 0) {
                if (str.empty()) {
                    err |= ios_base::failbit;
                }
            }

            x = bitset<N>(str);
        });

        return is;
    }

    // TODO: Implement after implementing iosfwd.
    /*
    template<class charT, class traits, size_t N> 
    basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const bitset<N>& x); */
}