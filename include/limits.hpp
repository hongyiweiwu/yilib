#pragma once

#include "type_traits.hpp"
#include "climits.hpp"
#include "cfloat.hpp"
#include "util/macros.hpp"

// TODO: Temporary include. Later replace with "cmath.hpp".
#include "math.h"

namespace std {
    /* 17.3.4 Floating-point type properties */
    enum float_round_style {
        round_indeterminate = -1,
        round_toward_zero = 0,
        round_to_nearest = 1,
        round_toward_infinity = 2,
        round_toward_neg_infinity = 3
    };

    enum float_denorm_style {
        denorm_indeterminate = -1,
        denorm_absent = 0,
        denorm_present = 1
    };

    /* 17.3.5 Class template numeric_limits */
    namespace __internal {
        template<class T> requires is_integral_v<T>
        struct __specialized_integral_numeric_limits {
            static constexpr bool is_specialized = true;
            static constexpr int digits = sizeof(T) * CHAR_BIT - is_signed_v<T>;
            static constexpr int digits10 = digits * 643L / 2136;    // 643L / 2136 is log10(2) approximated to 7 significant digits.
            static constexpr int max_digits10 = 0;
            static constexpr T min() noexcept { return is_signed_v<T> ? -max() - 1 : T(0); }
            static constexpr T max() noexcept { return (((T(1) << (digits - 1)) - 1) << 1) + 1; }
            static constexpr T lowest() noexcept { return min(); }

            static constexpr bool is_signed = is_signed_v<T>;
            static constexpr bool is_integer = true;
            static constexpr bool is_exact = true;
            static constexpr int radix = 2;
            static constexpr T epsilon() noexcept { return 0; }
            static constexpr T round_error() noexcept { return 0; }

            static constexpr int min_exponent = 0;
            static constexpr int min_exponent10 = 0;
            static constexpr int max_exponent = 0;
            static constexpr int max_exponent10 = 0;

            static constexpr bool has_infinity = false;
            static constexpr bool has_quiet_NaN = false;
            static constexpr bool has_signaling_NaN = false;
            static constexpr float_denorm_style has_denorm = denorm_absent;
            static constexpr bool has_denorm_loss = false;
            static constexpr T infinity() noexcept { return 0; }
            static constexpr T quiet_NaN() noexcept { return 0; }
            static constexpr T signaling_NaN() noexcept { return 0; }
            static constexpr T denorm_min() noexcept { return 0; }

            static constexpr bool is_iec559 = false;
            static constexpr bool is_bounded = true;
            static constexpr bool is_modulo = !is_signed;

            static constexpr bool traps = true;
            static constexpr bool tinyness_before = false;
            static constexpr float_round_style round_style = round_toward_zero;
        };
    }

    template<class T>
    class numeric_limits {
    public:
        static constexpr bool is_specialized = false;
        static constexpr T min() noexcept { return T(); }
        static constexpr T max() noexcept { return T(); }
        static constexpr T lowest() noexcept { return T(); }
        static constexpr int digits = 0;
        static constexpr int digits10 = 0;
        static constexpr int max_digits10 = 0;
        static constexpr bool is_signed = false;
        static constexpr bool is_integer = false;
        static constexpr bool is_exact = false;
        static constexpr int radix = 0;
        static constexpr T epsilon() noexcept { return T(); }
        static constexpr T round_error() noexcept { return T(); }
        static constexpr int min_exponent = 0;
        static constexpr int min_exponent10 = 0;
        static constexpr int max_exponent = 0;
        static constexpr int max_exponent10 = 0;
        static constexpr bool has_infinity = false;
        static constexpr bool has_quiet_NaN = false;
        static constexpr bool has_signaling_NaN = false;
        static constexpr float_denorm_style has_denorm = denorm_absent;
        static constexpr bool has_denorm_loss = false;
        static constexpr T infinity() noexcept { return T(); }
        static constexpr T quiet_NaN() noexcept { return T(); }
        static constexpr T signaling_NaN() noexcept { return T(); }
        static constexpr T denorm_min() noexcept { return T(); }
        static constexpr bool is_iec559 = false;
        static constexpr bool is_bounded = false;
        static constexpr bool is_modulo = false;
        static constexpr bool traps = false;
        static constexpr bool tinyness_before = false;
        static constexpr float_round_style round_style = round_toward_zero;
    };

    template<>
    class numeric_limits<bool> {
    public:
        static constexpr bool is_specialized = true;
        static constexpr bool min() noexcept { return false; }
        static constexpr bool max() noexcept { return true; }
        static constexpr bool lowest() noexcept { return false; }
        static constexpr int digits = 1;
        static constexpr int digits10 = 0;
        static constexpr int max_digits10 = 0;
        static constexpr bool is_signed = false;
        static constexpr bool is_integer = true;
        static constexpr bool is_exact = true;
        static constexpr int radix = 2;
        static constexpr bool epsilon() noexcept { return 0; }
        static constexpr bool round_error() noexcept { return 0; }
        static constexpr int min_exponent = 0;
        static constexpr int min_exponent10 = 0;
        static constexpr int max_exponent = 0;
        static constexpr int max_exponent10 = 0;
        static constexpr bool has_infinity = false;
        static constexpr bool has_quiet_NaN = false;
        static constexpr bool has_signaling_NaN = false;
        static constexpr float_denorm_style has_denorm = denorm_absent;
        static constexpr bool has_denorm_loss = false;
        static constexpr bool infinity() noexcept { return 0; }
        static constexpr bool quiet_NaN() noexcept { return 0; }
        static constexpr bool signaling_NaN() noexcept { return 0; }
        static constexpr bool denorm_min() noexcept { return 0; }
        static constexpr bool is_iec559 = false;
        static constexpr bool is_bounded = true;
        static constexpr bool is_modulo = false;
        static constexpr bool traps = false;
        static constexpr bool tinyness_before = false;
        static constexpr float_round_style round_style = round_toward_zero;
    };

    template<> class numeric_limits<char> : public __internal::__specialized_integral_numeric_limits<char> {};
    template<> class numeric_limits<signed char> : public __internal::__specialized_integral_numeric_limits<signed char> {};
    template<> class numeric_limits<unsigned char> : public __internal::__specialized_integral_numeric_limits<unsigned char> {};
    template<> class numeric_limits<wchar_t> : public __internal::__specialized_integral_numeric_limits<wchar_t> {};
    template<> class numeric_limits<char8_t> : public __internal::__specialized_integral_numeric_limits<char8_t> {};
    template<> class numeric_limits<char16_t> : public __internal::__specialized_integral_numeric_limits<char16_t> {};
    template<> class numeric_limits<char32_t> : public __internal::__specialized_integral_numeric_limits<char32_t> {};
    template<> class numeric_limits<short> : public __internal::__specialized_integral_numeric_limits<short> {};
    template<> class numeric_limits<unsigned short> : public __internal::__specialized_integral_numeric_limits<unsigned short> {};
    template<> class numeric_limits<int> : public __internal::__specialized_integral_numeric_limits<int> {};
    template<> class numeric_limits<unsigned int> : public __internal::__specialized_integral_numeric_limits<unsigned int> {};
    template<> class numeric_limits<long> : public __internal::__specialized_integral_numeric_limits<long> {};
    template<> class numeric_limits<unsigned long> : public __internal::__specialized_integral_numeric_limits<unsigned long> {};
    template<> class numeric_limits<long long> : public __internal::__specialized_integral_numeric_limits<long long> {};
    template<> class numeric_limits<unsigned long long> : public __internal::__specialized_integral_numeric_limits<unsigned long long> {};

    template<> class numeric_limits<float> {
    public:
        static constexpr bool is_specialized = true;
        static constexpr float min() noexcept { return FLT_MIN; }
        static constexpr float max() noexcept { return FLT_MAX; }
        static constexpr float lowest() noexcept { return -FLT_MAX; }

        static constexpr int digits = FLT_MANT_DIG;
        static constexpr int digits10 = FLT_DIG;
        static constexpr int max_digits10 = FLT_DECIMAL_DIG;

        static constexpr bool is_signed = true;
        static constexpr bool is_integer = false;
        static constexpr bool is_exact = false;

        static constexpr int radix = FLT_RADIX;
        static constexpr float epsilon() noexcept { return FLT_EPSILON; }
        static constexpr float round_error() noexcept { return 0.5F; }

        static constexpr float min_exponent = FLT_MIN_EXP;
        static constexpr float min_exponent10 = FLT_MIN_10_EXP;
        static constexpr float max_exponent = FLT_MAX_EXP;
        static constexpr float max_exponent10 = FLT_MAX_10_EXP;

        static constexpr bool has_infinity = true;
        static constexpr bool has_quiet_NaN = true;
        static constexpr bool has_signaling_NaN = true;
        static constexpr float_denorm_style has_denorm = denorm_present;
        static constexpr bool has_denorm_loss = false;

        static constexpr float infinity() noexcept { return HUGE_VALF; }
#if __has_intrinsics_for(builtin_nanf)
        static constexpr float quiet_NaN() noexcept { return __builtin_nanf(""); }
#endif
#if __has_intrinsics_for(builtin_nansf)
        static constexpr float signaling_NaN() noexcept { return __builtin_nansf(""); }
#endif

#ifdef __FLT_DENORM_MIN__
        static constexpr float denorm_min() noexcept { return __FLT_DENORM_MIN__; }
#endif

        static constexpr bool is_iec559 = true;
        static constexpr bool is_bounded = true;
        static constexpr bool is_modulo = false;
        static constexpr bool traps = false;
        static constexpr bool tinyness_before = false;

        static constexpr float_round_style round_style = round_to_nearest;
    };

    template<> class numeric_limits<double> {
    public:
        static constexpr bool is_specialized = true;
        static constexpr double min() noexcept { return DBL_MIN; }
        static constexpr double max() noexcept { return DBL_MAX; }
        static constexpr double lowest() noexcept { return -DBL_MAX; }

        static constexpr int digits = DBL_MANT_DIG;
        static constexpr int digits10 = DBL_DIG;
        static constexpr int max_digits10 = DBL_DECIMAL_DIG;

        static constexpr bool is_signed = true;
        static constexpr bool is_integer = false;
        static constexpr bool is_exact = false;

        static constexpr int radix = FLT_RADIX;
        static constexpr double epsilon() noexcept { return DBL_EPSILON; }
        static constexpr double round_error() noexcept { return 0.5; }

        static constexpr double min_exponent = DBL_MIN_EXP;
        static constexpr double min_exponent10 = DBL_MIN_10_EXP;
        static constexpr double max_exponent = DBL_MAX_EXP;
        static constexpr double max_exponent10 = DBL_MAX_10_EXP;

        static constexpr bool has_infinity = true;
        static constexpr bool has_quiet_NaN = true;
        static constexpr bool has_signaling_NaN = true;
        static constexpr float_denorm_style has_denorm = denorm_present;
        static constexpr bool has_denorm_loss = false;

        static constexpr double infinity() noexcept { return HUGE_VAL; }
#if __has_intrinsics_for(builtin_nan)
        static constexpr double quiet_NaN() noexcept { return __builtin_nan(""); }
#endif
#if __has_intrinsics_for(builtin_nans)
        static constexpr double signaling_NaN() noexcept { return __builtin_nans(""); }
#endif

#ifdef __DBL_DENORM_MIN__
        static constexpr double denorm_min() noexcept { return __DBL_DENORM_MIN__; }
#endif

        static constexpr bool is_iec559 = true;
        static constexpr bool is_bounded = true;
        static constexpr bool is_modulo = false;
        static constexpr bool traps = false;
        static constexpr bool tinyness_before = false;

        static constexpr float_round_style round_style = round_to_nearest;
    };

    template<> class numeric_limits<long double> {
    public:
        static constexpr bool is_specialized = true;
        static constexpr long double min() noexcept { return LDBL_MIN; }
        static constexpr long double max() noexcept { return LDBL_MAX; }
        static constexpr long double lowest() noexcept { return -LDBL_MAX; }

        static constexpr int digits = LDBL_MANT_DIG;
        static constexpr int digits10 = LDBL_DIG;
        static constexpr int max_digits10 = LDBL_DECIMAL_DIG;

        static constexpr bool is_signed = true;
        static constexpr bool is_integer = false;
        static constexpr bool is_exact = false;

        static constexpr int radix = FLT_RADIX;
        static constexpr long double epsilon() noexcept { return LDBL_EPSILON; }
        static constexpr long double round_error() noexcept { return 0.5L; }

        static constexpr long double min_exponent = LDBL_MIN_EXP;
        static constexpr long double min_exponent10 = LDBL_MIN_10_EXP;
        static constexpr long double max_exponent = LDBL_MAX_EXP;
        static constexpr long double max_exponent10 = LDBL_MAX_10_EXP;

        static constexpr bool has_infinity = true;
        static constexpr bool has_quiet_NaN = true;
        static constexpr bool has_signaling_NaN = true;
        static constexpr float_denorm_style has_denorm = denorm_present;
        static constexpr bool has_denorm_loss = false;

        static constexpr long double infinity() noexcept { return HUGE_VALL; }
#if __has_intrinsics_for(builtin_nanl)
        static constexpr double quiet_NaN() noexcept { return __builtin_nanl(""); }
#endif
#if __has_intrinsics_for(builtin_nansl)
        static constexpr double signaling_NaN() noexcept { return __builtin_nansl(""); }
#endif

#ifdef __LDBL_DENORM_MIN__
        static constexpr double denorm_min() noexcept { return __LDBL_DENORM_MIN__; }
#endif

        static constexpr bool is_iec559 = true;
        static constexpr bool is_bounded = true;
        static constexpr bool is_modulo = false;
        static constexpr bool traps = false;
        static constexpr bool tinyness_before = false;

        static constexpr float_round_style round_style = round_to_nearest;
    };

    template<class T>
    class numeric_limits<const T> : public numeric_limits<T> {};
    template<class T>
    class numeric_limits<volatile T> : public numeric_limits<T> {};
    template<class T>
    class numeric_limits<const volatile T> : public numeric_limits<T> {};
}