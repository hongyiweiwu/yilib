#pragma once

#include "utility.hpp"
#include "type_traits.hpp"
#include "memory/pointer_util.hpp"
#include "concepts.hpp"

namespace std {
    /* 20.14.5 Function template invoke */
    template<class F, class ...Args> 
    constexpr invoke_result_t<F, Args...> invoke(F&& f, Args&& ...args) noexcept(is_nothrow_invocable_v<F, Args...>) {
        return __internal::__INVOKE(forward<F>(f), forward<Args>(args)...);
    }

    /* 20.14.6 reference_wrapper */
    template<class T> class reference_wrapper {
    public:
        using type = T;

        template<class U> requires requires (void (*fun)(T&)) { fun(declval<U>()); } && (!is_same_v<remove_cvref_t<U>, reference_wrapper>)
        constexpr reference_wrapper(U&& u) noexcept(noexcept(declval<void (*)(T&)>()(declval<U>())))
            : ptr(addressof(([](T& t) -> decltype(auto) { return t; })(forward<U>(u)))) {}

        constexpr reference_wrapper(const reference_wrapper& x) noexcept = default;

        constexpr reference_wrapper& operator=(const reference_wrapper& x) noexcept = default;

        constexpr operator T& () const noexcept { return *ptr; }
        constexpr T& get() const noexcept { return *ptr; }

        template<class ...ArgTypes> requires __internal::is_complete<T>::value
        constexpr invoke_result_t<T&, ArgTypes...> operator()(ArgTypes&&... args) const {
            return invoke(get(), forward<ArgTypes>(args)...);
        }

    private:
        T* ptr;
    };

    template<class T> reference_wrapper(T&) -> reference_wrapper<T>;

    template<class T> constexpr reference_wrapper<T> ref(T& t) noexcept { return reference_wrapper<T>(t); }
    template<class T> constexpr reference_wrapper<T> ref(reference_wrapper<T> t) noexcept {
        return ref(t.get());
    }

    template<class T> constexpr reference_wrapper<const T> ref(const T& t) noexcept { return reference_wrapper<const T>(t); }
    template<class T> constexpr reference_wrapper<const T> ref(reference_wrapper<T> t) noexcept {
        return cref(t.get());
    }

    /* 20.14.8 Comparisons */
    template<class T = void> struct less {
        constexpr bool operator()(const T& x, const T& y) const { return x < y; }
    };

    template<> struct less<void> {
        template<class T, class U> constexpr auto operator()(T&& t, U&& u) const {
            return forward<T>(t) < forward<U>(u);
        }
    };

    /* 20.14.12 Class identity */
    struct identity {
        template<class T>
        constexpr T&& operator()(T&& t) const noexcept {
            return forward<T>(t);
        }

        using is_transparent = void;
    };

    /* 20.14.19 Class template hash */
    namespace __internal {
        /* Used purely to specify a specialization of std::hash that represents when std::hash is supposed to be deleted. All deleted std::hash specializations
         * should inherit from std::hash<__delete_hash_t> and automatically receive all the deleted std::hash properties. */
        struct __deleted_hash_t {
            explicit __deleted_hash_t() = default;
        };
    }

    // Forward declaration.
    template<class T> struct hash;

    template<> struct hash<__internal::__deleted_hash_t> {
        hash() = delete;
        hash(hash&&) = delete;
        hash& operator=(hash&&) = delete;
        hash(const hash&) = delete;
        hash& operator=(const hash&) = delete;
    };

    template<class T> struct hash : hash<__internal::__deleted_hash_t> {};

    /* 20.14.9 Concept-constrained comparisons */
    namespace ranges {
        struct less {
            template<class T, class U> requires totally_ordered_with<T, U> || requires { declval<T>() < declval<U>(); }
            constexpr bool operator()(T&& t, U&& u) const {
                return forward<T>(t) < forward<U>(u);
            }
        };
    }
}