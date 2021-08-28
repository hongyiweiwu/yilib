#pragma once

#include "ratio.hpp"
#include "type_traits.hpp"
#include "concepts.hpp"
#include "numeric.hpp"
#include "ostream.hpp"
#include "string.hpp"
#include "limits.hpp"
#include "ctime.hpp"


namespace std {
    namespace chrono {
        /* 27.4 Customization traits */
        template<class Rep>
        struct treat_as_floating_point : is_floating_point<Rep> {};

        template<class Rep>
        inline constexpr bool treat_as_floating_point_v = treat_as_floating_point<Rep>::value;

        template<class Rep>
        struct duration_values {
        public:
            static constexpr Rep zero() noexcept {
                return Rep(0);
            }

            static constexpr Rep min() noexcept {
                return numeric_limits<Rep>::lowest();
            }

            static constexpr Rep max() noexcept {
                return numeric_limits<Rep>::max();
            }
        };
    }

    namespace chrono {
        /* 27.5 Class template duration */
        template<class Rep, class Period = ratio<1>>
        requires std::__internal::is_specialization_of_ratio<Period>::value && (Period::num > 0)
        class duration {
        public:
            using rep = Rep;
            using period = typename Period::type;

        private:
            rep r;

        public:
            constexpr duration() = default;

            template<class Rep2>
            requires is_convertible_v<const Rep2&, rep> && (treat_as_floating_point_v<rep> || !treat_as_floating_point_v<Rep2>)
            constexpr explicit duration(const Rep2& r) : r(r) {}

            template<class Rep2, class Period2>
            requires treat_as_floating_point_v<rep> || (ratio_divide<Period2, period>::den == 1 && !treat_as_floating_point_v<Rep2>)
            constexpr duration(const duration<Rep2, Period2>& d) : r(duration_cast<duration>(d).count()) {}

            ~duration() = default;
            duration(const duration&) = default;
            duration& operator=(const duration&) = default;

            constexpr rep count() const {
                return r;
            }

            constexpr auto operator+() const {
                return common_type_t<duration>(*this);
            }

            constexpr auto operator-() const {
                return common_type_t<duration>(-r);
            }

            constexpr duration& operator++() {
                ++r;
                return *this;
            }

            constexpr duration operator++(int) {
                return duration(r++);
            }

            constexpr duration& operator--() {
                --r;
                return *this;
            }

            constexpr duration operator--(int) {
                return duration(r--);
            }

            constexpr duration& operator+=(const duration& d) {
                r += d.count();
                return *this;
            }

            constexpr duration& operator-=(const duration& d) {
                r += d.count();
                return *this;
            }

            constexpr duration& operator*=(const rep& rhs) {
                r *= rhs;
                return *this;
            }

            constexpr duration& operator/=(const rep& rhs) {
                r /= rhs;
                return *this;
            }

            constexpr duration& operator%=(const rep& rhs) {
                r %= rhs;
                return *this;
            }

            constexpr duration& operator%=(const duration& rhs) {
                r %= rhs.count();
                return *this;
            }

            static constexpr duration zero() noexcept {
                return duration(duration_values<rep>::zero());
            }

            static constexpr duration min() noexcept {
                return duration(duration_values<rep>::min());
            }

            static constexpr duration max() noexcept {
                return duration(duration_values<rep>::max());
            }
        };
    }

    namespace __internal {
        template<class T>
        struct is_specialization_of_duration : false_type {};

        template<class Rep, class Period>
        struct is_specialization_of_duration<std::chrono::duration<Rep, Period>> : true_type {};

        template<class T>
        concept clock = requires {
            typename T::rep;

            typename T::period;
            requires is_specialization_of_ratio<typename T::period>::value;

            typename T::duration;
            requires is_same_v<typename T::duration, std::chrono::duration<typename T::rep, typename T::period>>;

            typename T::time_point;
            { T::is_steady } -> same_as<const bool>;
            { T::now() } -> same_as<typename T::time_point>;
        };

        template<class T>
        concept trivial_clock = clock<T> && requires (typename T::rep& rep, typename T::duration& duration, typename T::time_point& time_point) {
            { rep == rep } -> convertible_to<bool>;
            { duration == duration } -> convertible_to<bool>;
            { time_point == time_point } -> convertible_to<bool>;

            { rep < rep } -> convertible_to<bool>;
            { duration < duration } -> convertible_to<bool>;
            { time_point < time_point } -> convertible_to<bool>;

            requires is_swappable_v<typename T::rep&>;
            requires is_swappable_v<typename T::duration&>;
            requires is_swappable_v<typename T::time_point&>;

            { T::now() } noexcept;
        };
    }

    template<class Rep1, class Period1, class Rep2, class Period2>
    struct common_type<chrono::duration<Rep1, Period1>, chrono::duration<Rep2, Period2>> {
        using type = chrono::duration<common_type_t<Rep1, Rep2>, ratio<gcd(Period1::num, Period2::num), lcm(Period1::den, Period2::den)>>;
    };

    namespace chrono {
        template<class T>
        struct is_clock : bool_constant<std::__internal::clock<T>> {};

        template<class T>
        inline constexpr bool is_clock_v = is_clock<T>::value;

        /* 27.6 Class template time_point */
        // Forward declaration. Defined below.
        struct local_t;

        template<class Clock, class Duration = typename Clock::duration>
        requires std::__internal::is_specialization_of_duration<Duration>::value
            && (std::__internal::clock<Clock> || is_same_v<Clock, local_t>)
        class time_point {
        public:
            using clock = Clock;
            using duration = Duration;
            using rep = typename duration::rep;
            using period = typename duration::period;

        private:
            duration d;

        public:
            constexpr time_point() : d(duration::zero()) {}

            constexpr explicit time_point(const duration& d) : d(d) {}

            template<class Duration2>
            requires is_convertible_v<Duration2, duration>
            constexpr time_point(const time_point<clock, Duration2>& t) : d(t.time_since_epoch()) {}

            constexpr duration time_since_epoch() const {
                return d;
            }

            constexpr time_point& operator++() {
                ++d;
                return *this;
            }

            constexpr time_point operator++(int) {
                return time_point(d++);
            }

            constexpr time_point& operator--() {
                --d;
                return *this;
            }

            constexpr time_point operator--(int) {
                return time_point(d--);
            }

            constexpr time_point& operator+=(const duration& d) {
                this->d += d;
                return *this;
            }

            constexpr time_point& operator-=(const duration& d) {
                this->d -= d;
                return *this;
            }

            static constexpr time_point min() noexcept {
                return time_point(duration::min());
            }

            static constexpr time_point max() noexcept {
                return time_point(duration::max());
            }
        };
    }

    template<class Clock, class Duration1, class Duration2>
    struct common_type<chrono::time_point<Clock, Duration1>, chrono::time_point<Clock, Duration2>> {
        using type = chrono::time_point<Clock, common_type_t<Duration1, Duration2>>;
    };

    namespace chrono {
        /* 27.5.6 duration arithmetic */
        template<class Rep1, class Period1, class Rep2, class Period2>
        constexpr common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>
        operator+(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs) {
            using return_type = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
            return return_type(return_type(lhs).count() + return_type(rhs).count());
        }

        template<class Rep1, class Period1, class Rep2, class Period2>
        constexpr common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>
        operator-(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs) {
            using return_type = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
            return return_type(return_type(lhs).count() - return_type(rhs).count());
        }

        template<class Rep1, class Period, class Rep2>
        requires is_convertible_v<const Rep2&, common_type_t<Rep1, Rep2>>
        constexpr duration<common_type_t<Rep1, Rep2>, Period>
        operator*(const duration<Rep1, Period>& d, const Rep2& s) {
            using return_type = duration<common_type_t<Rep1, Rep2>, Period>;
            return return_type(return_type(d).count() * s);
        }

        template<class Rep1, class Rep2, class Period>
        requires is_convertible_v<const Rep1&, common_type_t<Rep1, Rep2>>
        constexpr duration<common_type_t<Rep1, Rep2>, Period>
        operator*(const Rep1& s, const duration<Rep2, Period>& d) {
            return d * s;
        }

        template<class Rep1, class Period, class Rep2>
        requires is_convertible_v<const Rep2&, common_type_t<Rep1, Rep2>> && (!std::__internal::is_specialization_of_duration<Rep2>::value)
        constexpr duration<common_type_t<Rep1, Rep2>, Period>
        operator/(const duration<Rep1, Period>& d, const Rep2& s) {
            using return_type = duration<common_type_t<Rep1, Rep2>, Period>;
            return return_type(return_type(d).count() / s);
        }

        template<class Rep1, class Period1, class Rep2, class Period2>
        constexpr common_type_t<Rep1, Rep2>
        operator/(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs) {
            using common_type = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
            return common_type(lhs).count() / common_type(rhs).count();
        }

        template<class Rep1, class Period, class Rep2>
        requires is_convertible_v<const Rep2&, common_type_t<Rep1, Rep2>> && (!std::__internal::is_specialization_of_duration<Rep2>::value)
        constexpr duration<common_type_t<Rep1, Rep2>, Period>
        operator%(const duration<Rep1, Period>& d, const Rep2& s) {
            using return_type = duration<common_type_t<Rep1, Rep2>, Period>;
            return return_type(return_type(d).count() % s);
        }

        template<class Rep1, class Period1, class Rep2, class Period2>
        constexpr common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>
        operator%(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs) {
            using return_type = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
            return return_type(return_type(lhs).count() % return_type(rhs).count());
        }

        /* 27.5.7 duration comparisons */
        template<class Rep1, class Period1, class Rep2, class Period2>
        constexpr bool operator==(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs) {
            using common_type = typename common_type_t<const duration<Rep1, Period1>&, const duration<Rep2, Period2>&>::rep;
            return common_type(lhs).count() == common_type(rhs).count();
        }

        template<class Rep1, class Period1, class Rep2, class Period2>
        constexpr bool operator<(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs) {
            using common_type = typename common_type_t<const duration<Rep1, Period1>&, const duration<Rep2, Period2>&>::rep;
            return common_type(lhs).count() < common_type(rhs).count();
        }

        template<class Rep1, class Period1, class Rep2, class Period2>
        constexpr bool operator>(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs) {
            return rhs < lhs;
        }

        template<class Rep1, class Period1, class Rep2, class Period2>
        constexpr bool operator<=(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs) {
            return !(rhs < lhs);
        }

        template<class Rep1, class Period1, class Rep2, class Period2>
        constexpr bool operator>=(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs) {
            return !(lhs < rhs);
        }

        template<class Rep1, class Period1, class Rep2, class Period2>
        requires three_way_comparable<typename common_type_t<const duration<Rep1, Period1>&, const duration<Rep2, Period2>&>::rep>
        constexpr auto operator<=>(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs) {
            using common_type = typename common_type_t<const duration<Rep1, Period1>&, const duration<Rep2, Period2>&>::rep;
            return common_type(lhs).count() <=> common_type(rhs).count();
        }

        /* 27.5.8 Conversions */
        template<class ToDuration, class Rep, class Period>
        requires std::__internal::is_specialization_of_duration<ToDuration>::value
        constexpr ToDuration duration_cast(const duration<Rep, Period>& d) {
            using CF = ratio_divide<Period, typename ToDuration::period>;
            using CR = common_type_t<typename ToDuration::rep, Rep, std::intmax_t>;

            if constexpr (CF::num == 1 && CF::den == 1) {
                return ToDuration(static_cast<typename ToDuration::rep>(d.count()));
            } else if constexpr (CF::num != 1 && CF::den == 1) {
                return ToDuration(static_cast<typename ToDuration::rep>(static_cast<CR>(d.count()) * static_cast<CR>(CF::num)));
            } else if constexpr (CF::num == 1 && CF::den != 1) {
                return ToDuration(static_cast<typename ToDuration::rep>(static_cast<CR>(d.count()) / static_cast<CR>(CF::den)));
            } else {
                return ToDuration(static_cast<typename ToDuration::rep>(static_cast<CR>(d.count()) * static_cast<CR>(CF::num) / static_cast<CR>(CF::den)));
            }
        }

        template<class ToDuration, class Rep, class Period>
        requires std::__internal::is_specialization_of_duration<ToDuration>::value
        constexpr ToDuration floor(const duration<Rep, Period>& d) {
            const ToDuration t = duration_cast<ToDuration>(d);
            if (t > d) {
                return t - ToDuration(1);
            } else {
                return t;
            }
        }

        template<class ToDuration, class Rep, class Period>
        requires std::__internal::is_specialization_of_duration<ToDuration>::value
        constexpr ToDuration ceil(const duration<Rep, Period>& d) {
            const ToDuration t = duration_cast<ToDuration>(d);
            if (t < d) {
                return t + ToDuration(1);
            } else {
                return t;
            }
        }

        template<class ToDuration, class Rep, class Period>
        requires std::__internal::is_specialization_of_duration<ToDuration>::value && treat_as_floating_point_v<typename ToDuration::rep>
        constexpr ToDuration round(const duration<Rep, Period>& d) {
            const ToDuration lower = floor<ToDuration>(d);
            const ToDuration upper = lower + ToDuration(1);

            const common_type_t<ToDuration, duration<Rep, Period>> lower_diff = d - lower;
            const common_type_t<ToDuration, duration<Rep, Period>> upper_diff = upper - d;

            if (lower_diff == upper_diff) {
                if (lower.count() % 1 == 0) {
                    return lower;
                } else {
                    return upper;
                }
            } else if (lower_diff < upper_diff) {
                return lower;
            } else {
                return upper;
            }
        }

        /* 27.5.11 duration I/O */
        template<class charT, class traits, class Rep, class Period>
        basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const duration<Rep, Period>& d) {
            constexpr auto get_unit_suffix = []() -> string {
                if constexpr (is_same_v<typename Period::type, atto>) {
                    return "as";
                } else if constexpr (is_same_v<typename Period::type, femto>) {
                    return "fs";
                } else if constexpr (is_same_v<typename Period::type, pico>) {
                    return "ps";
                } else if constexpr (is_same_v<typename Period::type, nano>) {
                    return "ns";
                } else if constexpr (is_same_v<typename Period::type, micro>) {
                    return "us";
                } else if constexpr (is_same_v<typename Period::type, milli>) {
                    return "ms";
                } else if constexpr (is_same_v<typename Period::type, centi>) {
                    return "cs";
                } else if constexpr (is_same_v<typename Period::type, deci>) {
                    return "ds";
                } else if constexpr (is_same_v<typename Period::type, ratio<1>>) {
                    return "s";
                } else if constexpr (is_same_v<typename Period::type, deca>) {
                    return "das";
                } else if constexpr (is_same_v<typename Period::type, hecto>) {
                    return "hs";
                } else if constexpr (is_same_v<typename Period::type, kilo>) {
                    return "ks";
                } else if constexpr (is_same_v<typename Period::type, mega>) {
                    return "Ms";
                } else if constexpr (is_same_v<typename Period::type, giga>) {
                    return "Gs";
                } else if constexpr (is_same_v<typename Period::type, tera>) {
                    return "Ts";
                } else if constexpr (is_same_v<typename Period::type, peta>) {
                    return "Ps";
                } else if constexpr (is_same_v<typename Period::type, exa>) {
                    return "Es";
                } else if constexpr (is_same_v<typename Period::type, ratio<60>>) {
                    return "min";
                } else if constexpr (is_same_v<typename Period::type, ratio<3600>>) {
                    return "h";
                } else if constexpr (is_same_v<typename Period::type, ratio<86400>>) {
                    return "d";
                } else if constexpr (Period::type::den == 1) {
                    return string("[") + to_string(Period::type::num) + "]s";
                } else {
                    return string("[") + to_string(Period::type::num) + "/" + to_string(Period::type::den) + "]s";
                }
            };

            basic_ostringstream<charT, traits> s;
            s.flags(os.flags());
            s.imbue(os.getloc());
            s.precision(os.precision());
            s << d.count() << get_unit_suffix();
            return os << s.str();
        }

        // TODO: Implement */
        template<class charT, class traits, class Rep, class Period, class Alloc = allocator<charT>>
        basic_istream<charT, traits>& from_stream(basic_istream<charT, traits>& is, const charT* fmt, duration<Rep, Period>& d, basic_string<charT, traits, Alloc>* abbrev = nullptr, duration<long, ratio<60>>* offset = nullptr);

        using nanoseconds = duration<long long, nano>;
        using microseconds = duration<long long, micro>;
        using milliseconds = duration<long long, milli>;
        using seconds = duration<long long>;
        using minutes = duration<long, ratio<60>>;
        using hours = duration<long, ratio<3600>>;
        using days = duration<long, ratio_multiply<ratio<24>, hours::period>>;
        using weeks = duration<long, ratio_multiply<ratio<7>, days::period>>;
        using years = duration<long, ratio_multiply<ratio<146097, 400>, days::period>>;
        using months = duration<long, ratio_divide<years::period, ratio<12>>>;

        /* 27.6.6 time_point arithmetic */
        template<class Clock, class Duration1, class Rep2, class Period2>
        constexpr time_point<Clock, common_type_t<Duration1, duration<Rep2, Period2>>>
        operator+(const time_point<Clock, Duration1>& lhs, const duration<Rep2, Period2>& rhs) {
            return time_point<Clock, common_type_t<Duration1, duration<Rep2, Period2>>>(lhs.time_since_epoch() + rhs);
        }

        template<class Rep1, class Period1, class Clock, class Duration2>
        constexpr time_point<Clock, common_type_t<duration<Rep1, Period1>, Duration2>>
        operator+(const duration<Rep1, Period1>& lhs, const time_point<Clock, Duration2>& rhs) {
            return rhs + lhs;
        }

        template<class Clock, class Duration1, class Rep2, class Period2>
        constexpr time_point<Clock, common_type_t<Duration1, duration<Rep2, Period2>>>
        operator-(const time_point<Clock, Duration1>& lhs, const duration<Rep2, Period2>& rhs) {
            return time_point<Clock, common_type_t<Duration1, duration<Rep2, Period2>>>(lhs.time_since_epoch() - rhs);
        }

        template<class Clock, class Duration1, class Duration2>
        constexpr common_type_t<Duration1, Duration2>
        operator-(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs) {
            return lhs.time_since_epoch() - rhs.time_since_epoch();
        }

        /* 27.6.7 time_point comparisons */
        template<class Clock, class Duration1, class Duration2>
        constexpr bool operator==(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs) {
            return lhs.time_since_epoch() == rhs.time_since_epoch();
        }

        template<class Clock, class Duration1, class Duration2>
        constexpr bool operator<(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs) {
            return lhs.time_since_epoch() < rhs.time_since_epoch();
        }

        template<class Clock, class Duration1, class Duration2>
        constexpr bool operator>(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs) {
            return rhs < lhs;
        }

        template<class Clock, class Duration1, class Duration2>
        constexpr bool operator<=(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs) {
            return !(rhs < lhs);
        }

        template<class Clock, class Duration1, class Duration2>
        constexpr bool operator>=(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs) {
            return !(lhs < rhs);
        }

        template<class Clock, class Duration1, three_way_comparable_with<Duration1> Duration2>
        constexpr auto operator<=>(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs) {
            return lhs.time_since_epoch() <=> rhs.time_since_epoch();
        }

        template<class ToDuration, class Clock, class Duration>
        requires std::__internal::is_specialization_of_duration<ToDuration>::value
        constexpr time_point<Clock, ToDuration> time_point_cast(const time_point<Clock, Duration>& t) {
            return time_point<Clock, ToDuration>(duration_cast<ToDuration>(t.time_since_epoch()));
        }

        template<class ToDuration, class Clock, class Duration>
        requires std::__internal::is_specialization_of_duration<ToDuration>::value
        constexpr time_point<Clock, ToDuration> floor(const time_point<Clock, Duration>& tp) {
            return time_point<Clock, ToDuration>(floor<ToDuration>(tp.time_since_epoch()));
        }

        template<class ToDuration, class Clock, class Duration>
        requires std::__internal::is_specialization_of_duration<ToDuration>::value
        constexpr time_point<Clock, ToDuration> ceil(const time_point<Clock, Duration>& tp) {
            return time_point<Clock, ToDuration>(ceil<ToDuration>(tp.time_since_epoch()));
        }

        template<class ToDuration, class Clock, class Duration>
        requires std::__internal::is_specialization_of_duration<ToDuration>::value
        constexpr time_point<Clock, ToDuration> round(const time_point<Clock, Duration>& tp) {
            return time_point<Clock, ToDuration>(round<ToDuration>(tp.time_since_epoch()));
        }

        /* 27.5.10 Specialized algorithms */
        template<class Rep, class Period>
        requires numeric_limits<Rep>::is_signed
        constexpr duration<Rep, Period> abs(duration<Rep, Period> d) {
            return d >= d.zero() ? d : -d;
        }

        namespace __internal {
            /* The base class for clocks. In implementations of all classes satisfying the Clock concept, a type alias `time_point` is required to be provided as `chrono::time_point<clock_name>`.
             * However, during the declaration of this alias, the template is instantiated, but at that point the `time_point` hasn't been admitted as a type alias yet, so the concept checking fails.
             * This base provides a dummy `time_point` alias to allow concept checking to pass. The child classes should then override the `time_point` alias as needed. */
            class __clock_base {
            protected:
                using time_point = int;
                constexpr static time_point now() noexcept {
                    return 0;
                }
            };
        }

        class system_clock : private __internal::__clock_base {
        public:
            using rep = long long;
            using period = nano;
            using duration = duration<rep, period>;

            static constexpr bool is_steady = false;

            static chrono::time_point<system_clock> now() noexcept;

            static std::time_t to_time_t(const chrono::time_point<system_clock>& t) noexcept;
            static chrono::time_point<system_clock> from_time_t(std::time_t t) noexcept;

            using time_point = chrono::time_point<system_clock>;
        };

        template<class Duration>
        using sys_time = time_point<system_clock, Duration>;

        using sys_seconds = sys_time<seconds>;
        using sys_days = sys_time<days>;

        /* TODO: Implement. */
        template<class charT, class traits, class Duration>
        requires treat_as_floating_point_v<typename Duration::rep> && (Duration(1) < days(1))
        basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const sys_time<Duration>& tp);

        template<class charT, class traits>
        basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const sys_days& dp);

        template<class charT, class traits, class Duration, class Alloc = allocator<charT>>
        basic_istream<charT, traits>& from_stream(basic_istream<charT, traits>& is, const charT* fmt, sys_time<Duration>& tp, basic_string<charT, traits, Alloc>* abbrev = nullptr, minutes* offset = nullptr);

        struct steady_clock : private __internal::__clock_base {
            using rep = long long;
            using period = nano;
            using duration = duration<rep, period>;

            static constexpr bool is_steady = true;

            static chrono::time_point<steady_clock> now() noexcept;

            using time_point = chrono::time_point<steady_clock>;
        };

        using high_resolution_clock = system_clock;
    }
}