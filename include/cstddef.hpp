#pragma once

#include <stddef.h>

#include "concepts.hpp"
#include "util/nullptr_t.hpp"
#include "util/max_align_t.hpp"

namespace yilib {
    /* 17.2.3 Null pointers */
    using __internal::nullptr_t;

    /* 17.2.4 Sizes, alignments, and offsets */
    using ::ptrdiff_t;
    using size_t = ::size_t;
    using __internal::max_align_t;

    /* 17.2.5 byte type operations */
    enum class byte : unsigned char {};

    template<integral IntType>
    constexpr byte& operator<<=(byte& b, IntType shift) noexcept {
        return b = b << shift;
    }
    
    template<integral IntType>
    constexpr byte operator<<(byte b, IntType shift) noexcept {
        return static_cast<byte>(static_cast<unsigned int>(b) << shift);
    }

    template<integral IntType>
    constexpr byte& operator>>=(byte& b, IntType shift) noexcept {
        return b = b >> shift;
    }
    
    template<integral IntType>
    constexpr byte operator>>(byte b, IntType shift) noexcept {
        return static_cast<byte>(static_cast<unsigned int>(b) >> shift);
    }

    constexpr byte operator|(byte l, byte r) noexcept {
        return static_cast<byte>(static_cast<unsigned int>(l) | static_cast<unsigned int>(r));
    }

    constexpr byte& operator|=(byte& l, byte r) noexcept {
        return l = l | r;
    }

    constexpr byte operator&(byte l, byte r) noexcept {
        return static_cast<byte>(static_cast<unsigned int>(l) & static_cast<unsigned int>(r));
    }

    constexpr byte& operator&=(byte& l, byte r) noexcept {
        return l = l & r;
    }

    constexpr byte operator^(byte l, byte r) noexcept {
        return static_cast<byte>(static_cast<unsigned int>(l) ^ static_cast<unsigned int>(r));
    }

    constexpr byte& operator^=(byte& l, byte r) noexcept {
        return l = l ^ r;
    }

    constexpr byte operator~(byte b) noexcept {
        return static_cast<byte>(~static_cast<unsigned int>(b));
    }

    template<integral IntType>
    constexpr IntType to_integer(byte b) noexcept {
        return static_cast<IntType>(b);
    }
}