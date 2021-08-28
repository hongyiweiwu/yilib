#pragma once

#include "memory.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "tuple.hpp"

namespace std {
    template<class OuterAlloc, class ...InnerAllocs>
    class scoped_allocator_adaptor : public OuterAlloc {
    private:
        using OuterTraits = allocator_traits<OuterAlloc>;

        template<class T>
        static constexpr auto outermost(T& x) noexcept {
            if constexpr (requires { x.outer_allocator(); }) {
                return x.outer_allocator();
            } else {
                return x;
            }
        }

        template<class T>
        using outermost_alloc_traits = allocator_traits<remove_reference_t<decltype(outermost(declval<T>()))>>;

        scoped_allocator_adaptor<InnerAllocs...> inner;

    public:
        using outer_allocator_type = OuterAlloc;
        using inner_allocator_type = conditional_t<sizeof...(InnerAllocs) == 0, scoped_allocator_adaptor<OuterAlloc>, scoped_allocator_adaptor<InnerAllocs...>>;

        using value_type = typename OuterTraits::value_type;
        using size_type = typename OuterTraits::size_type;
        using difference_type = typename OuterTraits::difference_type;
        using pointer = typename OuterTraits::pointer;
        using const_pointer = typename OuterTraits::const_pointer;
        using void_pointer = typename OuterTraits::void_pointer;
        using const_void_pointer = typename OuterTraits::const_void_pointer;

        using propagate_on_container_copy_assignment = bool_constant<allocator_traits<OuterAlloc>::propagate_on_container_copy_assignment::value
            || (allocator_traits<InnerAllocs>::propagate_on_container_copy_assignment::value || ...)>;

        using propagate_on_container_move_assignment = bool_constant<allocator_traits<OuterAlloc>::propagate_on_container_move_assignment::value
            || (allocator_traits<InnerAllocs>::propagate_on_container_move_assignment::value || ...)>;

        using propagate_on_container_swap = bool_constant<allocator_traits<OuterAlloc>::propagate_on_container_swap::value
            || (allocator_traits<InnerAllocs>::propagate_on_container_swap::value || ...)>;

        using is_always_equal = bool_constant<allocator_traits<OuterAlloc>::is_always_equal::value
            || (allocator_traits<InnerAllocs>::is_always_equal::value || ...)>;

        template<class Tp> struct rebind {
            using other = scoped_allocator_adaptor<typename OuterTraits::template rebind_alloc<Tp>, InnerAllocs...>;
        };

        scoped_allocator_adaptor() : OuterAlloc(), inner() {}

        template<class OuterA2> requires is_constructible_v<OuterAlloc, OuterA2>
        scoped_allocator_adaptor(OuterA2&& outerAlloc, const InnerAllocs& ...innerAllocs) noexcept
            : OuterAlloc(forward<OuterA2>(outerAlloc)), inner(innerAllocs...) {}

        scoped_allocator_adaptor(const scoped_allocator_adaptor& other) noexcept : OuterAlloc(other), inner(other.inner) {}

        scoped_allocator_adaptor(scoped_allocator_adaptor&& other) noexcept : OuterAlloc(move(other)), inner(move(other).inner) {}

        template<class OuterA2> requires is_constructible_v<OuterAlloc, const OuterA2&>
        scoped_allocator_adaptor(const scoped_allocator_adaptor<OuterA2, InnerAllocs...>& other) noexcept : OuterAlloc(other), inner(other.inner) {}

        template<class OuterA2> requires is_constructible_v<OuterAlloc, OuterA2>
        scoped_allocator_adaptor(scoped_allocator_adaptor<OuterA2, InnerAllocs...>&& other) noexcept : OuterAlloc(move(other)), inner(move(other).inner) {}

        scoped_allocator_adaptor& operator=(const scoped_allocator_adaptor&) = default;
        scoped_allocator_adaptor& operator=(scoped_allocator_adaptor&&) = default;

        ~scoped_allocator_adaptor() = default;

        inner_allocator_type& inner_allocator() noexcept {
            if constexpr (sizeof...(InnerAllocs) == 0) {
                return *this;
            } else {
                return inner;
            }
        }

        const inner_allocator_type& inner_allocator() const noexcept {
            if constexpr (sizeof...(InnerAllocs) == 0) {
                return *this;
            } else {
                return inner;
            }
        }

        outer_allocator_type& outer_allocator() noexcept {
            return static_cast<OuterAlloc&>(*this);
        }

        const outer_allocator_type& outer_allocator() const noexcept {
            return static_cast<const OuterAlloc&>(*this);
        }

        [[nodiscard]] pointer allocate(size_type n) {
            return OuterTraits::allocate(outer_allocator(), n);
        }

        [[nodiscard]] pointer allocate(size_type n, const_void_pointer hint) {
            return OuterTraits::allocate(outer_allocator(), n, hint);
        }

        void deallocate(pointer p, size_type n) {
            OuterTraits::deallocate(outer_allocator(), p, n);
        }

        size_type max_size() const {
            return OuterTraits::max_size(outer_allocator());
        }

        template<class T, class ...Args>
        void construct(T* p, Args&& ...args) {
            apply([p, this]<class ...NewArgs>(NewArgs&& ...newargs) {
                outermost_alloc_traits<decltype(*this)>::construct(outermost(*this), p, forward<NewArgs>(newargs)...);
            }, uses_allocator_construction_args<T>(inner_allocator(), forward<Args>(args)...));
        }

        template<class T>
        void destroy(T* p) {
            outermost_alloc_traits<decltype(*this)>::destroy(outermost(*this), p);
        }

        scoped_allocator_adaptor select_on_container_copy_construction() const {
            return scoped_allocator_adaptor(
                OuterTraits::select_on_container_copy_construction(),
                allocator_traits<InnerAllocs>::select_on_container_copy_construction()...);
        }
    };

    template<class OuterAlloc, class ...InnerAllocs>
    scoped_allocator_adaptor(OuterAlloc, InnerAllocs...) -> scoped_allocator_adaptor<OuterAlloc, InnerAllocs...>;

    template<class OuterA1, class OuterA2, class ...InnerAllocs>
    bool operator==(const scoped_allocator_adaptor<OuterA1, InnerAllocs...>& a, const scoped_allocator_adaptor<OuterA2, InnerAllocs...>& b) noexcept {
        if constexpr (sizeof...(InnerAllocs)) {
            return a.outer_allocator() == b.outer_allocator();
        } else {
            return a.outer_allocator() == b.outer_allocator() && a.inner_allocator() == b.inner_allocator();
        }
    }
}