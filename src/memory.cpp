#include "memory.hpp"
#include "cstddef.hpp"
#include "type_traits.hpp"

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
}