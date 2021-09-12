#pragma once

#include "type_traits/is_constructible.hpp"
#include "utility.hpp"
#include "type_traits.hpp"
#include "memory/pointer_util.hpp"
#include "concepts.hpp"
#include "compare.hpp"
#include "tuple.hpp"

namespace std {
    /* 20.14.5 Function template invoke */
    template<class F, class ...Args> 
    constexpr invoke_result_t<F, Args...>
    invoke(F&& f, Args&& ...args) noexcept(is_nothrow_invocable_v<F, Args...>) {
        return __internal::__INVOKE(static_cast<int*>(nullptr), forward<F>(f), forward<Args>(args)...);
    }

    /* 20.14.6 reference_wrapper */
    template<class T> class reference_wrapper {
    public:
        using type = T;

        template<class U> 
        requires requires (void (*fun)(T&)) { fun(declval<U>()); } && (!is_same_v<remove_cvref_t<U>, reference_wrapper>)
        constexpr reference_wrapper(U&& u) noexcept(noexcept(declval<void (*)(T&)>()(declval<U>())))
            : ptr(addressof(([](T& t) -> decltype(auto) { return t; })(forward<U>(u)))) {}

        constexpr reference_wrapper(const reference_wrapper& x) noexcept = default;

        constexpr reference_wrapper& operator=(const reference_wrapper& x) noexcept = default;

        constexpr operator T& () const noexcept { 
            return *ptr; 
        }

        constexpr T& get() const noexcept { 
            return *ptr; 
        }

        template<class ...ArgTypes> 
        requires __internal::is_complete<T>::value
        constexpr invoke_result_t<T&, ArgTypes...> operator()(ArgTypes&&... args) const {
            return invoke(get(), forward<ArgTypes>(args)...);
        }

    private:
        T* ptr;
    };

    template<class T> 
    reference_wrapper(T&) -> reference_wrapper<T>;

    template<class T> 
    constexpr reference_wrapper<T> ref(T& t) noexcept { 
        return reference_wrapper<T>(t); 
    }

    template<class T> 
    constexpr reference_wrapper<const T> cref(const T& t) noexcept { 
        return reference_wrapper<const T>(t); 
    }

    template<class T>
    void ref(const T&&) = delete;

    template<class T>
    void cref(const T&&) = delete;

    template<class T> 
    constexpr reference_wrapper<T> ref(reference_wrapper<T> t) noexcept {
        return ref(t.get());
    }

    template<class T> 
    constexpr reference_wrapper<const T> cref(reference_wrapper<T> t) noexcept {
        return cref(t.get());
    }
    
    /* 20.14.7 Arithmetic operations */
    template<class T = void>
    struct plus {
        constexpr T operator()(const T& x, const T& y) const {
            return x + y;
        }
    };

    template<class T = void>
    struct minus {
        constexpr T operator()(const T& x, const T& y) const {
            return x - y;
        }
    };

    template<class T = void>
    struct multiplies {
        constexpr T operator()(const T& x, const T& y) const {
            return x * y;
        }
    };

    template<class T = void>
    struct divides {
        constexpr T operator()(const T& x, const T& y) const {
            return x / y;
        }
    };

    template<class T = void>
    struct modulus {
        constexpr T operator()(const T& x, const T& y) const {
            return x % y;
        }
    };

    template<class T = void>
    struct negate {
        constexpr T operator()(const T& x) const {
            return -x;
        }
    };

    template<>
    struct plus<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t, U&& u) const -> decltype(forward<T>(t) + forward<U>(u)) {
            return forward<T>(t) + forward<U>(u);
        }

        using is_transparent = void;
    };

    template<>
    struct minus<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t, U&& u) const -> decltype(forward<T>(t) - forward<U>(u)) {
            return forward<T>(t) - forward<U>(u);
        }

        using is_transparent = void;
    };

    template<>
    struct multiplies<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t, U&& u) const -> decltype(forward<T>(t) * forward<U>(u)) {
            return forward<T>(t) * forward<U>(u);
        }

        using is_transparent = void;
    };

    template<>
    struct divides<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t, U&& u) const -> decltype(forward<T>(t) / forward<U>(u)) {
            return forward<T>(t) / forward<U>(u);
        }

        using is_transparent = void;
    };

    template<>
    struct modulus<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t, U&& u) const -> decltype(forward<T>(t) % forward<U>(u)) {
            return forward<T>(t) % forward<U>(u);
        }

        using is_transparent = void;
    };

    template<>
    struct negate<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t) const -> decltype(-forward<T>(t)) {
            return -forward<T>(t);
        }

        using is_transparent = void;
    };

    /* 20.14.8 Comparisons */
    template<class T = void> 
    struct equal_to {
        constexpr bool operator()(const T& x, const T& y) const { 
            return x == y; 
        }
    };

    template<class T = void> 
    struct not_equal_to {
        constexpr bool operator()(const T& x, const T& y) const { 
            return x != y; 
        }
    };

    template<class T = void> 
    struct greater {
        constexpr bool operator()(const T& x, const T& y) const { 
            return x > y; 
        }
    };

    template<class T = void> 
    struct less {
        constexpr bool operator()(const T& x, const T& y) const { 
            return x < y; 
        }
    };

    template<class T = void> 
    struct greater_equal {
        constexpr bool operator()(const T& x, const T& y) const { 
            return x >= y; 
        }
    };

    template<class T = void> 
    struct less_equal {
        constexpr bool operator()(const T& x, const T& y) const { 
            return x <= y; 
        }
    };

    template<>
    struct equal_to<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t, U&& u) const -> decltype(forward<T>(t) == forward<U>(u)) {
            return forward<T>(t) == forward<U>(u);
        }

        using is_transparent = void;
    };

    template<>
    struct not_equal_to<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t, U&& u) const -> decltype(forward<T>(t) != forward<U>(u)) {
            return forward<T>(t) != forward<U>(u);
        }

        using is_transparent = void;
    };

    template<>
    struct greater<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t, U&& u) const -> decltype(forward<T>(t) > forward<U>(u)) {
            return forward<T>(t) > forward<U>(u);
        }

        using is_transparent = void;
    };

    template<>
    struct less<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t, U&& u) const -> decltype(forward<T>(t) < forward<U>(u)) {
            return forward<T>(t) < forward<U>(u);
        }

        using is_transparent = void;
    };

    template<>
    struct greater_equal<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t, U&& u) const -> decltype(forward<T>(t) >= forward<U>(u)) {
            return forward<T>(t) >= forward<U>(u);
        }

        using is_transparent = void;
    };

    template<>
    struct less_equal<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t, U&& u) const -> decltype(forward<T>(t) <= forward<U>(u)) {
            return forward<T>(t) <= forward<U>(u);
        }

        using is_transparent = void;
    };

    // Defined in "compare.hpp".
    struct compare_three_way;

    /* 20.14.10 Logical operations */
    template<class T = void> 
    struct logical_and {
        constexpr bool operator()(const T& x, const T& y) const { 
            return x && y; 
        }
    };

    template<class T = void> 
    struct logical_or {
        constexpr bool operator()(const T& x, const T& y) const { 
            return x || y; 
        }
    };

    template<class T = void> 
    struct logical_not {
        constexpr bool operator()(const T& x) const { 
            return !x;
        }
    };

    template<>
    struct logical_and<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t, U&& u) const -> decltype(forward<T>(t) && forward<U>(u)) {
            return forward<T>(t) && forward<U>(u);
        }

        using is_transparent = void;
    };

    template<>
    struct logical_or<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t, U&& u) const -> decltype(forward<T>(t) || forward<U>(u)) {
            return forward<T>(t) || forward<U>(u);
        }

        using is_transparent = void;
    };

    template<>
    struct logical_not<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t) const -> decltype(!forward<T>(t)) {
            return !forward<T>(t);
        }

        using is_transparent = void;
    };

    /* 20.14.11 Bitwise operations */
    template<class T = void> 
    struct bit_and {
        constexpr bool operator()(const T& x, const T& y) const { 
            return x & y; 
        }
    };

    template<class T = void> 
    struct bit_or {
        constexpr bool operator()(const T& x, const T& y) const { 
            return x | y; 
        }
    };

    template<class T = void> 
    struct bit_xor {
        constexpr bool operator()(const T& x, const T& y) const { 
            return x ^ y; 
        }
    };

    template<class T = void> 
    struct bit_not {
        constexpr bool operator()(const T& x) const { 
            return ~x;
        }
    };

    template<>
    struct bit_and<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t, U&& u) const -> decltype(forward<T>(t) & forward<U>(u)) {
            return forward<T>(t) & forward<U>(u);
        }

        using is_transparent = void;
    };

    template<>
    struct bit_or<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t, U&& u) const -> decltype(forward<T>(t) | forward<U>(u)) {
            return forward<T>(t) | forward<U>(u);
        }

        using is_transparent = void;
    };

    template<>
    struct bit_xor<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t, U&& u) const -> decltype(forward<T>(t) ^ forward<U>(u)) {
            return forward<T>(t) ^ forward<U>(u);
        }

        using is_transparent = void;
    };    

    template<>
    struct bit_not<void> {
        template<class T, class U>
        constexpr auto operator()(T&& t) const -> decltype(~forward<T>(t)) {
            return ~forward<T>(t);
        }

        using is_transparent = void;
    };

    /* 20.14.12 Class identity */
    struct identity {
        template<class T>
        constexpr T&& operator()(T&& t) const noexcept {
            return forward<T>(t);
        }

        using is_transparent = void;
    };

    /* 20.14.13 Function template not_fn */
    namespace __internal {
        template<class F>
        struct __not_fn_t {
            decay_t<F> f;

            template<class ...Args>
            constexpr decltype(!declval<invoke_result_t<decay_t<F>&, Args...>>()) 
            operator()(Args&& ...args) & noexcept(noexcept(!invoke(f, forward<Args>(args)...))) {
                return !invoke(f, forward<Args>(args)...);
            }

            template<class ...Args>
            constexpr decltype(!declval<invoke_result_t<decay_t<F> const&, Args...>>()) 
            operator()(Args&& ...args) const& noexcept(noexcept(!invoke(f, forward<Args>(args)...))) {
                return !invoke(f, forward<Args>(args)...);
            }

            template<class ...Args>
            constexpr decltype(!declval<invoke_result_t<decay_t<F>, Args...>>()) 
            operator()(Args&& ...args) && noexcept(noexcept(!invoke(f, forward<Args>(args)...))) {
                return !invoke(move(f), forward<Args>(args)...);
            }

            template<class ...Args>
            constexpr decltype(!declval<invoke_result_t<decay_t<F> const, Args...>>()) 
            operator()(Args&& ...args) const&& noexcept(noexcept(!invoke(f, forward<Args>(args)...))) {
                return !invoke(move(f), forward<Args>(args)...);
            }
        };
    }

    template<class F>
    requires is_constructible_v<decay_t<F>, F> && is_move_constructible_v<decay_t<F>>
    constexpr __internal::__not_fn_t<F> not_fn(F&& f) {
        return { forward<F>(f) };
    }

    /* 20.14.14 Function object bind_front */
    namespace __internal {
        template<class F, class ...Args>
        struct __bind_front_t {
            decay_t<F> f;
            tuple<decay_t<Args>...> args;

            __bind_front_t(F&& f, Args&& ...args) : f(forward<F>(f)), args(forward<Args>(args)...) {}

            __bind_front_t(const __bind_front_t& other) 
            requires is_copy_constructible_v<decay_t<F>> && (is_copy_constructible_v<decay_t<Args>> && ...) = default;

            __bind_front_t(__bind_front_t&& other) = default;

            template<class ...CallArgs>
            constexpr decltype(!declval<invoke_result_t<decay_t<F>&, decay_t<Args>&..., CallArgs...>>()) 
            operator()(CallArgs&& ...call_args) & noexcept(noexcept(!invoke(f, declval<decay_t<Args>&>()..., forward<CallArgs>(call_args)...))) {
                constexpr auto helper = []<std::size_t ...N>(decay_t<F>& f, tuple<decay_t<Args>...>& args, CallArgs&& ...call_args, index_sequence<N...>) {
                    return !invoke(f, get<N>(args)..., forward<CallArgs>(call_args)...);
                };

                return helper(f, args, forward<CallArgs>(call_args)..., make_index_sequence<sizeof...(Args)>{});
            }

            template<class ...CallArgs>
            constexpr decltype(!declval<invoke_result_t<const decay_t<F>&, const decay_t<Args>&..., CallArgs...>>()) 
            operator()(CallArgs&& ...call_args) const& noexcept(noexcept(!invoke(f, declval<const decay_t<Args>&>()..., forward<CallArgs>(call_args)...))) {
                constexpr auto helper = []<std::size_t ...N>(const decay_t<F>& f, const tuple<decay_t<Args>...>& args, CallArgs&& ...call_args, index_sequence<N...>) {
                    return !invoke(f, get<N>(args)..., forward<CallArgs>(call_args)...);
                };
                
                return helper(f, args, forward<CallArgs>(call_args)..., make_index_sequence<sizeof...(Args)>{});
            }

            template<class ...CallArgs>
            constexpr decltype(!declval<invoke_result_t<decay_t<F>, Args..., CallArgs...>>()) 
            operator()(CallArgs&& ...call_args) && noexcept(noexcept(!invoke(move(f), declval<decay_t<Args>>()..., forward<CallArgs>(call_args)...))) {
                constexpr auto helper = []<std::size_t ...N>(decay_t<F>&& f, tuple<decay_t<Args>...>&& args, CallArgs&& ...call_args, index_sequence<N...>) {
                    return !invoke(move(f), get<N>(move(args))..., forward<CallArgs>(call_args)...);
                };
                
                return helper(move(f), move(args), forward<CallArgs>(call_args)..., make_index_sequence<sizeof...(Args)>{});
            }

            template<class ...CallArgs>
            constexpr decltype(!declval<invoke_result_t<const decay_t<F>, const decay_t<Args>..., CallArgs...>>()) 
            operator()(CallArgs&& ...call_args) const&& noexcept(noexcept(!invoke(move(f), declval<const decay_t<Args>>()..., forward<CallArgs>(call_args)...))) {
                constexpr auto helper = []<std::size_t ...N>(const decay_t<F>&& f, const tuple<decay_t<Args>...>&& args, CallArgs&& ...call_args, index_sequence<N...>) {
                    return !invoke(move(f), get<N>(move(args))..., forward<CallArgs>(call_args)...);
                };
                
                return helper(move(f), move(args), forward<CallArgs>(call_args)..., make_index_sequence<sizeof...(Args)>{});
            }
        };
    }

    template<class F, class ...Args>
    requires is_constructible_v<decay_t<F>, F> && is_move_constructible_v<decay_t<F>>
        && (is_constructible_v<decay_t<Args>, Args> && ...)
        && (is_move_constructible_v<decay_t<Args>> && ...)
    constexpr __internal::__bind_front_t<F, Args...> bind_front(F&& f, Args&& ...args) {
        return { forward<F>(f), forward<Args>(args)... };
    }

    /* 20.14.19 Class template hash */
    namespace __internal {
        /* Used purely to specify a specialization of std::hash that represents when std::hash is supposed to be deleted. All deleted std::hash specializations
         * should inherit from std::hash<__delete_hash_t> and automatically receive all the deleted std::hash properties. */
        struct __deleted_hash_t {
            explicit __deleted_hash_t() = default;
        };

        /* Used purely to specify a specialization of std::hash that should be enabled. All std::hash specializations that are enabled should inherit from
         * hash<__enabled_hash_t> publicly, which contains a generic hashing algorithm. */
        struct __enabled_hash_t {
            explicit __enabled_hash_t() = default;
        };
    }

    // Forward declaration, declare below.
    template<class T> 
    struct hash;

    template<> 
    struct hash<__internal::__deleted_hash_t> {
        hash() = delete;
        hash(hash&&) = delete;
        hash& operator=(hash&&) = delete;
        hash(const hash&) = delete;
        hash& operator=(const hash&) = delete;
    };

    template<> 
    struct hash<__internal::__enabled_hash_t> {
    protected:
        std::size_t hash_bytes(const void* ptr, std::size_t len, std::size_t seed = std::size_t(0xc70f6907UL)) const noexcept;
    };

    template<class T> 
    struct hash : hash<__internal::__deleted_hash_t> {};

    template<> 
    struct hash<bool> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const bool& key) const noexcept;
    };

    template<> 
    struct hash<char> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const char& key) const noexcept;
    };

    template<> 
    struct hash<signed char> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const signed char& key) const noexcept;
    };

    template<> 
    struct hash<unsigned char> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const unsigned char& key) const noexcept;
    };

    template<> 
    struct hash<char8_t> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const char8_t& key) const noexcept;
    };

    template<> 
    struct hash<char16_t> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const char16_t& key) const noexcept;
    };

    template<> 
    struct hash<char32_t> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const char32_t& key) const noexcept;
    };

    template<> 
    struct hash<wchar_t> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const wchar_t& key) const noexcept;
    };

    template<> 
    struct hash<short> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const short& key) const noexcept;
    };

    template<> 
    struct hash<unsigned short> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const unsigned short& key) const noexcept;
    };

    template<> 
    struct hash<int> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const int& key) const noexcept;
    };

    template<> 
    struct hash<unsigned int> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const unsigned int& key) const noexcept;
    };

    template<> 
    struct hash<long> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const long& key) const noexcept;
    };

    template<> 
    struct hash<long long> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const long long& key) const noexcept;
    };

    template<> 
    struct hash<unsigned long> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const unsigned long& key) const noexcept;
    };

    template<> 
    struct hash<unsigned long long> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const unsigned long long& key) const noexcept;
    };

    template<> 
    struct hash<float> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const float& key) const noexcept;
    };

    template<> 
    struct hash<double> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const double& key) const noexcept;
    };

    template<> 
    struct hash<long double> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const long double& key) const noexcept;
    };

    template<> 
    struct hash<std::nullptr_t> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const std::nullptr_t& key) const noexcept;
    };
    
    template<class T> 
    struct hash<T*> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(T* const& key) const noexcept {
            return reinterpret_cast<std::size_t>(key);
        }
    };

    /* 20.14.9 Concept-constrained comparisons */
    namespace ranges {
        struct equal_to {
            template<class T, class U> 
            requires totally_ordered_with<T, U> || __internal::builtin_ptr_equal<T, U>
            constexpr bool operator()(T&& t, U&& u) const {
                if constexpr (__internal::builtin_ptr_equal<T, U>) {
                    return static_cast<const volatile void*>(t) == static_cast<const volatile void*>(u);
                } else {
                    return forward<T>(t) == forward<U>(u);
                }
            }

            using is_transparent = void;
        };

        struct not_equal_to {
            template<class T, class U> 
            requires totally_ordered_with<T, U> || __internal::builtin_ptr_unequal<T, U>
            constexpr bool operator()(T&& t, U&& u) const {
                if constexpr (__internal::builtin_ptr_unequal<T, U>) {
                    return static_cast<const volatile void*>(t) != static_cast<const volatile void*>(u);
                } else {
                    return forward<T>(t) != forward<U>(u);
                }
            }

            using is_transparent = void;
        };

        struct greater {
            template<class T, class U> 
            requires totally_ordered_with<T, U> || __internal::builtin_ptr_greater<T, U>
            constexpr bool operator()(T&& t, U&& u) const {
                if constexpr (__internal::builtin_ptr_greater<T, U>) {
                    return static_cast<const volatile void*>(t) > static_cast<const volatile void*>(u);
                } else {
                    return forward<T>(t) > forward<U>(u);
                }
            }

            using is_transparent = void;
        };

        struct less {
            template<class T, class U> 
            requires totally_ordered_with<T, U> || __internal::builtin_ptr_less<T, U>
            constexpr bool operator()(T&& t, U&& u) const {
                if constexpr (__internal::builtin_ptr_less<T, U>) {
                    return static_cast<const volatile void*>(t) < static_cast<const volatile void*>(u);
                } else {
                    return forward<T>(t) < forward<U>(u);
                }
            }

            using is_transparent = void;
        };

        struct greater_equal {
            template<class T, class U> 
            requires totally_ordered_with<T, U> || __internal::builtin_ptr_greater_or_equal<T, U>
            constexpr bool operator()(T&& t, U&& u) const {
                if constexpr (__internal::builtin_ptr_greater_or_equal<T, U>) {
                    return static_cast<const volatile void*>(t) >= static_cast<const volatile void*>(u);
                } else {
                    return forward<T>(t) >= forward<U>(u);
                }
            }

            using is_transparent = void;
        };

        struct less_equal {
            template<class T, class U> 
            requires totally_ordered_with<T, U> || __internal::builtin_ptr_less_or_equal<T, U>
            constexpr bool operator()(T&& t, U&& u) const {
                if constexpr (__internal::builtin_ptr_less_or_equal<T, U>) {
                    return static_cast<const volatile void*>(t) <= static_cast<const volatile void*>(u);
                } else {
                    return forward<T>(t) <= forward<U>(u);
                }
            }

            using is_transparent = void;
        };
    }
}