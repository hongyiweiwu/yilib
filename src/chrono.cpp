#include "chrono.hpp"

#include "ctime.hpp"

namespace std::chrono {
    system_clock::time_point system_clock::now() noexcept {
        struct timespec t;
        clock_gettime(CLOCK_REALTIME, &t);
        return system_clock::time_point(nanoseconds(t.tv_nsec) + seconds(t.tv_sec));
    }

    std::time_t system_clock::to_time_t(const system_clock::time_point& t) noexcept {
        return duration_cast<seconds>(t.time_since_epoch()).count();
    }

    system_clock::time_point system_clock::from_time_t(std::time_t t) noexcept {
        return system_clock::time_point(seconds(t));
    }

    steady_clock::time_point now() noexcept {
        struct timespec t;
        clock_gettime(CLOCK_MONOTONIC, &t);
        return steady_clock::time_point(nanoseconds(t.tv_nsec) + seconds(t.tv_sec));
    }
}