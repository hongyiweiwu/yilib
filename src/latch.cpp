#include "latch.hpp"
#include "mutex.hpp"
#include "condition_variable.hpp"

namespace std {
    void latch::count_down(std::ptrdiff_t update) {
        {
            const unique_lock<mutex> lock(mtx);
            counter -= update;
            if (counter < 0) {
                counter = 0;
            }
        }

        if (counter == 0) {
            condvar.notify_all();
        }
    }

    bool latch::try_wait() const noexcept {
        try {
            const unique_lock<mutex> lock(mtx);
            return counter == 0;
        } catch (...) {
            return false;
        }
    }

    void latch::wait() const {
        unique_lock<mutex> lock(mtx);
        condvar.wait(lock, [this]() { return counter == 0; });
    }

    void latch::arrive_and_wait(std::ptrdiff_t update) {
        count_down(update);
        wait();
    }
}