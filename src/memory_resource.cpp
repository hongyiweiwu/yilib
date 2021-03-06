#include "memory_resource.hpp"
#include "cstddef.hpp"
#include "new.hpp"
#include "memory.hpp"
#include "cstdint.hpp"

namespace std::pmr {
    void* memory_resource::allocate(std::size_t bytes, std::size_t alignment) {
        return do_allocate(bytes, alignment);
    }

    void memory_resource::deallocate(void* p, std::size_t bytes, std::size_t alignment) {
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

            void* do_allocate(std::size_t bytes, std::size_t alignment) { 
                return ::operator new(bytes, align_val_t(alignment));
            }

            void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) {
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
        memory_resource* old = __internal::default_memory_resource;
        __internal::default_memory_resource = r ? r : new_delete_resource();
        return old;
    }

    memory_resource* get_default_resource() noexcept {
        return __internal::default_memory_resource;
    }

    monotonic_buffer_resource::monotonic_buffer_resource(memory_resource* upstream)
        : memory_resource(), upstream_rsrc(upstream), current_buffer(nullptr), next_buffer_size(default_buffer_size), unused_buffer_part(nullptr) {}

    monotonic_buffer_resource::monotonic_buffer_resource(std::size_t initial_size, memory_resource* upstream)
        : memory_resource(), upstream_rsrc(upstream), current_buffer(nullptr), next_buffer_size(initial_size), unused_buffer_part(nullptr) {}

    monotonic_buffer_resource::monotonic_buffer_resource(void* buffer, std::size_t buffer_size, memory_resource* upstream)
        : memory_resource(), upstream_rsrc(upstream), current_buffer(buffer), next_buffer_size(buffer_size * growth_factor), 
            unused_buffer_part(static_cast<void*>(static_cast<metadata*>(buffer) + 1)) {
        *static_cast<metadata*>(buffer) = { .prev_buffer = nullptr, .buffer_size = buffer_size };
    }

    monotonic_buffer_resource::monotonic_buffer_resource() : monotonic_buffer_resource(get_default_resource()) {}
    monotonic_buffer_resource::monotonic_buffer_resource(std::size_t initial_size) : monotonic_buffer_resource(initial_size, get_default_resource()) {}
    monotonic_buffer_resource::monotonic_buffer_resource(void* buffer, std::size_t buffer_size) : monotonic_buffer_resource(buffer, buffer_size, get_default_resource()) {}

    monotonic_buffer_resource::~monotonic_buffer_resource() { release(); }

    void monotonic_buffer_resource::release() {
        while (current_buffer) {
            const struct metadata& metadata = *static_cast<struct metadata*>(current_buffer);
            upstream_rsrc->deallocate(current_buffer, metadata.buffer_size);
            current_buffer = metadata.prev_buffer;
        }
    }

    memory_resource* monotonic_buffer_resource::upstream_resource() const { return upstream_rsrc; }

    void* monotonic_buffer_resource::do_allocate(std::size_t bytes, std::size_t alignment) {
        if (current_buffer) {
            const struct metadata& metadata = *static_cast<struct metadata*>(current_buffer);
            std::size_t remaining_space = metadata.buffer_size - (static_cast<char*>(unused_buffer_part) - static_cast<char*>(current_buffer));
            if (void* addr = align(alignment, bytes, unused_buffer_part, remaining_space); addr) {
                unused_buffer_part = static_cast<void*>(static_cast<char*>(unused_buffer_part) + bytes);
                return addr;
            }
        }

        void* prev_buffer = current_buffer;
        while (next_buffer_size < bytes + alignment + sizeof(metadata)) {
            next_buffer_size *= growth_factor;
        }
        current_buffer = upstream_rsrc->allocate(next_buffer_size);
        
        *static_cast<metadata*>(current_buffer) = { .prev_buffer = prev_buffer, .buffer_size = next_buffer_size };
        std::size_t remaining_space = next_buffer_size - sizeof(metadata);
        next_buffer_size *= growth_factor;
        unused_buffer_part = static_cast<void*>(static_cast<char*>(current_buffer) + sizeof(metadata));

        void* const addr = align(alignment, bytes, unused_buffer_part, remaining_space);
        unused_buffer_part = static_cast<void*>(static_cast<char*>(unused_buffer_part) + bytes);
        return addr;
    }

    void monotonic_buffer_resource::do_deallocate(void*, size_t, size_t) {}
    bool monotonic_buffer_resource::do_is_equal(const memory_resource& other) const noexcept { return this == &other; }
}