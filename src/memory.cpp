#include "cstddef.hpp"
#include "type_traits.hpp"

#include "memory/shared_ptr.hpp"

#include "stdatomic.h"

namespace std {
    void declare_reachable(void* p) {}
    void declare_no_pointers(char* p, size_t n) {}
    void undeclare_no_pointers(char* p, size_t n) {}
    pointer_safety get_pointer_safety() noexcept { return pointer_safety::relaxed; }

    void* align(size_t alignment, size_t size, void*& ptr, size_t& space) {
        // This is the amount of spacing we need from ptr before start loading an object of the given alignment.
        auto spacing = (alignment - reinterpret_cast<make_unsigned_t<ptrdiff_t>>(ptr) % alignment) % alignment;
        if (spacing + size > space) {
            return nullptr;
        }

        space -= spacing;
        ptr = static_cast<void*>(static_cast<char*>(ptr) + spacing);
        return ptr;
    }

    const char* bad_weak_ptr::what() const noexcept { return "std::bad_weak_ptr"; }

    namespace __internal {
        __ctrl::__ctrl() noexcept {
            atomic_init(&weak_count, 1);
            atomic_init(&shared_count, 1);
        }

        long __ctrl::get_weak_count() const noexcept { return atomic_load(&weak_count); }
        long __ctrl::get_shared_count() const noexcept { return atomic_load(&shared_count); }

        long __ctrl::decrement_shared_count() noexcept {
            if (is_empty()) return 0;

            const auto count = atomic_fetch_sub(&shared_count, 1);
            if (!count) {
                delete_content();
                decrement_weak_count();
            }

            return count;
        }

        long __ctrl::decrement_weak_count() noexcept {
            const auto count = atomic_fetch_sub(&weak_count, 1);
            if (!count) delete_block();

            return count;
        }

        long __ctrl::increment_shared_count() noexcept {
            if (!is_empty()) return atomic_fetch_add(&shared_count, 1);
            return 0;
        }

        void __ctrl::delete_block() noexcept { delete this; }
    }
}