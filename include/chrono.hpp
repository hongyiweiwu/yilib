#pragma once

namespace std::chrono {
    template<class Rep, class Period = void>
    struct duration {};

    template<class Rep, class Period = void>
    struct time_point {};

    struct steady_clock {
        static time_point<steady_clock> now() noexcept {
            return {};
        }
    };
}