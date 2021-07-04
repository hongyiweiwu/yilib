#pragma once

// NOTE: All implementation here is dummy ones.

namespace std {
    class thread {
    public:
        class id {
        public:
            id() noexcept = default;
        };
    };

    bool operator==(thread::id, thread::id) noexcept { return true; }

    namespace this_thread {
        thread::id get_id() noexcept { return thread::id(); }
    }
}