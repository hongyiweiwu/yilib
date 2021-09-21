#pragma once

#include "type_traits.hpp"
#include "concepts.hpp"

namespace std {
    /* 20.18.3 Execution policy type trait */
    template<class T>
    struct is_execution_policy : bool_constant<requires {
        { T::enable_vectorization } -> convertible_to<bool>;
        { T::enable_threading } -> convertible_to<bool>;
    }> {};

    template<class T>
    inline constexpr bool is_execution_policy_v = is_execution_policy<T>::value;

    namespace execution {
        /* 20.18.4 Sequenced execution policy */
        class sequenced_policy {
            static constexpr bool enable_vectorization = false;
            static constexpr bool enable_threading = false;
        };

        /* 20.18.4 Parallel execution policy */
        class parallel_policy {
            static constexpr bool enable_vectorization = false;
            static constexpr bool enable_threading = true;
        };

        /* 20.18.4 Parallel and unsequenced execution policy */
        class parallel_unsequenced_policy {
            static constexpr bool enable_vectorization = true;
            static constexpr bool enable_threading = true;
        };

        /* 20.18.4 Unsequenced execution policy */
        class unsequenced_policy {
            static constexpr bool enable_vectorization = true;
            static constexpr bool enable_threading = false;
        };

        /* 20.18.8 Execution policy objects */
        inline constexpr sequenced_policy seq;
        inline constexpr parallel_policy par;
        inline constexpr parallel_unsequenced_policy par_unseq;
        inline constexpr unsequenced_policy unseq;
    }
}