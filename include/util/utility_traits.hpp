#pragma once

namespace std::__internal {
    /* A boolean variable template that always returns false. Mostly used in conjunction with static_assert to
     * guard against an impossible condition branch, when we hope the error would only be reported during template
     * instantiation. */
    template<class T> constexpr bool always_false = false;

    /* A list of types. Usually used to provide hints to compilers for template argument deduction when these arguments
     * cannot be specified explicitly. */
    template<class ...T> struct type_list {};
}