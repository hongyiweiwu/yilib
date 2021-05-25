#pragma once

#include "cstddef.hpp"
#include "limits.hpp"
#include "new.hpp"
#include "utility.hpp"
#include "memory.hpp"

// TODO: Implement synchronized_pool_resource and unsynchronized_pool_resource.

namespace std::pmr {
    /* 20.12.2 Class memory_resource */
    class memory_resource {
    private:
        static constexpr size_t max_align = alignof(max_align_t);
    public:
        memory_resource() = default;
        memory_resource(const memory_resource&) = default;
        virtual ~memory_resource() = default;

        memory_resource& operator=(const memory_resource&) = default;
        
        [[nodiscard]] void* allocate(size_t bytes, size_t alignment = max_align);
        void deallocate(void* p, size_t bytes, size_t alignment = max_align);

        bool is_equal(const memory_resource& other) const noexcept;
    private:
        virtual void* do_allocate(size_t bytes, size_t alignment) = 0;
        virtual void do_deallocate(void* p, size_t bytes, size_t alignment) = 0;

        virtual bool do_is_equal(const memory_resource& other) const noexcept = 0;
    };

    bool operator==(const memory_resource& a, const memory_resource& b) noexcept;

    /* 20.12.3 Class template polymorphic_allocator */
    // Forward declaration. Formally declared below and implemented in the corresponding cpp file.
    memory_resource* get_default_resource() noexcept;
    
    template<class Tp = byte>
    class polymorphic_allocator {
    private:
        memory_resource* memory_rsrc;
    public:
        using value_type = Tp;

        polymorphic_allocator() noexcept : memory_rsrc(get_default_resource()) {}
        polymorphic_allocator(memory_resource* r) noexcept : memory_rsrc(r) {}

        polymorphic_allocator(const polymorphic_allocator& other) = default;

        template<class U>
        polymorphic_allocator(const polymorphic_allocator<U>& other) noexcept
            : memory_rsrc(other.resource()) {}

        polymorphic_allocator& operator=(const polymorphic_allocator&) = delete;

        [[nodiscard]] Tp* allocate(size_t n) {
            if (numeric_limits<size_t>::max() / sizeof(Tp) < n) {
                throw bad_array_new_length();
            } else {
                return static_cast<Tp*>(memory_rsrc->allocate(n * sizeof(Tp), alignof(Tp)));
            }
        }

        void deallocate(Tp* p, size_t n) {
            memory_rsrc->deallocate(p, n * sizeof(Tp), alignof(Tp));
        }

        [[nodiscard]] void* allocate_bytes(size_t nbytes, size_t alignment = alignof(max_align_t)) {
            return memory_rsrc->allocate(nbytes, alignment);
        }
        
        void deallocate_bytes(void* p, size_t nbytes, size_t alignment = alignof(max_align_t)) {
            memory_rsrc->deallocate(p, nbytes, alignment);
        }

        template<class T> [[nodiscard]] T* allocate_object(size_t n = 1) {
            if (numeric_limits<size_t>::max() / sizeof(T) < n) {
                throw bad_array_new_length();
            } else {
                return static_cast<T*>(allocate_bytes(n * sizeof(T), alignof(T)));
            }
        }

        template<class T> void deallocate_object(T* p, size_t n = 1) {
            deallocate_bytes(p, n * sizeof(T), alignof(T));
        }

        template<class T, class ...CtorArgs> [[nodiscard]] T* new_object(CtorArgs&& ...ctor_args) {
            T* p = allocate_object<T>();
            try {
                construct(p, forward<CtorArgs>(ctor_args)...);
            } catch (...) {
                deallocate_object(p);
                throw;
            }

            return p;
        }

        template<class T> void delete_object(T* p) {
            destroy(p);
            deallocate_object(p);
        }

        template<class T, class ...Args>
        void construct(T* p, Args&& ...args) requires requires { uninitialized_construct_using_allocator(p, *this, forward<Args>(args)...); } {
            uninitialized_construct_using_allocator(p, *this, forward<Args>(args)...);
        }

        template<class T> void destroy(T* p) { p->~T(); }

        polymorphic_allocator select_on_container_copy_construction() const { return polymorphic_allocator(); }

        memory_resource* resource() const { return memory_rsrc; }
    };

    template<class T1, class T2> bool operator==(const polymorphic_allocator<T1>& a, const polymorphic_allocator<T2>& b) noexcept {
        return *a.resource() == *b.resource();
    }

    /* 20.12.4 Access to program-wide memory_resource objects */
    memory_resource* new_delete_resource() noexcept;
    memory_resource* null_memory_resource() noexcept;
    memory_resource* set_default_resource(memory_resource* r) noexcept;
    memory_resource* get_default_resource() noexcept;

    /* 20.12.5 Pool resource classes */
    struct pool_options {
        size_t max_blocks_per_chunk = 0;
        size_t largest_required_pool_block = 0;
    };

    class monotonic_buffer_resource : public memory_resource {
    private:
        struct metadata {
            void* prev_buffer;
            size_t buffer_size;
        };

        memory_resource* upstream_rsrc;
        /* Pointer to the current buffer. Each buffer starts with sizeof(sunit_t) bytes storing the pointer to the previous buffer, or 0 for the first
         * buffer, and another sizeof(sunit_t) bytes storing the size of the current buffer. */
        void* current_buffer;
        size_t next_buffer_size;
        /* Marks the beginning of the unused portion of the current buffer. For a newly-allocated buffer, this points to 2 * sizeof(sunit_t) bytes after
         * the beginning of the buffer. */
        void* unused_buffer_part;

        // These values are used by GCC's implementation.
        static constexpr size_t default_buffer_size = 128 * sizeof(void*);
        static constexpr float growth_factor = 1.5;

    public:
        explicit monotonic_buffer_resource(memory_resource* upstream);
        monotonic_buffer_resource(size_t initial_size, memory_resource* upstream);
        monotonic_buffer_resource(void* buffer, size_t buffer_size, memory_resource* upstream);

        monotonic_buffer_resource();
        explicit monotonic_buffer_resource(size_t initial_size);
        monotonic_buffer_resource(void* buffer, size_t buffer_size);

        monotonic_buffer_resource(const monotonic_buffer_resource&) = delete;

        virtual ~monotonic_buffer_resource();

        monotonic_buffer_resource& operator=(const monotonic_buffer_resource&) = delete;

        void release();
        memory_resource* upstream_resource() const;

    protected:
        void* do_allocate(size_t bytes, size_t alignment) override;
        void do_deallocate(void* p, size_t bytes, size_t alignment) override;
        bool do_is_equal(const memory_resource& other) const noexcept override;
    };
}