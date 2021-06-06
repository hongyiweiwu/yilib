#pragma once

#include "type_traits/reference_manip.hpp"

namespace std::__internal {
    template<class T>
    typename add_rvalue_reference<T>::type declval() noexcept;    
}