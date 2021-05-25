#include "memory_resource.hpp"
#include "cstddef.hpp"
#include "new.hpp"
#include "memory.hpp"
#include "cstdint.hpp"

namespace std::pmr {
    void* memory_resource::allocate(size_t bytes, size_t alignment) {
        return do_allocate(bytes, alignment);
    }

    void memory_resource::deallocate(void* p, size_t bytes, size_t alignment) {
        return do_deallocate(p, bytes, alignment);
    }

    bool memory_resource::is_equal(const memory_resource& other) const noexcept {
        return do_is_equal(other);
    }

    bool operator==(const memory_resource& a, const memory_resource& b) noexcept {
        return &a == &b && a.is_equal(b);
    }

    namespace __internal {
        class null_memory_resource : public memory_resource {
        private:
            constexpr null_memory_resource() : memory_resource() {}
            null_memory_resource(const null_memory_resource&) = delete;

            void* do_allocate(size_t, size_t) { throw bad_alloc(); }
            void do_deallocate(void*, size_t, size_t) {}
            bool do_is_equal(const memory_resource& other) const noexcept { return &other == this; }

        public:
            static constinit null_memory_resource singleton;
        };

        class new_delete_memory_resource : public memory_resource {
        private:
            constexpr new_delete_memory_resource() : memory_resource() {}
            new_delete_memory_resource(const new_delete_memory_resource&) = delete;

            void* do_allocate(size_t bytes, size_t alignment) { 
                return ::operator new(bytes, align_val_t(alignment));
            }

            void do_deallocate(void* p, size_t bytes, size_t alignment) {
                return ::operator delete(p, bytes, align_val_t(alignment));
            }

            bool do_is_equal(const memory_resource& other) const noexcept { return &other == this; }

        public:
            static constinit new_delete_memory_resource singleton;
        };

        constinit null_memory_resource null_memory_resource::singleton;
        constinit new_delete_memory_resource new_delete_memory_resource::singleton;
        constinit memory_resource* default_memory_resource = nullptr;
    }

    memory_resource* new_delete_resource() noexcept {
        return &__internal::new_delete_memory_resource::singleton;
    }

    memory_resource* null_memory_resource() noexcept {
        return &__internal::null_memory_resource::singleton;
    }

    memory_resource* set_default_resource(memory_resource* r) noexcept {
        auto old = __internal::default_memory_resource;
        __internal::default_memory_resource = r ? r : new_delete_resource();
        return old;
    }

    memory_resource* get_default_resource() noexcept {
        return __internal::default_memory_resource;
    }


    size_t monotonic_buffer_resource::curr_buffer_size() const noexcept {
        return *(reinterpret_cast<metadata_block_t*>(current_buffer) + 1);
    }

    monotonic_buffer_resource::monotonic_buffer_resource(memory_resource* upstream)
        : memory_resource(), upstream_rsrc(upstream), current_buffer(nullptr), next_buffer_size(default_buffer_size), unused_buffer_part(nullptr) {}

    monotonic_buffer_resource::monotonic_buffer_resource(size_t initial_size, memory_resource* upstream)
        : memory_resource(), upstream_rsrc(upstream), current_buffer(nullptr), next_buffer_size(initial_size), unused_buffer_part(nullptr) {}

    monotonic_buffer_resource::monotonic_buffer_resource(void* buffer, size_t buffer_size, memory_resource* upstream)
        : memory_resource(), upstream_rsrc(upstream), current_buffer(buffer), next_buffer_size(buffer_size * growth_factor), 
            unused_buffer_part(reinterpret_cast<void*>(reinterpret_cast<metadata_block_t*>(buffer) + 2)) {
        *reinterpret_cast<metadata_block_t*>(buffer) = 0;
        *(reinterpret_cast<metadata_block_t*>(buffer) + 1) = buffer_size;
    }

    monotonic_buffer_resource::monotonic_buffer_resource() : monotonic_buffer_resource(get_default_resource()) {}
    monotonic_buffer_resource::monotonic_buffer_resource(size_t initial_size) : monotonic_buffer_resource(initial_size, get_default_resource()) {}
    monotonic_buffer_resource::monotonic_buffer_resource(void* buffer, size_t buffer_size) : monotonic_buffer_resource(buffer, buffer_size, get_default_resource()) {}

    monotonic_buffer_resource::~monotonic_buffer_resource() { release(); }

    void monotonic_buffer_resource::release() {
        while (current_buffer) {
            auto prev_addr = reinterpret_cast<void*>(*reinterpret_cast<metadata_block_t*>(current_buffer));
            upstream_rsrc->deallocate(current_buffer, curr_buffer_size());
            current_buffer = prev_addr;
        }
    }

    memory_resource* monotonic_buffer_resource::upstream_resource() const { return upstream_rsrc; }

    void* monotonic_buffer_resource::do_allocate(size_t bytes, size_t alignment) {
        if (current_buffer) {
            size_t remaining_space = curr_buffer_size() - (reinterpret_cast<ptrdiff_t>(unused_buffer_part) - reinterpret_cast<ptrdiff_t>(current_buffer));
            if (void* addr = align(alignment, bytes, unused_buffer_part, remaining_space); addr) {
                unused_buffer_part = reinterpret_cast<void*>(reinterpret_cast<ptrdiff_t>(unused_buffer_part) + bytes);
                return addr;
            }
        }

        void* prev_buffer = current_buffer;
        while (next_buffer_size < bytes + alignment + 2 * sizeof(metadata_block_t)) {
            next_buffer_size *= growth_factor;
        }
        current_buffer = upstream_rsrc->allocate(next_buffer_size);
        
        metadata_block_t* metadata_ptr = static_cast<metadata_block_t*>(current_buffer);
        *metadata_ptr = reinterpret_cast<metadata_block_t>(prev_buffer);
        metadata_ptr++;
        *metadata_ptr = static_cast<metadata_block_t>(next_buffer_size);
        size_t remaining_space = next_buffer_size - 2 * sizeof(metadata_block_t);
        next_buffer_size *= growth_factor;
        unused_buffer_part = metadata_ptr + 1;

        void* addr = align(alignment, bytes, unused_buffer_part, remaining_space);
        unused_buffer_part = reinterpret_cast<void*>(reinterpret_cast<ptrdiff_t>(unused_buffer_part) + bytes);
        return addr;
    }

    void monotonic_buffer_resource::do_deallocate(void*, size_t, size_t) {}
    bool monotonic_buffer_resource::do_is_equal(const memory_resource& other) const noexcept { return this == &other; }
}