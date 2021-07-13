#pragma once

#include "concepts.hpp"
#include "memory/pointer_util.hpp"
#include "iterator.hpp"
#include "type_traits.hpp"
#include "utility.hpp"

// TODO: Implement ranges:: algorithms & parallel algorithms.
namespace std {
    /* 25.11.2 Special memory concepts */
    namespace __internal {
        template<class I>
        concept no_throw_input_iterator = input_iterator<I> && is_lvalue_reference_v<iter_reference_t<I>>
            && same_as<remove_cvref_t<iter_reference_t<I>>, iter_value_t<I>>;

        template<class S, class I>
        concept no_throw_sentinel_for = sentinel_for<S, I>;

        /* TODO: Uncomment after implementing <ranges>.
        template<class R>
        concept no_throw_input_range = range<R> && no_throw_input_iterator<iterator_t<R>> && no_throw_sentinel_for<sentinel_t<R>, iterator_t<R>>; */
    
        template<class I>
        concept no_throw_forward_iterator = no_throw_input_iterator<I> && forward_iterator<I> && no_throw_sentinel_for<I, I>;

        /* TODO: Uncomment after implementing <ranges>.
        template<class R>
        concept no_throw_forward_range = no_throw_input_range<R> && no_throw_forward_iterator<iterator_t<R>>; */
    }

    /* 25.11.3 uninitialized_default_construct */
    template<__internal::legacy_forward_iterator ForwardIt>
    void uninitialized_default_construct(ForwardIt first, ForwardIt last) {
        for (; first != last; first++) {
            ::new (const_cast<void*>(static_cast<const volatile void*>(addressof(*first)))) typename iterator_traits<ForwardIt>::value_type;
        }
    }

    template<__internal::legacy_forward_iterator ForwardIt, class Size>
    ForwardIt uninitialized_default_construct_n(ForwardIt first, Size n) {
        for (; n > 0; first++, n--) {
            ::new (const_cast<void*>(static_cast<const volatile void*>(addressof(*first)))) typename iterator_traits<ForwardIt>::value_type;
        }
        return first;
    }

    /* 25.11.4 uninitialized_value_construct */
    template<__internal::legacy_forward_iterator ForwardIt>
    void uninitialized_value_construct(ForwardIt first, ForwardIt last) {
        for (; first != last; first++) {
            ::new (const_cast<void*>(static_cast<const volatile void*>(addressof(*first)))) typename iterator_traits<ForwardIt>::value_type();
        }
    }

    template<__internal::legacy_forward_iterator ForwardIt, class Size>
    ForwardIt uninitialized_value_construct_n(ForwardIt first, Size n) {
        for (; n > 0; first++, n--) {
            ::new (const_cast<void*>(static_cast<const volatile void*>(addressof(*first)))) typename iterator_traits<ForwardIt>::value_type();
        }
        return first;
    }

    /* 25.11.5 uninitialized_copy */
    template<__internal::legacy_input_iterator InputIt, __internal::legacy_forward_iterator ForwardIt>
    ForwardIt uninitialized_copy(InputIt first, InputIt last, ForwardIt result) {
        for (; first != last; result++, first++) {
            ::new (const_cast<void*>(static_cast<const volatile void*>(*result))) typename iterator_traits<ForwardIt>::value_type(*first);
        }
        return result;
    }

    template<__internal::legacy_input_iterator InputIt, class Size, __internal::legacy_forward_iterator ForwardIt>
    ForwardIt uninitialized_copy_n(InputIt first, Size n, ForwardIt result) {
        for (; n > 0; result++, first++, n--) {
            ::new (const_cast<void*>(static_cast<const volatile void*>(*result))) typename iterator_traits<ForwardIt>::value_type(*first);
        }
        return result;
    }

    /* 25.11.6 uninitialized_move */
    template<__internal::legacy_input_iterator InputIt, __internal::legacy_forward_iterator ForwardIt>
    ForwardIt uninitialized_move(InputIt first, InputIt last, ForwardIt result) {
        for (; first != last; result++, first++) {
            ::new (const_cast<void*>(static_cast<const volatile void*>(*result))) typename iterator_traits<ForwardIt>::value_type(*first);
        }
        return result;
    }

    template<__internal::legacy_input_iterator InputIt, class Size, __internal::legacy_forward_iterator ForwardIt>
    pair<InputIt, ForwardIt> uninitialized_move_n(InputIt first, Size n, ForwardIt result) {
        for (; n > 0; result++, first++, n--) {
            ::new (const_cast<void*>(static_cast<const volatile void*>(*result))) typename iterator_traits<ForwardIt>::value_type(*first);
        }
        return {first, result};
    }

    /* 25.11.7 uninitialized_fill */
    template<__internal::legacy_forward_iterator ForwardIt, class T>
    void uninitialized_fill(ForwardIt first, ForwardIt last, const T& x) {
        for (; first != last; first++) {
            ::new (const_cast<void*>(static_cast<const volatile void*>(*first))) typename iterator_traits<ForwardIt>::value_type(x);
        }
    }

    template<__internal::legacy_forward_iterator ForwardIt, class Size, class T>
    ForwardIt uninitialized_fill_n(ForwardIt first, Size n, const T& x) {
        for (; n--; first++) {
            ::new (const_cast<void*>(static_cast<const volatile void*>(*first))) typename iterator_traits<ForwardIt>::value_type(x);
        }
        return first;
    }

    /* 25.11.8 construct_at */
    template<class T, class ...Args> requires requires (void* ptr, Args&& ...args) { ::new (ptr) T(forward<Args>(args)...); }
    constexpr T* construct_at(T* p, Args&& ...args) {
        return ::new (const_cast<void*>(static_cast<const volatile void*>(p))) T(forward<Args>(args)...);
    }

    namespace ranges {
        namespace __construct_at_impl {
            struct construct_at_fn {
                template<class T, class ...Args> requires requires (void* ptr, Args&& ...args) { ::new (ptr) T(forward<Args>(args)...); }
                constexpr T* operator()(T* p, Args&& ...args) const {
                    return std::construct_at(p, forward<Args>(args)...);
                }
            };
        }

        inline constexpr __construct_at_impl::construct_at_fn construct_at;
    }

    /* 25.11.9 destroy */
    template<class T> constexpr void destroy_at(T* location) {
        if constexpr (is_array<T>::value) {
            // Equivalent to destroy(begin(*location), end(*location)).
            for (T& elem: *location) {
                destroy_at(addressof(elem));
            }
        } else {
            location->~T();
        }
    }
    
    namespace ranges {
        namespace __destroy_at_impl {
            struct destroy_at_fn {
                template<destructible T> constexpr void destroy_at(T* location) noexcept {
                    return std::destroy_at(location);
                }
            };
        }

        inline constexpr __destroy_at_impl::destroy_at_fn destroy_at;
    }

    template<__internal::no_throw_forward_iterator ForwardIt>
    constexpr void destroy(ForwardIt first, ForwardIt last) {
        for (; first != last; first++) {
            destroy_at(addressof(*first));
        }
    }

    template<__internal::no_throw_forward_iterator ForwardIt, class Size>
    constexpr ForwardIt destroy_n(ForwardIt first, Size n) {
        for (; n > 0; first++, n--) {
            destroy_at(addressof(*first));
        }
        return first;
    }
}