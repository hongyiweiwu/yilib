#pragma once

#include "cstdint.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "numeric.hpp"

namespace std {
    /* 20.16.3 Class template ratio */
    template<std::intmax_t N, std::intmax_t D = 1>
    requires (D != 0)
    class ratio {
    private:
        /* Returns 1 if the argument is nonnegative, -1 otherwise. */
        template<std::intmax_t X>
        static constexpr int sign() {
            return X > 0 ? 1 : X < 0 ? -1 : 0;
        }

        /* Returns the absolute value of the given argument. */
        template<std::intmax_t X>
        static constexpr std::intmax_t abs() {
            return X >= 0 ? X : -X;
        }

    public:
        static constexpr std::intmax_t num = sign<N>() * sign<D>() * abs<N>() / gcd(abs<N>(), abs<D>());
        static constexpr std::intmax_t den = abs<D>() / gcd(abs<N>(), abs<D>());
        using type = ratio<num, den>;
    };

    namespace __internal {
        template<class T>
        struct is_specialization_of_ratio : false_type {};

        template<std::intmax_t N, std::intmax_t D>
        struct is_specialization_of_ratio<ratio<N, D>> : true_type {};

        /* 20.16.4 Ratio arithmetic */
        template<std::intmax_t x, std::intmax_t y>
        requires (!(y > 0 && x > numeric_limits<std::intmax_t>::max() - y)) && (!(y < 0 && x < numeric_limits<std::intmax_t>::min() - y))
        static consteval std::intmax_t no_overflow_addition() noexcept {
            return x + y;
        }

        /* Safely multiplies the given two integers. Participates in overload resolution only if the multiplication doesn't result in overflow. */
        template<std::intmax_t x, std::intmax_t y>
        requires (x == 0) || (y == 0) || (x * y / y == x)
        static consteval std::intmax_t no_overflow_multiply() noexcept {
            return x * y;
        }
    }

    template<class R1, class R2>
    using ratio_multiply = ratio<__internal::no_overflow_multiply<R1::num / gcd(R1::num, R2::den), R2::num / gcd(R1::den, R2::num)>(),
        __internal::no_overflow_multiply<R1::den / gcd(R1::den, R2::num), R2::den / gcd(R1::num, R2::den)>()>;

    template<class R1, class R2>
    requires (R2::num != 0)
    using ratio_divide = ratio_multiply<ratio<R1::num, R1::den>, ratio<R2::den, R2::num>>;

    template<class R1, class R2>
    using ratio_add = ratio_multiply<
        ratio<gcd(R1::num, R2::num), R1::den / gcd(R1::den, R2::den)>,
        ratio<__internal::no_overflow_addition<
            __internal::no_overflow_multiply<R1::num / gcd(R1::num, R2::num), R2::den / gcd(R1::den, R2::den)>(),
            __internal::no_overflow_multiply<R2::num / gcd(R1::num, R2::num), R1::den / gcd(R1::den, R2::den)>()
        >(), R2::den>
    >;

    template<class R1, class R2>
    using ratio_subtract = ratio_add<R1, ratio<-R2::num, R2::den>>;


    /* 20.16.5 Ratio comparison */
    template<class R1, class R2>
    struct ratio_equal : bool_constant<R1::num == R2::num && R1::den == R2::den> {};

    template<class R1, class R2>
    struct ratio_not_equal : bool_constant<!ratio_equal<R1, R2>::value> {};

    namespace __internal {
        template<class R1, class R2>
        static consteval bool __do_ratio_less() noexcept {
            constexpr auto sign = []<signed_integral T>(T x) {
                return x > 0 ? 1 : x < 0 ? -1 : 0;
            };

            if constexpr (constexpr int r1_sign = sign(R1::num), r2_sign = sign(R2::num); r1_sign != r2_sign) {
                // The two ratios don't have the same sign. So we can directly compare their sign and get the result.
                return r1_sign < r2_sign;
            } else if constexpr (r1_sign == -1) {
                // Both ratios have negative signs. To simplify we exchange their place.
                return __do_ratio_less<ratio<-R2::num, R2::den>, ratio<-R1::num, R1::den>>();
            }

            if constexpr (constexpr std::intmax_t q1 = R1::num / R1::den, q2 = R2::num / R2::den; q1 != q2) {
                return q1 < q2;
            }

            constexpr std::intmax_t m1 = R1::num % R1::den;
            constexpr std::intmax_t m2 = R2::num % R2::den;

            return m1 < m2;
        }
    }

    template<class R1, class R2>
    struct ratio_less : bool_constant<__internal::__do_ratio_less<R1, R2>()> {};

    template<class R1, class R2>
    struct ratio_less_equal : bool_constant<ratio_less<R2, R1>::value> {};

    template<class R1, class R2>
    struct ratio_greater : bool_constant<ratio_less<R2, R1>::value> {};

    template<class R1, class R2>
    struct ratio_greater_equal : bool_constant<!ratio_less<R1, R2>::value> {};

    template<class R1, class R2>
    inline constexpr bool ratio_equal_v = ratio_equal<R1, R2>::value;

    template<class R1, class R2>
    inline constexpr bool ratio_not_equal_v = ratio_not_equal<R1, R2>::value;

    template<class R1, class R2>
    inline constexpr bool ratio_less_v = ratio_less<R1, R2>::value;

    template<class R1, class R2>
    inline constexpr bool ratio_less_equal_v = ratio_less_equal<R1, R2>::value;

    template<class R1, class R2>
    inline constexpr bool ratio_greater_v = ratio_greater<R1, R2>::value;

    template<class R1, class R2>
    inline constexpr bool ratio_greater_equal_v = ratio_greater_equal<R1, R2>::value;

    /* 20.16.6 Convenience SI typedefs */
    using atto = ratio<1, 1'000'000'000'000'000'000>;
    using femto = ratio<1, 1'000'000'000'000'000>;
    using pico = ratio<1, 1'000'000'000'000>;
    using nano = ratio<1, 1'000'000'000>;
    using micro = ratio<1, 1'000'000>;
    using milli = ratio<1, 1'000>;
    using centi = ratio<1, 100>;
    using deci = ratio<1, 10>;
    using deca = ratio<10, 1>;
    using hecto = ratio<100, 1>;
    using kilo = ratio<1'000, 1>;
    using mega = ratio<1'000'000, 1>;
    using giga = ratio<1'000'000'000, 1>;
    using tera = ratio<1'000'000'000'000, 1>;
    using peta = ratio<1'000'000'000'000'000, 1>;
    using exa = ratio<1'000'000'000'000'000'000, 1>;
}
