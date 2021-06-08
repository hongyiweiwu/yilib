#pragma once

#include "util/macros.hpp"
#include "type_traits.hpp"
#include "limits.hpp"
#include "concepts.hpp"

namespace std {
    /* 26.5.3 bit_cast */
#if __has_intrinsics_for(builtin_bit_cast)
    template<class To, class From> requires (sizeof(To) == sizeof(From))
        && is_trivially_copyable_v<To> && is_trivially_copyable_v<From>
    constexpr To bit_cast(const From& from) noexcept {
        return __builtin_bit_cast(To, from);
    }
#endif

    /* 26.5.6 Counting */
    template<unsigned_integral T> requires (!same_as<T, bool>) && (!same_as<T, char>)
        && (!same_as<T, char8_t>) && (!same_as<T, char16_t>)
        && (!same_as<T, char32_t>) && (!same_as<T, wchar_t>) 
    constexpr int countl_zero(T x) noexcept {
        if (x == 0) [[unlikely]] return numeric_limits<T>::digits;
        else return __builtin_clzll(x) - (numeric_limits<unsigned long long>::digits - numeric_limits<T>::digits);
    }

    template<unsigned_integral T> requires (!same_as<T, bool>) && (!same_as<T, char>)
        && (!same_as<T, char8_t>) && (!same_as<T, char16_t>)
        && (!same_as<T, char32_t>) && (!same_as<T, wchar_t>) 
    constexpr int countl_one(T x) noexcept {
        if (x == numeric_limits<T>::max()) [[unlikely]] return numeric_limits<T>::digits;
        else return countl_zero(T(~x)); // Recasts ~x back to type T to undo integral promotion.
    }

    template<unsigned_integral T> requires (!same_as<T, bool>) && (!same_as<T, char>)
        && (!same_as<T, char8_t>) && (!same_as<T, char16_t>)
        && (!same_as<T, char32_t>) && (!same_as<T, wchar_t>) 
    constexpr int countr_zero(T x) noexcept {
        if (x == 0) [[unlikely]] return numeric_limits<T>::digits;
        else return __builtin_ctzll(x); 
    }

    template<unsigned_integral T> requires (!same_as<T, bool>) && (!same_as<T, char>)
        && (!same_as<T, char8_t>) && (!same_as<T, char16_t>)
        && (!same_as<T, char32_t>) && (!same_as<T, wchar_t>) 
    constexpr int countr_one(T x) noexcept { 
        if (x == numeric_limits<T>::max()) [[unlikely]] return numeric_limits<T>::digits;
        else return countr_zero(T(~x)); // Recasts ~x back to type T to undo integral promotion.
    }

    template<unsigned_integral T> requires (!same_as<T, bool>) && (!same_as<T, char>)
        && (!same_as<T, char8_t>) && (!same_as<T, char16_t>)
        && (!same_as<T, char32_t>) && (!same_as<T, wchar_t>) 
    constexpr int popcount(T x) noexcept { return __builtin_popcount(x); }

    /* 26.5.4 Integral powers of 2 */
    template<unsigned_integral T> requires (!same_as<T, bool>) && (!same_as<T, char>)
        && (!same_as<T, char8_t>) && (!same_as<T, char16_t>)
        && (!same_as<T, char32_t>) && (!same_as<T, wchar_t>)
    constexpr bool has_single_bit(T x) noexcept {
        return popcount(x) == 1;
    }

    template<unsigned_integral T> requires (!same_as<T, bool>) && (!same_as<T, char>)
        && (!same_as<T, char8_t>) && (!same_as<T, char16_t>)
        && (!same_as<T, char32_t>) && (!same_as<T, wchar_t>)
    constexpr T bit_width(T x) noexcept {
        if (x == 0) [[unlikely]] return 0;
        else return numeric_limits<T>::digits - countl_zero(x);
    }

    template<unsigned_integral T> requires (!same_as<T, bool>) && (!same_as<T, char>)
        && (!same_as<T, char8_t>) && (!same_as<T, char16_t>)
        && (!same_as<T, char32_t>) && (!same_as<T, wchar_t>)
    constexpr T bit_ceil(T x) {
        if (has_single_bit(x)) return x;
        else return 1 << bit_width(x);
    }

    template<unsigned_integral T> requires (!same_as<T, bool>) && (!same_as<T, char>)
        && (!same_as<T, char8_t>) && (!same_as<T, char16_t>)
        && (!same_as<T, char32_t>) && (!same_as<T, wchar_t>)
    constexpr T bit_floor(T x) {
        if (x == 0) [[unlikely]] return 0;
        else return 1 << (bit_width(x) - 1);
    }

    /* 26.5.5 Rotating */
    template<class T> requires (!same_as<T, bool>) && (!same_as<T, char>)
        && (!same_as<T, char8_t>) && (!same_as<T, char16_t>)
        && (!same_as<T, char32_t>) && (!same_as<T, wchar_t>) 
    [[nodiscard]] constexpr T rotl(T x, int s) noexcept {
        auto r = s % numeric_limits<T>::digits;
        if (r == 0) return x;
        else if (r > 0) return (x << r) | (x >> (numeric_limits<T>::digits - r));
        else return (x >> -r) | (x << (numeric_limits<T>::digits + r));
    }

    template<class T> requires (!same_as<T, bool>) && (!same_as<T, char>)
        && (!same_as<T, char8_t>) && (!same_as<T, char16_t>)
        && (!same_as<T, char32_t>) && (!same_as<T, wchar_t>) 
    [[nodiscard]] constexpr T rotr(T x, int s) noexcept {
        auto r = s % numeric_limits<T>::digits;
        if (r == 0) return x;
        else if (r > 0) return (x >> r) | (x << (numeric_limits<T>::digits - r));
        else return (x << -r) | (x >> (numeric_limits<T>::digits + r));
    }

    /* 26.5.7 Endian */
    enum class endian {
        little = __ORDER_LITTLE_ENDIAN__,
        big = __ORDER_BIG_ENDIAN__,
        native = __BYTE_ORDER__,
    };
}