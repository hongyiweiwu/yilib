#pragma once

#include "type_traits.hpp"
#include "util/utility_traits.hpp"
#include "utility.hpp"
#include "exception.hpp"
#include "compare.hpp"
#include "functional.hpp"
#include "array.hpp"
#include "memory.hpp"

namespace std {
    /* 20.7.3 Class template variant */
    template<class... Types>
    requires (sizeof...(Types) > 0)
    class variant {
    private:
        aligned_union_t<0, Types...> storage;
        std::size_t curr;

    public:
        constexpr variant() noexcept(noexcept(typename __internal::pick_ith_type<0, Types...>::type()))
        requires is_default_constructible_v<typename __internal::pick_ith_type<0, Types...>::type> : curr(0) {
            construct_at(__get_storage<typename __internal::pick_ith_type<0, Types...>::type>());
        }

        constexpr variant(const variant& other)
        requires (is_trivially_copy_constructible_v<Types> && ...) = default;

        constexpr variant(const variant& other)
        noexcept(noexcept((Types(declval<const Types&>()) && ...)))
        requires (is_copy_constructible_v<Types> && ...) && (!is_trivially_copy_constructible_v<Types> || ...) : curr(other.curr) {
            constexpr auto helper = [&]<class T, class ...Tn>(std::size_t idx, __internal::type_list<Tn...>, const auto& helper) {
                if (idx != 0) {
                    helper(idx - 1, __internal::type_list<Tn...>{}, helper);
                } else {
                    construct_at(__get_storage<T>(), *other.template __get_storage<T>());
                }
            };

            helper(curr, __internal::type_list<Types...>{}, helper);
        }

        constexpr variant(variant&& other)
        requires (is_trivially_move_constructible_v<Types> && ...) = default;

        constexpr variant(variant&& other)
        noexcept(noexcept((Types(declval<Types>()) && ...)))
        requires (is_move_constructible_v<Types> && ...) && (!is_trivially_move_constructible_v<Types> || ...) : curr(other.curr) {
            constexpr auto helper = [&]<class T, class ...Tn>(std::size_t idx, __internal::type_list<Tn...>, const auto& helper) {
                if (idx != 0) {
                    helper(idx - 1, __internal::type_list<Tn...>{}, helper);
                } else {
                    construct_at(__get_storage<T>(), move(*other.template __get_storage<T>()));
                }
            };

            helper(curr, __internal::type_list<Types...>{}, helper);
        }

    private:
        /* Tests for the alternative type closest to a type `T` through overload resolution. Each alternative `Ti` is given a function with signature `Ti FUN(Ti)`. The struct attempts
         * to call `FUN` with an element of type `T`. The type corresponding to the function that's chosen in the overload resolution is determined as the closest alternative and provided
         * in the `type` alias. */
        struct closest_alternative {
            template<class Ti, class T>
            struct test_base {};

            template<class Ti, class T>
            requires requires (void (*fn)(T (&&arr)[1])) { fn({declval<T>()}); }
            struct test_base<Ti, T> {
                constexpr Ti operator()(Ti) const noexcept;
            };

            template<class T>
            struct test : public test_base<Types, T>... {
                using test_base<Types, T>::operator()...;
            };

            template<class T>
            using type = decltype(test<T>()(declval<T>()));
        };

        /* Returns whether a given type is a specialization of `in_place_type_t`. */
        template<class T>
        struct is_specialization_of_in_place_type : public false_type {};

        template<class T>
        struct is_specialization_of_in_place_type<in_place_type_t<T>> : public true_type {};

        /* Returns whether a given type is a specialization of `in_place_index_t`. */
        template<class T>
        struct is_specialization_of_in_place_index : public false_type {};

        template<std::size_t I>
        struct is_specialization_of_in_place_index<in_place_index_t<I>> : public true_type {};

    public:
        template<class T>
        requires (!is_same_v<remove_cvref_t<T>, variant>) && (!is_specialization_of_in_place_type<remove_cvref_t<T>>::value)
            && (!is_specialization_of_in_place_index<remove_cvref_t<T>>::value) && requires (T&& t, const typename closest_alternative::template test<T>& test) { test(forward<T>(t)); }
            && is_constructible_v<typename closest_alternative::template type<T>, T>
        constexpr variant(T&& t) noexcept : curr(__internal::get_index_of<0, typename closest_alternative::template type<T>, Types...>::value) {
            construct_at(__get_storage<T>(), forward<T>(t));
        }

    public:
        template<class T, class ...Args>
        requires __internal::appears_exactly_i_times<1, T, Types...>::value && is_constructible_v<T, Args...>
        constexpr explicit variant(in_place_type_t<T>, Args&&... args) : curr(__internal::get_index_of<0, T, Types...>::value) {
            construct_at(__get_storage<T>(), forward<Args>(args)...);
        }

        template<class T, class U, class ...Args>
        requires __internal::appears_exactly_i_times<1, T, Types...>::value && is_constructible_v<T, initializer_list<U>, Args...>
        constexpr explicit variant(in_place_type_t<T>, initializer_list<U> il, Args&&... args) : curr(__internal::get_index_of<0, T, Types...>::value) {
            construct_at(__get_storage<T>(), il, forward<Args>(args)...);
        }

        template<std::size_t I, class ...Args>
        requires (I < sizeof...(Types)) && is_constructible_v<typename __internal::pick_ith_type<I, Types...>::type, Args...>
        constexpr explicit variant(in_place_index_t<I>, Args&&... args) {
            using T = typename __internal::pick_ith_type<I, Types...>::type;
            construct_at(__get_storage<T>(), forward<Args>(args)...);
        }

        template<std::size_t I, class U, class ...Args>
        requires (I < sizeof...(Types)) && is_constructible_v<typename __internal::pick_ith_type<I, Types...>::type, initializer_list<U>, Args...>
        constexpr explicit variant(in_place_index_t<I>, initializer_list<U> il, Args&&... args) {
            using T = typename __internal::pick_ith_type<I, Types...>::type;
            construct_at(__get_storage<T>(), il, forward<Args>(args)...);
        }

        constexpr ~variant()
        requires (is_trivially_destructible_v<Types> && ...) = default;

    public:
        constexpr ~variant()
        requires (!is_trivially_destructible_v<Types> || ...) {
            if (!valueless_by_exception()) {
                constexpr auto helper = [&]<class T, class ...Tn>(std::size_t idx, __internal::type_list<Tn...>, const auto& helper) {
                    if (idx != 0) {
                        helper(idx - 1, __internal::type_list<Tn...>{}, helper);
                    } else {
                        destroy_at(__get_storage<T>());
                    }
                };

                helper(curr, __internal::type_list<Types...>{}, helper);
                curr = std::size_t(-1);
            }
        }

        constexpr variant& operator=(const variant& other)
        requires (is_trivially_copy_constructible_v<Types> && ...) && (is_trivially_copy_assignable_v<Types> && ...) = default;

        constexpr variant& operator=(const variant& other)
        requires (is_copy_constructible_v<Types> && ...) && (is_copy_assignable_v<Types> && ...)
            && ((!is_trivially_copy_constructible_v<Types> || ...) || (!is_trivially_copy_assignable_v<Types> || ...)) {
            if (valueless_by_exception() && other.valueless_by_exception()) {
                return *this;
            } else if (!valueless_by_exception() && other.valueless_by_exception()) {
                ~variant();
            } else if (index() == other.index()) {
                constexpr auto helper = [&]<class T, class ...Tn>(std::size_t idx, __internal::type_list<Tn...>, const auto& helper) {
                    if (idx != 0) {
                        helper(idx - 1, __internal::type_list<Tn...>{}, helper);
                    } else {
                        *__get_storage<T>() = *other.template __get_storage<T>();
                    }
                };

                helper(index(), __internal::type_list<Types...>{}, helper);
            } else {
                constexpr auto helper = [&]<class T, class ...Tn>(std::size_t idx, __internal::type_list<Tn...>, const auto& helper) {
                    if (idx != 0) {
                        helper(idx - 1, __internal::type_list<Tn...>{}, helper);
                    } else if constexpr (is_nothrow_copy_constructible_v<T> || !is_nothrow_move_constructible_v<T>) {
                        emplace<T>(*other.template __get_storage<T>());
                    } else {
                        operator=(variant(other));
                    }
                };

                helper(other.index(), __internal::type_list<Types...>{}, helper);
            }

            return *this;
        }

        constexpr variant& operator=(variant&& other)
        requires (is_trivially_move_constructible_v<Types> && ...) && (is_trivially_move_assignable_v<Types> && ...) = default;

        constexpr variant& operator=(variant&& other)
        noexcept(noexcept((is_nothrow_move_constructible_v<Types> && ...) && (is_nothrow_move_assignable_v<Types> && ...)))
        requires (is_move_constructible_v<Types> && ...) && (is_move_assignable_v<Types> && ...)
            && ((!is_trivially_move_constructible_v<Types> || ...) || (!is_trivially_move_assignable_v<Types> || ...)) {
            if (valueless_by_exception() && other.valueless_by_exception()) {
                return *this;
            } else if (!valueless_by_exception() && other.valueless_by_exception()) {
                ~variant();
            } else {
                constexpr auto helper = [&]<class T, class ...Tn>(std::size_t idx, __internal::type_list<Tn...>, const auto& helper) {
                    if (idx != 0) {
                        helper(idx - 1, __internal::type_list<Tn...>{}, helper);
                    } else if (index() == other.index()) {
                        *__get_storage<T>() = move(*other.template __get_storage<T>());
                    } else {
                        emplace<T>(move(*other.template __get_storage<T>()));
                    }
                };

                helper(other.index(), __internal::type_list<Types...>{}, helper);
            }

            return *this;
        }

        template<class T>
        requires (!is_same_v<remove_cvref_t<T>, variant>) && requires (T&& t) { closest_alternative::template test<T>()(forward<T>(t)); }
            && is_assignable_v<typename closest_alternative::template type<T>&, T> && is_constructible_v<typename closest_alternative::template type<T>, T>
        constexpr variant& operator=(T&& t)
        noexcept(noexcept(is_nothrow_assignable_v<typename closest_alternative::template type<T>&, T>
            && is_nothrow_constructible_v<typename closest_alternative::template type<T>, T>)) {
            using Tj = typename closest_alternative::template type<T>;
            if (index() == __internal::get_index_of<0, Tj, Types...>::value) {
                *__get_storage<Tj>() = forward<T>(t);
            } else if constexpr (is_nothrow_constructible_v<Tj, T> || !is_nothrow_move_constructible_v<Tj>) {
                emplace<Tj>(forward<T>(t));
            } else {
                operator=(variant(forward<T>(t)));
            }

            return *this;
        }

        template<class T, class ...Args>
        requires __internal::appears_exactly_i_times<1, T, Types...>::value && is_constructible_v<T, Args...>
        constexpr T& emplace(Args&&... args) {
            ~variant();
            T* const storage = this->__get_storage<T>();
            construct_at(storage, forward<Args>(args)...);
            curr = __internal::get_index_of<0, T, Types...>::value;
            return *storage;
        }

        template<class T, class U, class ...Args>
        requires __internal::appears_exactly_i_times<1, T, Types...>::value && is_constructible_v<T, initializer_list<U>, Args...>
        constexpr T& emplace(initializer_list<U> il, Args&&... args) {
            ~variant();
            T* const storage = this->__get_storage<T>();
            construct_at(storage, il, forward<Args>(args)...);
            curr = __internal::get_index_of<0, T, Types...>::value;
            return *storage;
        }

        template<std::size_t I, class ...Args>
        requires (I < sizeof...(Types)) && is_constructible_v<typename __internal::pick_ith_type<I, Args...>::type, Args...>
        constexpr typename __internal::pick_ith_type<I, Args...>::type& emplace(Args&&... args) {
            return emplace<typename __internal::pick_ith_type<I, Args...>::type>(forward<Args>(args)...);
        }

        template<std::size_t I, class U, class ...Args>
        requires (I < sizeof...(Types)) && is_constructible_v<typename __internal::pick_ith_type<I, Args...>::type, initializer_list<U>&, Args...>
        constexpr typename __internal::pick_ith_type<I, Args...>::type& emplace(initializer_list<U> il, Args&&... args) {
            return emplace<typename __internal::pick_ith_type<I, Args...>::type>(il, forward<Args>(args)...);
        }

        constexpr bool valueless_by_exception() const noexcept {
            return curr == std::size_t(-1);
        }

        constexpr std::size_t index() const noexcept {
            return curr;
        }

        constexpr void swap(variant& other) noexcept(noexcept(((is_nothrow_move_constructible_v<Types> && is_nothrow_swappable_v<Types>) && ...)))
        requires (is_move_constructible_v<Types> && ...) {
            if (valueless_by_exception() && other.valueless_by_exception()) {
                return;
            } else if (index() == other.index()) {
                constexpr auto helper = [&]<class T, class ...Tn>(std::size_t idx, __internal::type_list<Tn...>, const auto& helper) {
                    if (idx != 0) {
                        helper(idx - 1, __internal::type_list<Tn...>{}, helper);
                    } else {
                        using std::swap;
                        swap(*__get_storage<T>(), *other.template __get_storage<T>());
                    }
                };

                helper(index(), __internal::type_list<Types...>{}, helper);
            } else {
                std::swap(storage.value, other.storage.value);
            }
        }

        /* Returns a pointer to the storage object cast to the desired type. */
        template<class T>
        constexpr T* __get_storage() const noexcept {
            return static_cast<T*>(static_cast<void*>(const_cast<aligned_union_t<0, Types...>*>(&storage)));
        }
    };

    /* 20.7.4 Variant helper classes */
    template<class T>
    struct variant_size;

    template<class T>
    struct variant_size<const T> : integral_constant<std::size_t, variant_size<remove_cv_t<T>>::value> {};

    template<class T>
    inline constexpr std::size_t variant_size_v = variant_size<T>::value;

    template<class ...Types>
    struct variant_size<variant<Types...>> : integral_constant<std::size_t, sizeof...(Types)> {};

    template<std::size_t I, class T>
    struct variant_alternative;

    template<std::size_t I, class T>
    struct variant_alternative<I, const T> {
        using type = add_const_t<typename variant_alternative<I, remove_cv_t<T>>::type>;
    };

    template<std::size_t I, class T>
    using variant_alternative_t = typename variant_alternative<I, T>::type;

    template<std::size_t I, class ...Types>
    struct variant_alternative<I, variant<Types...>> {
        using type = typename __internal::pick_ith_type<I, Types...>::type;
    };

    inline constexpr std::size_t variant_npos = -1;

    /* 20.17.11 Class bad_variant_access */
    class bad_variant_access : public exception {
    public:
        const char* what() const noexcept override;
    };

    /* 20.7.5 Value access */
    template<class T, class ...Types>
    requires __internal::appears_exactly_i_times<1, T, Types...>::value
    constexpr bool holds_alternative(const variant<Types...>& v) noexcept {
        return v.index() == __internal::get_index_of<0, T, Types...>::value;
    }

    template<std::size_t I, class ...Types>
    requires (I < sizeof...(Types))
    constexpr variant_alternative_t<I, variant<Types...>>& get(variant<Types...>& v) {
        return get<typename __internal::pick_ith_type<I, Types...>::type>(v);
    }

    template<std::size_t I, class ...Types>
    requires (I < sizeof...(Types))
    constexpr variant_alternative_t<I, variant<Types...>>&& get(variant<Types...>&& v) {
        return get<typename __internal::pick_ith_type<I, Types...>::type>(move(v));
    }

    template<std::size_t I, class ...Types>
    requires (I < sizeof...(Types))
    constexpr const variant_alternative_t<I, variant<Types...>>& get(const variant<Types...>& v) {
        return get<typename __internal::pick_ith_type<I, Types...>::type>(v);
    }

    template<std::size_t I, class ...Types>
    requires (I < sizeof...(Types))
    constexpr const variant_alternative_t<I, variant<Types...>>&& get(const variant<Types...>&& v) {
        return get<typename __internal::pick_ith_type<I, Types...>::type>(move(v));
    }

    template<class T, class ...Types>
    requires __internal::appears_exactly_i_times<1, T, Types...>::value
    constexpr T& get(variant<Types...>& v) {
        if (v.index() != __internal::get_index_of<0, T, Types...>::value) {
            throw bad_variant_access();
        } else {
            return *v.template __get_storage<T>();
        }
    }

    template<class T, class ...Types>
    requires __internal::appears_exactly_i_times<1, T, Types...>::value
    constexpr T&& get(variant<Types...>&& v) {
        if (v.index() != __internal::get_index_of<0, T, Types...>::value) {
            throw bad_variant_access();
        } else {
            return move(*v.template __get_storage<T>());
        }
    }

    template<class T, class ...Types>
    requires __internal::appears_exactly_i_times<1, T, Types...>::value
    constexpr const T& get(const variant<Types...>& v) {
        if (v.index() != __internal::get_index_of<0, T, Types...>::value) {
            throw bad_variant_access();
        } else {
            return *v.template __get_storage<T>();
        }
    }

    template<class T, class ...Types>
    requires __internal::appears_exactly_i_times<1, T, Types...>::value
    constexpr const T&& get(const variant<Types...>&& v) {
        if (v.index() != __internal::get_index_of<0, T, Types...>::value) {
            throw bad_variant_access();
        } else {
            return move(*v.template __get_storage<T>());
        }
    }

    template<std::size_t I, class ...Types>
    requires (I < sizeof...(Types))
    constexpr add_pointer_t<variant_alternative_t<I, variant<Types...>>> get_if(variant<Types...>* v) noexcept {
        return get_if<typename __internal::pick_ith_type<I, Types...>::type>(v);
    }

    template<std::size_t I, class ...Types>
    requires (I < sizeof...(Types))
    constexpr add_pointer_t<variant_alternative_t<I, variant<Types...>>> get_if(const variant<Types...>* v) noexcept {
        return get_if<typename __internal::pick_ith_type<I, Types...>::type>(v);
    }

    template<class T, class ...Types>
    requires __internal::appears_exactly_i_times<1, T, Types...>::value
    constexpr add_pointer_t<T> get_if(variant<Types...>* v) noexcept {
        if (v && v->index() != __internal::get_index_of<0, T, Types...>::value) {
            throw bad_variant_access();
        } else {
            return addressof(get(*v));
        }
    }

    template<class T, class ...Types>
    requires __internal::appears_exactly_i_times<1, T, Types...>::value
    constexpr add_pointer_t<const T> get_if(const variant<Types...>* v) noexcept {
        if (v && v->index() != __internal::get_index_of<0, T, Types...>::value) {
            throw bad_variant_access();
        } else {
            return addressof(get(*v));
        }
    }

    /* 20.7.6 Relation operators */
    template<class ...Types>
    requires (requires (const variant<Types...>& v) { { get<Types>(v) == get<Types>(v) } -> convertible_to<bool>; } && ...)
    constexpr bool operator==(const variant<Types...>& lhs, const variant<Types...>& rhs) {
        if (lhs.index() != rhs.index()) {
            return false;
        } else if (lhs.valueless_by_exception()) {
            return true;
        } else {
            constexpr auto helper = [&]<class T, class ...Tn>(std::size_t idx, __internal::type_list<Tn...>, const auto& helper) {
                if (idx != 0) {
                    return helper(idx - 1, __internal::type_list<Tn...>{}, helper);
                } else {
                    return get<T>(lhs) == get<T>(rhs);
                }
            };

            return helper(lhs.index(), __internal::type_list<Types...>{}, helper);
        }
    }

    template<class ...Types>
    requires (requires (const variant<Types...>& v) { { get<Types>(v) != get<Types>(v) } -> convertible_to<bool>; } && ...)
    constexpr bool operator!=(const variant<Types...>& lhs, const variant<Types...>& rhs) {
        if (lhs.index() != rhs.index()) {
            return false;
        } else if (lhs.valueless_by_exception()) {
            return true;
        } else {
            constexpr auto helper = [&]<class T, class ...Tn>(std::size_t idx, __internal::type_list<Tn...>, const auto& helper) {
                if (idx != 0) {
                    return helper(idx - 1, __internal::type_list<Tn...>{}, helper);
                } else {
                    return get<T>(lhs) != get<T>(rhs);
                }
            };

            return helper(lhs.index(), __internal::type_list<Types...>{}, helper);
        }
    }

    template<class ...Types>
    requires (requires (const variant<Types...>& v) { { get<Types>(v) < get<Types>(v) } -> convertible_to<bool>; } && ...)
    constexpr bool operator<(const variant<Types...>& lhs, const variant<Types...>& rhs) {
        if (rhs.valueless_by_exception()) {
            return false;
        } else if (lhs.valueless_by_exception()) {
            return true;
        } else if (lhs.index() < rhs.index()) {
            return true;
        } else if (lhs.index() > rhs.index()) {
            return false;
        } else {
            constexpr auto helper = [&]<class T, class ...Tn>(std::size_t idx, __internal::type_list<Tn...>, const auto& helper) {
                if (idx != 0) {
                    return helper(idx - 1, __internal::type_list<Tn...>{}, helper);
                } else {
                    return get<T>(lhs) < get<T>(rhs);
                }
            };

            return helper(lhs.index(), __internal::type_list<Types...>{}, helper);
        }
    }

    template<class ...Types>
    requires (requires (const variant<Types...>& v) { { get<Types>(v) > get<Types>(v) } -> convertible_to<bool>; } && ...)
    constexpr bool operator>(const variant<Types...>& lhs, const variant<Types...>& rhs) {
        if (lhs.valueless_by_exception()) {
            return false;
        } else if (rhs.valueless_by_exception()) {
            return true;
        } else if (lhs.index() > rhs.index()) {
            return true;
        } else if (lhs.index() < rhs.index()) {
            return false;
        } else {
            constexpr auto helper = [&]<class T, class ...Tn>(std::size_t idx, __internal::type_list<Tn...>, const auto& helper) {
                if (idx != 0) {
                    return helper(idx - 1, __internal::type_list<Tn...>{}, helper);
                } else {
                    return get<T>(lhs) > get<T>(rhs);
                }
            };

            return helper(lhs.index(), __internal::type_list<Types...>{}, helper);
        }
    }

    template<class ...Types>
    requires (requires (const variant<Types...>& v) { { get<Types>(v) <= get<Types>(v) } -> convertible_to<bool>; } && ...)
    constexpr bool operator<=(const variant<Types...>& lhs, const variant<Types...>& rhs) {
        if (lhs.valueless_by_exception()) {
            return true;
        } else if (rhs.valueless_by_exception()) {
            return false;
        } else if (lhs.index() > rhs.index()) {
            return false;
        } else if (lhs.index() < rhs.index()) {
            return true;
        } else {
            constexpr auto helper = [&]<class T, class ...Tn>(std::size_t idx, __internal::type_list<Tn...>, const auto& helper) {
                if (idx != 0) {
                    return helper(idx - 1, __internal::type_list<Tn...>{}, helper);
                } else {
                    return get<T>(lhs) <= get<T>(rhs);
                }
            };

            return helper(lhs.index(), __internal::type_list<Types...>{}, helper);
        }
    }

    template<class ...Types>
    requires (requires (const variant<Types...>& v) { { get<Types>(v) >= get<Types>(v) } -> convertible_to<bool>; } && ...)
    constexpr bool operator>=(const variant<Types...>& lhs, const variant<Types...>& rhs) {
        if (rhs.valueless_by_exception()) {
            return true;
        } else if (lhs.valueless_by_exception()) {
            return false;
        } else if (lhs.index() < rhs.index()) {
            return false;
        } else if (lhs.index() > rhs.index()) {
            return true;
        } else {
            constexpr auto helper = [&]<class T, class ...Tn>(std::size_t idx, __internal::type_list<Tn...>, const auto& helper) {
                if (idx != 0) {
                    return helper(idx - 1, __internal::type_list<Tn...>{}, helper);
                } else {
                    return get<T>(lhs) >= get<T>(rhs);
                }
            };

            return helper(lhs.index(), __internal::type_list<Types...>{}, helper);
        }
    }

    template<class ...Types>
    requires (three_way_comparable<Types> && ...)
    constexpr common_comparison_category_t<compare_three_way_result_t<Types>...>
    operator<=>(const variant<Types...>& lhs, const variant<Types...>& rhs) {
        if (lhs.valueless_by_exception() && rhs.valueless_by_exception()) {
            return strong_ordering::equal;
        } else if (lhs.valueless_by_exception()) {
            return strong_ordering::less;
        } else if (rhs.valueless_by_exception()) {
            return strong_ordering::greater;
        } else if (const strong_ordering c = lhs.index() <=> rhs.index(); c != 0) {
            return c;
        } else {
            constexpr auto helper = [&]<class T, class ...Tn>(std::size_t idx, __internal::type_list<Tn...>, const auto& helper) {
                if (idx != 0) {
                    return helper(idx - 1, __internal::type_list<Tn...>{}, helper);
                } else {
                    return get<T>(lhs) <=> get<T>(rhs);
                }
            };

            return helper(lhs.index(), __internal::type_list<Types...>{}, helper);
        }
    }

    /* 20.7.7 Visitation */
    namespace __internal {
        template<class Visitor, class ...Variants>
        struct __visit_impl {
        private:
            /* Concats multiple `type_list`. The function template is ill-formed if less than two lists are provided. Returns an instance of the new `type_list`. */
            template<class ...Results, class ...CurrListElems, class ...Lists>
            static consteval auto concat_type_list(__internal::type_list<Results...>, __internal::type_list<CurrListElems...>, Lists...) noexcept {
                constexpr auto curr_result = __internal::type_list<Results..., CurrListElems...>();
                if constexpr (sizeof...(Lists) == 0) {
                    return curr_result;
                } else {
                    return concat_type_list(curr_result, Lists()...);
                }
            }

            /* Appends a single element to an `index_sequence`. */
            template<std::size_t ...Is, std::size_t I>
            static consteval auto append_to_index_sequence(index_sequence<Is...>, integral_constant<std::size_t, I>)noexcept {
                return index_sequence<Is..., I>{};
            }

            /* Given a `type_list` of `index_sequence`, append a number to each. Returns an instance of the resultant `type_list`. */
            template<class ...Seqs, std::size_t N>
            static consteval auto append_to_index_sequences(__internal::type_list<Seqs...>, integral_constant<std::size_t, N>) noexcept {
                return __internal::type_list<decltype(append_to_index_sequence(Seqs{}, integral_constant<std::size_t, N>{}))...>{};
            }

            /* Given a `type_list` of length `N` consisting of `index_sequence` of length `K`, and an `index_sequence` of length `M`, create a new `type_list`
             * of length `M * N`, consisting of each `index_sequence` from the old `type_list` appended with an element from the supplied `index_sequence`. */
            template<class ...Seqs, std::size_t ...Ns>
            static consteval auto cartesian_product_for_index_sequences(__internal::type_list<Seqs...>, index_sequence<Ns...>) noexcept {
                return concat_type_list(
                    __internal::type_list<>(),
                    append_to_index_sequences(__internal::type_list<Seqs...>(), integral_constant<std::size_t, Ns>())...
                );
            }

            /* Creates a `type_list` of `index_sequence`, each of length `sizeof...(Variant)`, and whose `i`th element is between `0 <= i < variant_size(ith_variant)`. The `type_list`
             * includes all such possible `index_sequence`.
             * To call the function, supply a `type_list` with one empty `index_sequence` as the first element, and an `index_sequence` of the `variant_size` of each `Variant` as the second argument. */
            template<class ...IndexSequences, std::size_t I, std::size_t ...In>
            static consteval auto make_index_pack(__internal::type_list<IndexSequences...>, index_sequence<I, In...>) noexcept {
                constexpr auto curr_result = cartesian_product_for_index_sequences(__internal::type_list<IndexSequences...>(), make_index_sequence<I>());
                if constexpr (sizeof...(In) == 0) {
                    return curr_result;
                } else {
                    return make_index_pack(curr_result, index_sequence<In...>());
                }
            }

            /* The type of `type_list` that contains all possible `index_sequences` indicating the possibilities of alternatives for each variant. It's the return type of `make_index_pack`. */
            using index_pack_t = decltype(make_index_pack(__internal::type_list<index_sequence<>>(), index_sequence<variant_size_v<remove_cvref_t<Variants>>...>()));

            /* The return type of the `visit` function. */
            using return_t = invoke_result_t<Visitor, decltype(get<0>(declval<Variants>()))...>;

            /* A template for the method that will be stored in the fmatrix. It simply takes the index of the alternative held by each variant as a template argument,
             * and invokes the visitor on the alternatives. */
            template<std::size_t ...I>
            static constexpr return_t fmatrix_method(Visitor&& vis, Variants&& ...vars) {
                return invoke(forward<Visitor>(vis), get<I>(forward<Variants>(vars))...);
            }

            using fmatrix_method_t = return_t (*)(Visitor&& vis, Variants&& ...vars);

            /* Given an index for each `Variant`, returns the fmatrix entry for it. */
            static constexpr std::size_t compute_fmatrix_index(std::size_t (&&index_pack)[sizeof...(Variants)]) noexcept {
                constexpr std::size_t variant_sizes[sizeof...(Variants)] = { variant_size_v<remove_cvref_t<Variants>>... };

                std::size_t multiplier = 1;
                std::size_t idx = 0;

                for (std::size_t i = sizeof...(Variants); i > 0; i--) {
                    idx += index_pack[i - 1] * multiplier;
                    multiplier *= variant_sizes[i - 1];
                }

                return idx;
            }

            /* Creates an fmatrix, which contains a visitation function pointer for each possible combination of alternatives held by the variants, indexed according
             * to the `compute_fmatrix_index` function. */
            template<class ...Seqs>
            static constexpr auto make_fmatrix(__internal::type_list<Seqs...>) {
                array<fmatrix_method_t, (variant_size_v<remove_cvref_t<Variants>> * ...)> fmatrix;
                constexpr auto input_single_entry = []<std::size_t ...I>(array<fmatrix_method_t, (variant_size_v<remove_cvref_t<Variants>> * ...)>& fmatrix, index_sequence<I...>) noexcept {
                    fmatrix[compute_fmatrix_index({ I... })] = &fmatrix_method<I...>;
                };

                (input_single_entry(fmatrix, Seqs()),...);

                return fmatrix;
            }

            static constexpr array<fmatrix_method_t, (variant_size_v<remove_cvref_t<Variants>> * ...)> fmatrix = make_fmatrix(index_pack_t());

        public:
            constexpr decltype(auto) operator()(Visitor&& vis, Variants&& ...vars) const {
                if ((vars.valueless_by_exception() || ...)) {
                    throw bad_variant_access();
                } else if constexpr (sizeof...(Variants) == 0) {
                    return invoke(forward<Visitor>(vis));
                }

                return fmatrix[compute_fmatrix_index({ vars.index()... })](forward<Visitor>(vis), forward<Variants>(vars)...);
            }

            template<class R>
            constexpr R operator()(Visitor&& vis, Variants&& ...vars) const {
                if constexpr (is_void_v<return_t>) {
                    operator()(forward<Visitor>(vis), forward<Variants>(vars)...);
                } else {
                    return operator()(forward<Visitor>(vis), forward<Variants>(vars)...);
                }
            }

            static consteval bool is_return_type_equal() noexcept {
                constexpr auto helper = []<class ...IndexPacks>(__internal::type_list<IndexPacks...>) noexcept {
                    constexpr auto is_single_return_type_equal = []<std::size_t ...I>(index_sequence<I...>) noexcept {
                        return is_same_v<return_t, invoke_result_t<Visitor, decltype(get<I>(declval<Variants>()))...>>;
                    };

                    return (is_single_return_type_equal(IndexPacks()) && ...);
                };

                return helper(index_pack_t());
            }
        };
    }

    template<class Visitor, class ...Variants>
    requires (__internal::__visit_impl<Visitor, Variants...>::is_return_type_equal())
    constexpr auto visit(Visitor&& vis, Variants&& ...vars) {
        return __internal::__visit_impl<Visitor, Variants...>()(forward<Visitor>(vis), forward<Variants>(vars)...);
    }

    template<class R, class Visitor, class ...Variants>
    requires (__internal::__visit_impl<Visitor, Variants...>::is_return_type_equal())
    constexpr R visit(Visitor&& vis, Variants&& ...vars) {
        return __internal::__visit_impl<Visitor, Variants...>().template operator()<R>(forward<Visitor>(vis), forward<Variants>(vars)...);
    }

    /* 20.7.8 std::monostate */
    struct monostate {};

    /* 20.7.9 monostate relational operators */
    constexpr bool operator==(monostate, monostate) noexcept;
    constexpr strong_ordering operator<=>(monostate, monostate) noexcept;

    /* 20.7.10 Specialized algorithms */
    template<class ...Types>
    requires ((is_move_constructible_v<Types> && is_swappable_v<Types>) && ...)
    constexpr void swap(variant<Types...>& v, variant<Types...>& w) noexcept(noexcept(v.swap(w))) {
        v.swap(w);
    }

    /* 20.7.12 Hash support */
    template<class ...Types>
    requires (requires (const Types& t) { hash<remove_const_t<Types>>()(t); } && ...)
    struct hash<variant<Types...>> : hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const variant<Types...>& v) const {
            constexpr auto helper = [&]<class T, class ...Tn>(std::size_t idx, __internal::type_list<Tn...>, const auto& helper) {
                if (idx != 0) {
                    return helper(idx - 1, __internal::type_list<Tn...>{}, helper);
                } else {
                    return hash<remove_const_t<T>>()(get<T>(v));
                }
            };

            return helper(v.index(), __internal::type_list<Types...>{}, helper);
        };
    };

    template<>
    struct hash<monostate> {
        std::size_t operator()(const monostate&) const noexcept;
    };
}