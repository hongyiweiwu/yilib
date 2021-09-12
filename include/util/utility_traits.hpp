#pragma once

namespace std::__internal {
    /* A boolean variable template that always returns false. Mostly used in conjunction with static_assert to
     * guard against an impossible condition branch, when we hope the error would only be reported during template
     * instantiation. */
    template<class ...T>
    constexpr bool always_false = false;

    template<class U, class ...T>
    struct always_illformed {
        static_assert(always_false<U>);
    };

    /* A list of types. Usually used to provide hints to compilers for template argument deduction when these arguments
     * cannot be specified explicitly. */
    template<class ...T>
    struct type_list {};

    /* Pick the ith type from the list. */
    template<unsigned int I, class ...Tn>
    struct pick_ith_type;

    template<unsigned int I, class T, class ...Tn>
    struct pick_ith_type<I, T, Tn...> {
        using type = typename pick_ith_type<I - 1, Tn...>::type;
    };

    template<class T, class ...Tn>
    struct pick_ith_type<0, T, Tn...> {
        using type = T;
    };

    /* Returns the index of a class in a list of classes. Should be called as `get_index_of<0, Needle, Hay...>`. */
    template<unsigned int I, class T, class T1, class ...Types>
    struct get_index_of {
        static constexpr unsigned int value = get_index_of<I + 1, T, Types...>::value;
    };

    template<unsigned int I, class T, class ...Types>
    struct get_index_of<I, T, T, Types...> {
        static constexpr bool value = I;
    };

    /* Provides a static member `value` that is `true` iff `T` appears in `Types` for exactly `I` times. */
    template<unsigned int I, class T, class ...Types>
    struct appears_exactly_i_times {
        template<class Tp>
        struct wrapper {};

        template<class Tp>
        static constexpr bool is_same_v = requires (void (*fn)(wrapper<Tp>)) { fn(wrapper<T>()); };

        static constexpr bool value = (is_same_v<Types> + ...) == I;
    };
}