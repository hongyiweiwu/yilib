#pragma once

#include "istream.hpp"
#include "ostream.hpp"
#include "cmath.hpp"
#include "type_traits.hpp"

#include "complex.h"
#undef complex

namespace std {
    /* 26.4.3 Class template complex */
    template<class T>
    class complex {
    private:
        T re;
        T im;
    public:
        using value_type = T;

        constexpr complex(const T& re = T(), const T& im = T()) : re(re), im(im) {}

        constexpr complex(const complex&) = default;

        template<class X>
        constexpr complex(const complex<X>& other) : re(other.re), im(other.im) {}

        constexpr T real() const {
            return re;
        }

        constexpr void real(T r) {
            re = r;
        }

        constexpr T imag() const {
            return im;
        }

        constexpr void imag(T i) {
            im = i;
        }

        constexpr complex& operator=(const T& other) {
            re = other;
            return *this;
        }

        constexpr complex& operator+=(const T& other) {
            re += other;
            return *this;
        }

        constexpr complex& operator-=(const T& other) {
            re -= other;
            return *this;
        }

        constexpr complex& operator*=(const T& other) {
            re *= other;
            return *this;
        }

        constexpr complex& operator/=(const T& other) {
            re /= other;
            return *this;
        }

        constexpr complex& operator=(const complex&) = default;

        template<class X>
        constexpr complex& operator=(const complex<X>& other) {
            re = other.re;
            im = other.im;
            return *this;
        }

        template<class X>
        constexpr complex& operator+=(const complex<X>& other) {
            re += other.re;
            im += other.im;
            return *this;
        }

        template<class X>
        constexpr complex& operator-=(const complex<X>& other) {
            re -= other.re;
            im -= other.im;
            return *this;
        }

        template<class X>
        constexpr complex& operator*=(const complex<X>& other) {
            const complex old = *this;
            re = old.real() * other.real() - old.imag() * other.imag();
            im = old.real() * other.imag() + old.imag() * other.real();
            return *this;
        }

        template<class X>
        constexpr complex& operator/=(const complex<X>& other) {
            const complex old = *this;
            const X modulus = other.real() * other.real() + other.imag() * other.imag();
            re = (old.real() * other.real() + old.imag() * other.imag()) / modulus;
            im = (old.imag() * other.real() - old.real() * other.imag()) / modulus;
            return *this;
        }
    };

    /* 26.4.4 Specializations */
    // Forward declaration, defined below.
    template<class T>
    constexpr T real(const complex<T>& cpl);

    template<class T>
    constexpr T imag(const complex<T>& cpl);

    template<>
    class complex<float> {
    private:
        float re;
        float im;
    public:
        using value_type = float;

        constexpr complex(float re = 0.0f, float im = 0.0f) : re(re), im(im) {}
        constexpr complex(const complex&) = default;
        constexpr explicit complex(const complex<double>& other) : re(static_cast<float>(std::real(other))), im(static_cast<float>(std::imag(other))) {}
        constexpr explicit complex(const complex<long double>& other) : re(static_cast<float>(std::real(other))), im(static_cast<float>(std::imag(other))) {}

        template<class X>
        constexpr complex(const complex<X>& other) : re(other.re), im(other.im) {}

        constexpr float real() const {
            return re;
        }

        constexpr void real(float r) {
            re = r;
        }

        constexpr float imag() const {
            return im;
        }

        constexpr void imag(float i) {
            im = i;
        }

        constexpr complex& operator=(float other) {
            re = other;
            return *this;
        }

        constexpr complex& operator+=(float other) {
            re += other;
            return *this;
        }

        constexpr complex& operator-=(float other) {
            re -= other;
            return *this;
        }

        constexpr complex& operator*=(float other) {
            re *= other;
            return *this;
        }

        constexpr complex& operator/=(float other) {
            re /= other;
            return *this;
        }

        constexpr complex& operator=(const complex&) = default;

        template<class X>
        constexpr complex& operator=(const complex<X>& other) {
            re = other.re;
            im = other.im;
            return *this;
        }

        template<class X>
        constexpr complex& operator+=(const complex<X>& other) {
            re += other.re;
            im += other.im;
            return *this;
        }

        template<class X>
        constexpr complex& operator-=(const complex<X>& other) {
            re -= other.re;
            im -= other.im;
            return *this;
        }

        template<class X>
        constexpr complex& operator*=(const complex<X>& other) {
            const complex old = *this;
            re = old.real() * other.real() - old.imag() * other.imag();
            im = old.real() * other.imag() + old.imag() * other.real();
            return *this;
        }

        template<class X>
        constexpr complex& operator/=(const complex<X>& other) {
            const complex old = *this;
            const float modulus = other.real() * other.real() + other.imag() * other.imag();
            re = (old.real() * other.real() + old.imag() * other.imag()) / modulus;
            im = (old.imag() * other.real() - old.real() * other.imag()) / modulus;
            return *this;
        }
    };

    template<>
    class complex<double> {
    private:
        double re;
        double im;
    public:
        using value_type = double;

        constexpr complex(double re = 0.0, double im = 0.0) : re(re), im(im) {}
        constexpr complex(const complex<float>& other) : re(other.real()), im(other.imag()) {}
        constexpr explicit complex(const complex&) = default;
        constexpr explicit complex(const complex<long double>& other) : re(static_cast<double>(std::real(other))), im(static_cast<double>(std::imag(other))) {}

        template<class X>
        constexpr complex(const complex<X>& other) : re(other.re), im(other.im) {}

        constexpr double real() const {
            return re;
        }

        constexpr void real(double r) {
            re = r;
        }

        constexpr double imag() const {
            return im;
        }

        constexpr void imag(double i) {
            im = i;
        }

        constexpr complex& operator=(double other) {
            re = other;
            return *this;
        }

        constexpr complex& operator+=(double other) {
            re += other;
            return *this;
        }

        constexpr complex& operator-=(double other) {
            re -= other;
            return *this;
        }

        constexpr complex& operator*=(double other) {
            re *= other;
            return *this;
        }

        constexpr complex& operator/=(double other) {
            re /= other;
            return *this;
        }

        constexpr complex& operator=(const complex&) = default;

        template<class X>
        constexpr complex& operator=(const complex<X>& other) {
            re = other.re;
            im = other.im;
            return *this;
        }

        template<class X>
        constexpr complex& operator+=(const complex<X>& other) {
            re += other.re;
            im += other.im;
            return *this;
        }

        template<class X>
        constexpr complex& operator-=(const complex<X>& other) {
            re -= other.re;
            im -= other.im;
            return *this;
        }

        template<class X>
        constexpr complex& operator*=(const complex<X>& other) {
            const complex old = *this;
            re = old.real() * other.real() - old.imag() * other.imag();
            im = old.real() * other.imag() + old.imag() * other.real();
            return *this;
        }

        template<class X>
        constexpr complex& operator/=(const complex<X>& other) {
            const complex old = *this;
            const double modulus = other.real() * other.real() + other.imag() * other.imag();
            re = (old.real() * other.real() + old.imag() * other.imag()) / modulus;
            im = (old.imag() * other.real() - old.real() * other.imag()) / modulus;
            return *this;
        }
    };

    template<>
    class complex<long double> {
    private:
        double re;
        double im;
    public:
        using value_type = long double;

        constexpr complex(long double re = 0.0l, long double im = 0.0l) : re(re), im(im) {}
        constexpr complex(const complex<float>& other) : re(other.real()), im(other.imag()) {}
        constexpr explicit complex(const complex<double>& other) : re(other.real()), im(other.imag()) {}
        constexpr explicit complex(const complex<long double>&) = default;

        template<class X>
        constexpr complex(const complex<X>& other) : re(other.re), im(other.im) {}

        constexpr long double real() const {
            return re;
        }

        constexpr void real(long double r) {
            re = r;
        }

        constexpr long double imag() const {
            return im;
        }

        constexpr void imag(long double i) {
            im = i;
        }

        constexpr complex& operator=(long double other) {
            re = other;
            return *this;
        }

        constexpr complex& operator+=(long double other) {
            re += other;
            return *this;
        }

        constexpr complex& operator-=(long double other) {
            re -= other;
            return *this;
        }

        constexpr complex& operator*=(long double other) {
            re *= other;
            return *this;
        }

        constexpr complex& operator/=(long double other) {
            re /= other;
            return *this;
        }

        constexpr complex& operator=(const complex&) = default;

        template<class X>
        constexpr complex& operator=(const complex<X>& other) {
            re = other.re;
            im = other.im;
            return *this;
        }

        template<class X>
        constexpr complex& operator+=(const complex<X>& other) {
            re += other.re;
            im += other.im;
            return *this;
        }

        template<class X>
        constexpr complex& operator-=(const complex<X>& other) {
            re -= other.re;
            im -= other.im;
            return *this;
        }

        template<class X>
        constexpr complex& operator*=(const complex<X>& other) {
            const complex old = *this;
            re = old.real() * other.real() - old.imag() * other.imag();
            im = old.real() * other.imag() + old.imag() * other.real();
            return *this;
        }

        template<class X>
        constexpr complex& operator/=(const complex<X>& other) {
            const complex old = *this;
            const long double modulus = other.real() * other.real() + other.imag() * other.imag();
            re = (old.real() * other.real() + old.imag() * other.imag()) / modulus;
            im = (old.imag() * other.real() - old.real() * other.imag()) / modulus;
            return *this;
        }
    };

    /* 26.4.7 Operators */
    template<class T>
    constexpr complex<T> operator+(const complex<T>& lhs, const complex<T>& rhs) {
        return complex<T>(lhs) += rhs;
    }

    template<class T>
    constexpr complex<T> operator+(const complex<T>& lhs, const T& rhs) {
        return complex<T>(lhs) += rhs;
    }

    template<class T>
    constexpr complex<T> operator+(const T& lhs, const complex<T>& rhs) {
        return complex<T>(lhs) += rhs;
    }

    template<class T>
    constexpr complex<T> operator-(const complex<T>& lhs, const complex<T>& rhs) {
        return complex<T>(lhs) -= rhs;
    }

    template<class T>
    constexpr complex<T> operator-(const complex<T>& lhs, const T& rhs) {
        return complex<T>(lhs) -= rhs;
    }

    template<class T>
    constexpr complex<T> operator-(const T& lhs, const complex<T>& rhs) {
        return complex<T>(lhs) -= rhs;
    }

    template<class T>
    constexpr complex<T> operator*(const complex<T>& lhs, const complex<T>& rhs) {
        return complex<T>(lhs) *= rhs;
    }

    template<class T>
    constexpr complex<T> operator*(const complex<T>& lhs, const T& rhs) {
        return complex<T>(lhs) *= rhs;
    }

    template<class T>
    constexpr complex<T> operator*(const T& lhs, const complex<T>& rhs) {
        return complex<T>(lhs) *= rhs;
    }

    template<class T>
    constexpr complex<T> operator/(const complex<T>& lhs, const complex<T>& rhs) {
        return complex<T>(lhs) /= rhs;
    }

    template<class T>
    constexpr complex<T> operator/(const complex<T>& lhs, const T& rhs) {
        return complex<T>(lhs) /= rhs;
    }

    template<class T>
    constexpr complex<T> operator/(const T& lhs, const complex<T>& rhs) {
        return complex<T>(lhs) /= rhs;
    }

    template<class T>
    constexpr complex<T> operator+(const complex<T>& lhs) {
        return complex<T>(lhs);
    }

    template<class T>
    constexpr complex<T> operator-(const complex<T>& lhs) {
        return complex<T>(-lhs.real(), -lhs.imag());
    }

    template<class T>
    constexpr bool operator==(const complex<T>& lhs, const complex<T>& rhs) {
        return lhs.real() == rhs.real() && lhs.imag() == rhs.imag();
    }

    template<class T>
    constexpr bool operator==(const complex<T>& lhs, const T& rhs) {
        return lhs.real() == rhs && lhs.imag() == 0;
    }

    template<class T, class charT, class traits>
    basic_istream<charT, traits>& operator>>(basic_istream<charT, traits>& is, complex<T>& rhs) {
        if (!is.good()) {
            is.setstate(ios_base::failbit);
            goto end;
        }

        ws(is);
        if (traits::eq_int_type(is.peek(), traits::to_int_type('('))) {
            is.get();
            T real;
            T imag = T(0);
            is >> real;
            if (is.fail()) {
                is.setstate(ios_base::failbit);
                goto end;
            }

            ws(is);
            if (traits::eq_int_type(is.peek(), traits::to_int_type(','))) {
                is.get();
                is >> imag;
                if (is.fail()) {
                    is.setstate(ios_base::failbit);
                    goto end;
                }
            }

            ws(is);
            if (!traits::eq_int_type(is.get(), traits::to_int_type(')'))) {
                is.setstate(ios_base::failbit);
            } else {
                rhs = complex<T>(real, imag);
            }
        } else {
            T real;
            is >> real;
            if (!is.fail()) {
                rhs = complex<T>(real);
            } else {
                is.setstate(ios_base::failbit);
            }
        }

end:
        return is;
    }

    /* TODO: Implement after "stringstream.hpp" is done. */
    template<class T, class charT, class traits>
    basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const complex<T>& rhs);

    /* 26.4.8 Values */
    template<class T>
    constexpr T real(const complex<T>& cpl) {
        return cpl.real();
    }

    template<class T>
    constexpr T imag(const complex<T>& cpl) {
        return cpl.imag();
    }

    template<class T>
    T abs(const complex<T>& cpl) {
        return std::sqrt(std::pow(cpl.real(), 2) + std::pow(cpl.imag(), 2));
    }

    template<class T>
    T arg(const complex<T>& cpl) {
        return std::atan2(imag(cpl), real(cpl));
    }

    template<class T>
    constexpr T norm(const complex<T>& cpl) {
        return std::pow(cpl.real(), 2) + std::pow(cpl.imag(), 2);
    }

    template<class T>
    constexpr complex<T> conj(const complex<T>& cpl) {
        return complex<T>(cpl.real(), -cpl.imag());
    }

    namespace __internal {
        /* A dispatcher that calls the type-correct function in complex.h, given the double, float, and long double versions of the function and the C++ complex object to transform. The complex object must have
         * an internal type equal to either float, double, or long double, or undefined behaviors occur. */
        template<class T>
        inline complex<T> __c_complex_fn_dispatcher(double _Complex (*double_fn)(double _Complex), float _Complex (*float_fn)(float _Complex), long double _Complex (*long_double_fn)(long double _Complex), const complex<T>& arg) noexcept {
            if constexpr (is_same_v<T, float>) {
                const float _Complex z = float_fn(arg.real() + arg.imag() * I);
                return complex<T>(crealf(z), cimagf(z));
            } else if constexpr (is_same_v<T, long double>) {
                const long double _Complex z = long_double_fn(arg.real() + arg.imag() * I);
                return complex<T>(creall(z), cimagl(z));
            } else {
                const double _Complex z = double_fn(arg.real() + arg.imag() * I);
                return complex<T>(creal(z), cimag(z));
            }
        }
    }

    template<class T>
    complex<T> proj(const complex<T>& cpl) {
        return __internal::__c_complex_fn_dispatcher(cproj, cprojf, cprojl, cpl);
    }

    template<class T>
    complex<T> polar(const T& rho, const T& theta = T()) {
        return complex<T>(rho * std::cos(theta), rho * std::sin(theta));
    }

    /* 26.4.9 Transcendentals */
    template<class T>
    complex<T> acos(const complex<T>& cpl) {
        return __internal::__c_complex_fn_dispatcher(cacos, cacosf, cacosl, cpl);
    }

    template<class T>
    complex<T> asin(const complex<T>& cpl) {
        return __internal::__c_complex_fn_dispatcher(casin, casinf, casinl, cpl);
    }

    template<class T>
    complex<T> atan(const complex<T>& cpl) {
        return __internal::__c_complex_fn_dispatcher(catan, catanf, catanl, cpl);
    }

    template<class T>
    complex<T> acosh(const complex<T>& cpl) {
        return __internal::__c_complex_fn_dispatcher(cacosh, cacoshf, cacoshl, cpl);
    }

    template<class T>
    complex<T> asinh(const complex<T>& cpl) {
        return __internal::__c_complex_fn_dispatcher(casinh, casinhf, casinhl, cpl);
    }

    template<class T>
    complex<T> atanh(const complex<T>& cpl) {
        return __internal::__c_complex_fn_dispatcher(catanh, catanhf, catanhl, cpl);
    }

    template<class T>
    complex<T> cos(const complex<T>& cpl) {
        return __internal::__c_complex_fn_dispatcher(ccos, ccosf, ccosl, cpl);
    }

    template<class T>
    complex<T> cosh(const complex<T>& cpl) {
        return __internal::__c_complex_fn_dispatcher(ccosh, ccoshf, ccoshl, cpl);
    }

    template<class T>
    complex<T> exp(const complex<T>& cpl) {
        return __internal::__c_complex_fn_dispatcher(cexp, cexpf, cexpl, cpl);
    }

    template<class T>
    complex<T> log(const complex<T>& cpl) {
        return __internal::__c_complex_fn_dispatcher(clog, clogf, clogl, cpl);
    }

    template<class T>
    complex<T> log10(const complex<T>& cpl) {
        return log(cpl) / log(complex<T>(T(10)));
    }

    template<class T>
    complex<T> pow(const complex<T>& cpl, const T& exp) {
        return pow(cpl, complex<T>(exp));
    }

    template<class T>
    complex<T> pow(const complex<T>& cpl, const complex<T>& exp) {
        if constexpr (is_same_v<T, float>) {
            const float _Complex z = cpowf(cpl.real() + cpl.imag() * I, exp.real() + exp.imag() * I);
            return complex<T>(crealf(z), cimagf(z));
        } else if constexpr (is_same_v<T, long double>) {
            const long double _Complex z = cpowl(cpl.real() + cpl.imag() * I, exp.real() + exp.imag() * I);
            return complex<T>(creall(z), cimagl(z));
        } else {
            const double _Complex z = cpow(cpl.real() + cpl.imag() * I, exp.real() + exp.imag() * I);
            return complex<T>(creal(z), cimag(z));
        }
    }

    template<class T>
    complex<T> pow(const T& x, const complex<T>& exp) {
        return pow(complex<T>(x), exp);
    }

    template<class T>
    complex<T> sin(const complex<T>& cpl) {
        return __internal::__c_complex_fn_dispatcher(csin, csinf, csinl, cpl);
    }

    template<class T>
    complex<T> sinh(const complex<T>& cpl) {
        return __internal::__c_complex_fn_dispatcher(csinh, csinhf, csinhl, cpl);
    }

    template<class T>
    complex<T> sqrt(const complex<T>& cpl) {
        return __internal::__c_complex_fn_dispatcher(csqrt, csqrtf, csqrtl, cpl);
    }

    template<class T>
    complex<T> tan(const complex<T>& cpl) {
        return __internal::__c_complex_fn_dispatcher(ctan, ctanf, ctanl, cpl);
    }

    template<class T>
    complex<T> tanh(const complex<T>& cpl);

    /* 26.4.11 Complex literals */
    inline namespace literals {
        inline namespace complex_literals {
            constexpr complex<long double> operator""il(long double n) {
                return complex<long double>(0.0L, static_cast<long double>(n));
            }

            constexpr complex<long double> operator""il(unsigned long long n) {
                return complex<long double>(0.0L, static_cast<long double>(n));
            }

            constexpr complex<double> operator""i(long double n) {
                return complex<double>(0.0, static_cast<double>(n));
            }

            constexpr complex<double> operator""i(unsigned long long n) {
                return complex<double>(0.0, static_cast<double>(n));
            }

            constexpr complex<float> operator""if(long double n) {
                return complex<float>(0.0f, static_cast<float>(n));
            }

            constexpr complex<float> operator""if(unsigned long long n) {
                return complex<float>(0.0f, static_cast<float>(n));
            }
        }
    }
}