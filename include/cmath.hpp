#pragma once

#include "math.h"
#include "stdlib.h"

#include "concepts.hpp"

#undef fpclassify
#undef isfinite
#undef isinf
#undef isnan
#undef isnormal
#undef signbit
#undef isgreater
#undef isgreaterequal
#undef isless
#undef islessequal
#undef islessgreater
#undef isunordered

// TODO: Implement special mathematics functions.

namespace std {
    using ::float_t;
    using ::double_t;

    float acos(float x);
    using ::acos;
    long double acos(long double x);
    using ::acosf;
    using ::acosl;

    template<integral T>
    double acos(T x) {
        return std::acos(static_cast<double>(x));
    }

    float asin(float x);
    using ::asin;
    long double asin(long double x);
    using ::asinf;
    using ::asinl;

    template<integral T>
    double asin(T x) {
        return std::asin(static_cast<double>(x));
    }

    float atan(float x);
    using ::atan;
    long double atan(long double x);
    using ::atanf;
    using ::atanl;

    template<integral T>
    double atan(T x) {
        return std::atan(static_cast<double>(x));
    }

    float atan2(float y, float x);
    using ::atan2;
    long double atan2(long double y, long double x);
    using ::atan2f;
    using ::atan2l;

    template<class T1, class T2> 
    requires is_arithmetic_v<T1> && is_arithmetic_v<T2>
    auto atan2(T1 y, T2 x) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double>) {
            return std::atan2(static_cast<long double>(y), static_cast<long double>(x));
        } else {
            return std::atan2(static_cast<double>(y), static_cast<double>(x));
        }
    }

    float cos(float x);
    using ::cos;
    long double cos(long double x);
    using ::cosf;
    using ::cosl;

    template<integral T>
    double cos(T x) {
        return std::cos(static_cast<double>(x));
    }

    float sin(float x);
    using ::sin;
    long double sin(long double x);
    using ::sinf;
    using ::sinl;

    template<integral T>
    double sin(T x) {
        return std::sin(static_cast<double>(x));
    }

    float tan(float x);
    using ::tan;
    long double tan(long double x);
    using ::tanf;
    using ::tanl;

    template<integral T>
    double tan(T x) {
        return std::tan(static_cast<double>(x));
    }

    float acosh(float x);
    using ::acosh;
    long double acosh(long double x);
    using ::acoshf;
    using ::acoshl;

    template<integral T>
    double acosh(T x) {
        return std::acosh(static_cast<double>(x));
    }

    float asinh(float x);
    using ::asinh;
    long double asinh(long double x);
    using ::asinhf;
    using ::asinhl;

    template<integral T>
    double asinh(T x) {
        return std::asinh(static_cast<double>(x));
    }

    float atanh(float x);
    using ::atanh;
    long double atanh(long double x);
    using ::atanhf;
    using ::atanhl;

    template<integral T>
    double atanh(T x) {
        return std::atanh(static_cast<double>(x));
    }

    float cosh(float x);
    using ::cosh;
    long double cosh(long double x);
    using ::coshf;
    using ::coshl;

    template<integral T>
    double cosh(T x) {
        return std::cosh(static_cast<double>(x));
    }

    float sinh(float x);
    using ::sinh;
    long double sinh(long double x);
    using ::sinhf;
    using ::sinhl;

    template<integral T>
    double sinh(T x) {
        return std::sinh(static_cast<double>(x));
    }

    float tanh(float x);
    using ::tanh;
    long double tanh(long double x);
    using ::tanhf;
    using ::tanhl;

    template<integral T>
    double tanh(T x) {
        return std::tanh(static_cast<double>(x));
    }

    float exp(float x);
    using ::exp;
    long double exp(long double x);
    using ::expf;
    using ::expl;

    template<integral T>
    double exp(T x) {
        return std::exp(static_cast<double>(x));
    }

    float exp2(float x);
    using ::exp2;
    long double exp2(long double x);
    using ::exp2f;
    using ::exp2l;

    template<integral T>
    double exp2(T x) {
        return std::exp2(static_cast<double>(x));
    }

    float expm1(float x);
    using ::expm1;
    long double exm1p(long double x);
    using ::expm1f;
    using ::expm1l;

    template<integral T>
    double expm1(T x) {
        return std::expm1(static_cast<double>(x));
    }

    float frexp(float x, int* exp);
    using ::frexp;
    long double frexp(long double x, int* exp);
    using ::frexpf;
    using ::frexpl;

    template<integral T>
    double frexp(T x, int* exp) {
        return std::frexp(static_cast<double>(x), exp);
    }

    int ilogb(float x);
    using ::ilogb;
    int ilogb(long double x);
    using ::ilogbf;
    using ::ilogbl;

    template<integral T>
    double ilogb(T x) {
        return std::ilogb(static_cast<double>(x));
    }

    float ldexp(float x, int exp);
    using ::ldexp;
    long double ldexp(long double x, int exp);
    using ::ldexpf;
    using ::ldexpl;

    template<integral T>
    double ldexp(T x, int exp) {
        return std::ldexp(static_cast<double>(x), exp);
    }

    float log(float x);
    using ::log;
    long double log(long double x);
    using ::logf;
    using ::logl;

    template<integral T>
    double log(T x) {
        return std::log(static_cast<double>(x));
    }

    float log10(float x);
    using ::log10;
    long double log10(long double x);
    using ::log10f;
    using ::log10l;

    template<integral T>
    double log10(T x) {
        return std::log10(static_cast<double>(x));
    }

    float log1p(float x);
    using ::log1p;
    long double log1p(long double x);
    using ::log1pf;
    using ::log1pl;

    template<integral T>
    double log1p(T x) {
        return std::log1p(static_cast<double>(x));
    }

    float log2(float x);
    using ::log2;
    long double log2(long double x);
    using ::log2f;
    using ::log2l;

    template<integral T>
    double log2(T x) {
        return std::log2(static_cast<double>(x));
    }

    float logb(float x);
    using ::logb;
    long double logb(long double x);
    using ::logbf;
    using ::logbl;

    template<integral T>
    double logb(T x) {
        return std::logb(static_cast<double>(x));
    }

    float modf(float x, float* iptr);
    using ::modf;
    long double modf(long double x, long double* iptr);
    using ::modff;
    using ::modfl;

    float scalbn(float x, int n);
    using ::scalbn;
    long double scalbn(long double x, int n);
    using ::scalbnf;
    using ::scalbnl;

    template<integral T>
    double scalbn(T x, int n) {
        return std::scalbn(static_cast<double>(x), n);
    }

    float scalbln(float x, long int n);
    using ::scalbln;
    long double scalbln(long double x, long int n);
    using ::scalblnf;
    using ::scalblnl;

    template<integral T>
    double scalbln(T x, long int n) {
        return std::scalbln(static_cast<double>(x), n);
    }

    float cbrt(float x);
    using ::cbrt;
    long double cbrt(long double x);
    using ::cbrtf;
    using ::cbrtl;

    template<integral T>
    double cbrt(T x) {
        return std::cbrt(static_cast<double>(x));
    }

    /* 26.8.2 Absolute values */
    using ::abs;       // Implemented in cstdlib.hpp.
    long int abs(long int j);
    long long int abs(long long int j);
    float abs(float j);
    double abs(double j);
    long double abs(long double j);

    float fabs(float x);
    using ::fabs;
    long double fabs(long double x);
    using ::fabsf;
    using ::fabsl;

    template<integral T>
    double fabs(T x) {
        return std::fabs(static_cast<double>(x));
    }

    float hypot(float x, float y);
    using ::hypot;
    long double hypot(long double x, long double y);
    using ::hypotf;
    using ::hypotl;

    template<class T1, class T2>
    auto hypot(T1 x, T2 y) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double>) {
            return std::hypot(static_cast<long double>(x), static_cast<long double>(y));
        } else {
            return std::hypot(static_cast<double>(x), static_cast<double>(y));
        }
    }

    /* 26.8.3 Three-dimensional hypotenuse */
    float hypot(float x, float y, float z);
    double hypot(double x, double y, double z);
    long double hypot(long double x, long double y, long double z);

    template<class T1, class T2, class T3>
    auto hypot(T1 x, T2 y, T3 z) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double> || is_same_v<T3, long double>) {
            return std::hypot(static_cast<long double>(x), static_cast<long double>(y), static_cast<long double>(z));
        } else {
            return std::hypot(static_cast<double>(x), static_cast<double>(y), static_cast<double>(z));
        }
    }

    float pow(float x, float y);
    using ::pow;
    long double pow(long double x, long double y);
    using ::powf;
    using ::powl;

    template<class T1, class T2>
    auto pow(T1 x, T2 y) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double>) {
            return std::pow(static_cast<long double>(x), static_cast<long double>(y));
        } else {
            return std::pow(static_cast<double>(x), static_cast<double>(y));
        }
    }

    float sqrt(float x);
    using ::sqrt;
    long double sqrt(long double x);
    using ::sqrtf;
    using ::sqrtl;

    template<integral T>
    double sqrt(T x) {
        return std::sqrt(static_cast<double>(x));
    }

    float erf(float x);
    using ::erf;
    long double erf(long double x);
    using ::erff;
    using ::erfl;

    template<integral T>
    double erf(T x) {
        return std::erf(static_cast<double>(x));
    }

    float erfc(float x);
    using ::erfc;
    long double erfc(long double x);
    using ::erfcf;
    using ::erfcl;

    template<integral T>
    double erfc(T x) {
        return std::erfc(static_cast<double>(x));
    }

    float lgamma(float x);
    using ::lgamma;
    long double lgamma(long double x);
    using ::lgammaf;
    using ::lgammal;

    template<integral T>
    double lgamma(T x) {
        return std::lgamma(static_cast<double>(x));
    }

    float tgamma(float x);
    using ::tgamma;
    long double tgamma(long double x);
    using ::tgammaf;
    using ::tgammal;

    template<integral T>
    double tgamma(T x) {
        return std::tgamma(static_cast<double>(x));
    }

    float ceil(float x);
    using ::ceil;
    long double ceil(long double x);
    using ::ceilf;
    using ::ceill;

    template<integral T>
    double ceil(T x) {
        return std::ceil(static_cast<double>(x));
    }

    float floor(float x);
    using ::floor;
    long double floor(long double x);
    using ::floorf;
    using ::floorl;

    template<integral T>
    double floor(T x) {
        return std::floor(static_cast<double>(x));
    }

    float nearbyint(float x);
    using ::nearbyint;
    long double nearbyint(long double x);
    using ::nearbyintf;
    using ::nearbyintl;

    template<integral T>
    double nearbyint(T x) {
        return std::nearbyint(static_cast<double>(x));
    }

    float rint(float x);
    using ::rint;
    long double rint(long double x);
    using ::rintf;
    using ::rintl;

    template<integral T>
    double rint(T x) {
        return std::rint(static_cast<double>(x));
    }

    long int lrint(float x);
    using ::lrint;
    long int lrint(long double x);
    using ::lrintf;
    using ::lrintl;

    template<integral T>
    double lrint(T x) {
        return std::lrint(static_cast<double>(x));
    }

    long long int llrint(float x);
    using ::llrint;
    long long int llrint(long double x);
    using ::llrintf;
    using ::llrintl;

    template<integral T>
    double llrint(T x) {
        return std::llrint(static_cast<double>(x));
    }

    float round(float x);
    using ::round;
    long double round(long double x);
    using ::roundf;
    using ::roundl;

    template<integral T>
    double round(T x) {
        return std::round(static_cast<double>(x));
    }

    long int lround(float x);
    using ::lround;
    long int lround(long double x);
    using ::lroundf;
    using ::lroundl;

    template<integral T>
    double lround(T x) {
        return std::lround(static_cast<double>(x));
    }

    long long int llround(float x);
    using ::llround;
    long long int llround(long double x);
    using ::llroundf;
    using ::llroundl;

    template<integral T>
    double llround(T x) {
        return std::llround(static_cast<double>(x));
    }

    float trunc(float x);
    using ::trunc;
    long double trunc(long double x);
    using ::truncf;
    using ::truncl;

    template<integral T>
    double trunc(T x) {
        return std::trunc(static_cast<double>(x));
    }

    float fmod(float x, float y);
    using ::fmod;
    long double fmod(long double x, long double y);
    using ::fmodf;
    using ::fmodl;

    template<class T1, class T2>
    auto fmod(T1 x, T2 y) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double>) {
            return std::fmod(static_cast<long double>(x), static_cast<long double>(y));
        } else {
            return std::fmod(static_cast<double>(x), static_cast<double>(y));
        }
    }

    float remainder(float x, float y);
    using ::remainder;
    long double remainder(long double x, long double y);
    using ::remainderf;
    using ::remainderl;

    template<class T1, class T2>
    auto remainder(T1 x, T2 y) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double>) {
            return std::remainder(static_cast<long double>(x), static_cast<long double>(y));
        } else {
            return std::remainder(static_cast<double>(x), static_cast<double>(y));
        }
    }

    float remquo(float x, float y, int* quo);
    using ::remquo;
    long double remquo(long double x, long double y, int* quo);
    using ::remquof;
    using ::remquol;

    template<class T1, class T2>
    auto remquo(T1 x, T2 y, int* quo) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double>) {
            return std::remquo(static_cast<long double>(x), static_cast<long double>(y), quo);
        } else {
            return std::remquo(static_cast<double>(x), static_cast<double>(y), quo);
        }
    }

    float copysign(float x, float y);
    using ::copysign;
    long double copysign(long double x, long double y);
    using ::copysignf;
    using ::copysignl;

    template<class T1, class T2>
    auto copysign(T1 x, T2 y) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double>) {
            return std::copysign(static_cast<long double>(x), static_cast<long double>(y));
        } else {
            return std::copysign(static_cast<double>(x), static_cast<double>(y));
        }
    }

    using ::nan;
    using ::nanf;
    using ::nanl;

    float nextafter(float x, float y);
    using ::nextafter;
    long double nextafter(long double x, long double y);
    using ::nextafterf;
    using ::nextafterl;

    template<class T1, class T2>
    auto nextafter(T1 x, T2 y) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double>) {
            return std::nextafter(static_cast<long double>(x), static_cast<long double>(y));
        } else {
            return std::nextafter(static_cast<double>(x), static_cast<double>(y));
        }
    }

    float nexttoward(float x, long double y);
    using ::nexttoward;
    long double nexttoward(long double x, long double y);
    using ::nexttowardf;
    using ::nexttowardl;

    template<integral T>
    double nexttoward(T x, long double y) {
        return std::nexttoward(x, y);
    }

    float fdim(float x, float y);
    using ::fdim;
    long double fdim(long double x, long double y);
    using ::fdimf;
    using ::fdiml;

    template<class T1, class T2>
    auto fdim(T1 x, T2 y) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double>) {
            return std::fdim(static_cast<long double>(x), static_cast<long double>(y));
        } else {
            return std::fdim(static_cast<double>(x), static_cast<double>(y));
        }
    }

    float fmax(float x, float y);
    using ::fmax;
    long double fmax(long double x, long double y);
    using ::fmaxf;
    using ::fmaxl;

    template<class T1, class T2>
    auto fmax(T1 x, T2 y) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double>) {
            return std::fmax(static_cast<long double>(x), static_cast<long double>(y));
        } else {
            return std::fmax(static_cast<double>(x), static_cast<double>(y));
        }
    }

    float fmin(float x, float y);
    using ::fmin;
    long double fmin(long double x, long double y);
    using ::fminf;
    using ::fminl;

    template<class T1, class T2>
    auto fmin(T1 x, T2 y) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double>) {
            return std::fmin(static_cast<long double>(x), static_cast<long double>(y));
        } else {
            return std::fmin(static_cast<double>(x), static_cast<double>(y));
        }
    }

    float fma(float x, float y, float z);
    using ::fma;
    long double fma(long double x, long double y, long double z);
    using ::fmaf;
    using ::fmal;

    template<class T1, class T2, class T3>
    auto fma(T1 x, T2 y, T3 z) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double> || is_same_v<T3, long double>) {
            return std::fma(static_cast<long double>(x), static_cast<long double>(y), static_cast<long double>(z));
        } else {
            return std::fma(static_cast<double>(x), static_cast<double>(y), static_cast<double>(z));
        }
    }

    /* 26.8.4 Linear interpolation */
    constexpr float lerp(float a, float b, float t) noexcept {
        if ((a <= 0 && b >= 0) || (a >= 0 && b <= 0)) {
            return t * b + (1 - t) * a;
        } else if (t == 1) {
            return b;
        } else {
            const float x = a + t * (b - a);
            return t > 1 == b > a ? std::fmax(b, x) : std::fmin(b, x);
        }
    }

    constexpr double lerp(double a, double b, double t) noexcept {
        if ((a <= 0 && b >= 0) || (a >= 0 && b <= 0)) {
            return t * b + (1 - t) * a;
        } else if (t == 1) {
            return b;
        } else {
            const double x = a + t * (b - a);
            return t > 1 == b > a ? std::fmax(b, x) : std::fmin(b, x);
        }
    }

    constexpr long double lerp(long double a, long double b, long double t) noexcept {
        if ((a <= 0 && b >= 0) || (a >= 0 && b <= 0)) {
            return t * b + (1 - t) * a;
        } else if (t == 1) {
            return b;
        } else {
            const long double x = a + t * (b - a);
            return t > 1 == b > a ? std::fmax(b, x) : std::fmin(b, x);
        }
    }

    /* 26.8.5 Classification / comparison functions */
    int fpclassify(float x);
    int fpclassify(double x);
    int fpclassify(long double x);

    bool isfinite(float x);
    bool isfinite(double x);
    bool isfinite(long double x);

    bool isinf(float x);
    bool isinf(double x);
    bool isinf(long double x);

    bool isnan(float x);
    bool isnan(double x);
    bool isnan(long double x);

    bool isnormal(float x);
    bool isnormal(double x);
    bool isnormal(long double x);

    bool signbit(float x);
    bool signbit(double x);
    bool signbit(long double x);

    bool isgreater(float x, float y);
    bool isgreater(double x, double y);
    bool isgreater(long double x, long double y);

    template<class T1, class T2>
    auto isgreater(T1 x, T2 y) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double>) {
            return std::isgreater(static_cast<long double>(x), static_cast<long double>(y));
        } else {
            return std::isgreater(static_cast<double>(x), static_cast<double>(y));
        }
    }

    bool isgreaterequal(float x, float y);
    bool isgreaterequal(double x, double y);
    bool isgreaterequal(long double x, long double y);

    template<class T1, class T2>
    auto isgreaterequal(T1 x, T2 y) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double>) {
            return std::isgreaterequal(static_cast<long double>(x), static_cast<long double>(y));
        } else {
            return std::isgreaterequal(static_cast<double>(x), static_cast<double>(y));
        }
    }

    bool isless(float x, float y);
    bool isless(double x, double y);
    bool isless(long double x, long double y);

    template<class T1, class T2>
    auto isless(T1 x, T2 y) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double>) {
            return std::isless(static_cast<long double>(x), static_cast<long double>(y));
        } else {
            return std::isless(static_cast<double>(x), static_cast<double>(y));
        }
    }

    bool islessequal(float x, float y);
    bool islessequal(double x, double y);
    bool islessequal(long double x, long double y);

    template<class T1, class T2>
    auto islessequal(T1 x, T2 y) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double>) {
            return std::islessequal(static_cast<long double>(x), static_cast<long double>(y));
        } else {
            return std::islessequal(static_cast<double>(x), static_cast<double>(y));
        }
    }

    bool islessgreater(float x, float y);
    bool islessgreater(double x, double y);
    bool islessgreater(long double x, long double y);

    template<class T1, class T2>
    auto islessgreater(T1 x, T2 y) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double>) {
            return std::islessgreater(static_cast<long double>(x), static_cast<long double>(y));
        } else {
            return std::islessgreater(static_cast<double>(x), static_cast<double>(y));
        }
    }

    bool isunordered(float x, float y);
    bool isunordered(double x, double y);
    bool isunordered(long double x, long double y);

    template<class T1, class T2>
    auto isunordered(T1 x, T2 y) {
        if constexpr (is_same_v<T1, long double> || is_same_v<T2, long double>) {
            return std::isunordered(static_cast<long double>(x), static_cast<long double>(y));
        } else {
            return std::isunordered(static_cast<double>(x), static_cast<double>(y));
        }
    }
}