#pragma once

#include "stddef.h"

#include "type_traits/is_integral.hpp"

namespace std {
    /* 17.2.3 Null pointers */
    using nullptr_t = decltype(nullptr);

    /* 17.2.4 Sizes, alignments, and offsets */
    using ::ptrdiff_t;
    using ::size_t;
    using ::max_align_t;

    /* 17.2.5 byte type operations */
    enum class byte : unsigned char {};

    template<class IntType>
    constexpr byte& operator<<=(byte& b, IntType shift) noexcept {
        static_assert(__internal::is_integral<IntType>::value, "Parameter shift must be of an integral type.");
        return b = b << shift;
    }
    
    template<class IntType>
    constexpr byte operator<<(byte b, IntType shift) noexcept {
        static_assert(__internal::is_integral<IntType>::value, "Parameter shift must be of an integral type.");
        return static_cast<byte>(static_cast<unsigned int>(b) << shift);
    }

    template<class IntType>
    constexpr byte& operator>>=(byte& b, IntType shift) noexcept {
        static_assert(__internal::is_integral<IntType>::value, "Parameter shift must be of an integral type.");
        return b = b >> shift;
    }
    
    template<class IntType>
    constexpr byte operator>>(byte b, IntType shift) noexcept {
        static_assert(__internal::is_integral<IntType>::value, "Parameter shift must be of an integral type.");
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

    template<class IntType>
    constexpr IntType to_integer(byte b) noexcept {
        static_assert(__internal::is_integral<IntType>::value, "Return type of this function must be of an integral type.");
        return static_cast<IntType>(b);
    }
}