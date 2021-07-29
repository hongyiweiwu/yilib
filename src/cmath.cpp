#include "cmath.hpp"
#include "cstdlib.hpp"

namespace std {
    float acos(float x) {
        return std::acosf(x);
    }

    long double acos(long double x) {
        return std::acosl(x);
    }

    float asin(float x) {
        return std::asinf(x);
    }

    long double asin(long double x) {
        return std::asinl(x);
    }

    float atan(float x) {
        return std::atanf(x);
    }

    long double atan(long double x) {
        return std::atanl(x);
    }

    float atan2(float y, float x) {
        return std::atan2f(y, x);
    }

    long double atan2(long double y, long double x) {
        return std::atan2l(y, x);
    }

    float cos(float x) {
        return std::cosf(x);
    }

    long double cos(long double x) {
        return std::cosl(x);
    }

    float sin(float x) {
        return std::sinf(x);
    }

    long double sin(long double x) {
        return std::sinl(x);
    }

    float tan(float x) {
        return std::tanf(x);
    }

    long double tan(long double x) {
        return std::tanl(x);
    }

    float acosh(float x) {
        return std::acoshf(x);
    }

    long double acosh(long double x) {
        return std::acoshl(x);
    }

    float asinh(float x) {
        return std::asinhf(x);
    }

    long double asinh(long double x) {
        return std::asinhl(x);
    }

    float atanh(float x) {
        return std::atanhf(x);
    }

    long double atanh(long double x) {
        return std::atanhl(x);
    }

    float cosh(float x) {
        return std::coshf(x);
    }

    long double cosh(long double x) {
        return std::coshl(x);
    }

    float sinh(float x) {
        return std::sinhf(x);
    }

    long double sinh(long double x) {
        return std::sinhl(x);
    }

    float tanh(float x) {
        return std::tanhf(x);
    }

    long double tanh(long double x) {
        return std::tanhl(x);
    }

    float exp(float x) {
        return std::expf(x);
    }

    long double exp(long double x) {
        return std::expl(x);
    }

    float exp2(float x) {
        return std::exp2f(x);
    }

    long double exp2(long double x) {
        return std::exp2l(x);
    }

    float expm1(float x) {
        return std::expm1f(x);
    }

    long double expm1(long double x) {
        return std::expm1l(x);
    }

    float frexp(float x, int* exp) {
        return std::frexp(x, exp);
    }

    long double frexp(long double x, int* exp) {
        return std::frexp(x, exp);
    }

    int ilogb(float x) {
        return std::ilogbf(x);
    }

    int ilogb(long double x) {
        return std::ilogbl(x);
    }

    float ldexp(float x, int exp) {
        return std::ldexp(x, exp);
    }

    long double ldexp(long double x, int exp) {
        return std::ldexp(x, exp);
    }

    float log(float x) {
        return std::logf(x);
    }

    long double log(long double x) {
        return std::logl(x);
    }

    float log10(float x) {
        return std::log10f(x);
    }

    long double log10(long double x) {
        return std::log10l(x);
    }

    float log1p(float x) {
        return std::log1pf(x);
    }

    long double log1p(long double x) {
        return std::log1pl(x);
    }

    float log2(float x) {
        return std::log2f(x);
    }

    long double log2(long double x) {
        return std::log2l(x);
    }

    float logb(float x) {
        return std::logbf(x);
    }

    long double logb(long double x) {
        return std::logbl(x);
    }

    float modf(float x, float* iptr) {
        return std::modff(x, iptr);
    }

    long double modf(long double x, long double* iptr) {
        return std::modfl(x, iptr);
    }

    float scalbn(float x, int n) {
        return std::scalbnf(x, n);
    }

    long double scalbn(long double x, int n) {
        return std::scalbnl(x, n);
    }

    float scalbln(float x, int n) {
        return std::scalblnf(x, n);
    }

    long double scalbln(long double x, long int n) {
        return std::scalblnl(x, n);
    }

    float cbrt(float x) {
        return std::cbrtf(x);
    }

    long double cbrt(long double x) {
        return std::cbrtl(x);
    }

    long int abs(long int j) {
        return std::labs(j);
    }

    long long int abs(long long int j) {
        return std::llabs(j);
    }

    float abs(float j) {
        return std::fabsf(j);
    }

    double abs(double j) {
        return std::fabs(j);
    }

    long double abs(long double j) {
        return std::fabsl(j);
    }

    float fabs(float x) {
        return std::fabsf(x);
    }

    long double fabs(long double x) {
        return std::fabsl(x);
    }

    float hypot(float x, float y) {
        return std::hypotf(x, y);
    }

    long double hypot(long double x, long double y) {
        return std::hypotl(x, y);
    }

    float hypot(float x, float y, float z) {
        return hypot(hypot(x, y), z);
    }

    double hypot(double x, double y, double z) {
        return hypot(hypot(x, y), z);
    }

    long double hypot(long double x, long double y, long double z) {
        return hypot(hypot(x, y), z);
    }

    float pow(float x, float y) {
        return std::powf(x, y);
    }

    long double pow(long double x, long double y) {
        return std::powl(x, y);
    }

    float sqrt(float x) {
        return std::sqrtf(x);
    }

    long double sqrt(long double x) {
        return std::sqrtl(x);
    }

    float erf(float x) {
        return std::erff(x);
    }

    long double erf(long double x) {
        return std::erfl(x);
    }

    float erfc(float x) {
        return std::erfcf(x);
    }

    long double erfc(long double x) {
        return std::erfcl(x);
    }

    float lgamma(float x) {
        return std::lgammaf(x);
    }

    long double lgamma(long double x) {
        return std::lgammal(x);
    }

    float tgamma(float x) {
        return std::tgammaf(x);
    }

    long double tgamma(long double x) {
        return std::tgammal(x);
    }

    float ceil(float x) {
        return std::ceilf(x);
    }

    long double ceil(long double x) {
        return std::ceill(x);
    }

    float floor(float x) {
        return std::floorf(x);
    }

    long double floor(long double x) {
        return std::floorl(x);
    }

    float nearbyint(float x) {
        return std::nearbyintf(x);
    }

    long double nearbyint(long double x) {
        return std::nearbyintl(x);
    }

    float rint(float x) {
        return std::rintf(x);
    }

    long double rint(long double x) {
        return std::rintl(x);
    }

    long int lrint(float x) {
        return std::lrintf(x);
    }

    long int lrint(long double x) {
        return std::lrintl(x);
    }

    long long int llrint(float x) {
        return std::llrintf(x);
    }

    long long int llrint(long double x) {
        return std::llrintl(x);
    }

    float round(float x) {
        return std::roundf(x);
    }

    long double round(long double x) {
        return std::roundl(x);
    }

    long int lround(float x) {
        return std::lroundf(x);
    }

    long int lround(long double x) {
        return std::lroundl(x);
    }

    long long int llround(float x) {
        return std::llroundf(x);
    }

    long long int llround(long double x) {
        return std::llroundl(x);
    }

    float trunc(float x) {
        return std::truncf(x);
    }

    long double trunc(long double x) {
        return std::truncl(x);
    }

    float fmod(float x, float y) {
        return std::fmodf(x, y);
    }

    long double fmod(long double x, long double y) {
        return std::fmodl(x, y);
    }

    float remainder(float x, float y) {
        return std::remainderf(x, y);
    }

    long double remainder(long double x, long double y) {
        return std::remainderl(x, y);
    }

    float remquo(float x, float y, int* quo) {
        return std::remquof(x, y, quo);
    }

    long double remquo(long double x, long double y, int* quo) {
        return std::remquol(x, y, quo);
    }

    float copysign(float x, float y) {
        return std::copysignf(x, y);
    }

    long double copysign(long double x, long double y) {
        return std::copysignl(x, y);
    }

    float nextafter(float x, float y) {
        return std::nextafterf(x, y);
    }

    long double nextafter(long double x, long double y) {
        return std::nextafterl(x, y);
    }

    float nexttoward(float x, long double y) {
        return std::nexttowardf(x, y);
    }

    long double nexttoward(long double x, long double y) {
        return std::nexttowardl(x, y);
    }

    float fdim(float x, float y) {
        return std::fdimf(x, y);
    }

    long double fdim(long double x, long double y) {
        return std::fdiml(x, y);
    }

    float fmax(float x, float y) {
        return std::fmaxf(x, y);
    }

    long double fmax(long double x, long double y) {
        return std::fmaxl(x, y);
    }

    float fmin(float x, float y) {
        return std::fminf(x, y);
    }

    long double fmin(long double x, long double y) {
        return std::fminl(x, y);
    }

    float fma(float x, float y, float z) {
        return std::fmaf(x, y, z);
    }

    long double fma(long double x, long double y, long double z) {
        return std::fmal(x, y, z);
    }

    int fpclassify(float x) {
        return __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x);
    }
    int fpclassify(double x) {
        return __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x);
    }
    int fpclassify(long double x) {
        return __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x);
    }

    bool isfinite(float x) {
        return __builtin_isfinite(x);
    }

    bool isfinite(double x) {
        return __builtin_isfinite(x);
    }

    bool isfinite(long double x) {
        return __builtin_isfinite(x);
    }

    bool isinf(float x) {
        return __builtin_isinf(x);
    }

    bool isinf(double x) {
        return __builtin_isinf(x);
    }

    bool isinf(long double x) {
        return __builtin_isinf(x);
    }

    bool isnan(float x) {
        return __builtin_isnan(x);
    }

    bool isnan(double x) {
        return __builtin_isnan(x);
    }

    bool isnan(long double x) {
        return __builtin_isnan(x);
    }

    bool isnormal(float x) {
        return __builtin_isnormal(x);
    }

    bool isnormal(double x) {
        return __builtin_isnormal(x);
    }

    bool isnormal(long double x) {
        return __builtin_isnormal(x);
    }

    bool signbit(float x) {
        return __builtin_signbit(x);
    }

    bool signbit(double x) {
        return __builtin_signbit(x);
    }

    bool signbit(long double x) {
        return __builtin_signbit(x);
    }

    bool isgreater(float x, float y) {
        return __builtin_isgreater(x, y);
    }

    bool isgreater(double x, double y) {
        return __builtin_isgreater(x, y);
    }

    bool isgreater(long double x, long double y) {
        return __builtin_isgreater(x, y);
    }
    
    bool isgreaterequal(float x, float y) {
        return __builtin_isgreaterequal(x, y);
    }

    bool isgreaterequal(double x, double y) {
        return __builtin_isgreaterequal(x, y);
    }

    bool isgreaterequal(long double x, long double y) {
        return __builtin_isgreaterequal(x, y);
    }
    
    bool isless(float x, float y) {
        return __builtin_isless(x, y);
    }

    bool isless(double x, double y) {
        return __builtin_isless(x, y);
    }

    bool isless(long double x, long double y) {
        return __builtin_isless(x, y);
    }

    bool islessequal(float x, float y) {
        return __builtin_islessequal(x, y);
    }

    bool islessequal(double x, double y) {
        return __builtin_islessequal(x, y);
    }

    bool islessequal(long double x, long double y) {
        return __builtin_islessequal(x, y);
    }
    
    bool islessgreater(float x, float y) {
        return __builtin_islessgreater(x, y);
    }

    bool islessgreater(double x, double y) {
        return __builtin_islessgreater(x, y);
    }

    bool islessgreater(long double x, long double y) {
        return __builtin_islessgreater(x, y);
    }
    
    bool isunordered(float x, float y) {
        return __builtin_isunordered(x, y);
    }
    
    bool isunordered(double x, double y) {
        return __builtin_isunordered(x, y);
    }
    
    bool isunordered(long double x, long double y) {
        return __builtin_isunordered(x, y);
    }
    
}