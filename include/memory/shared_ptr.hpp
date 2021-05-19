#pragma once

#include "exception.hpp"

namespace std {
    /* 20.11.2 Class bad_weak_ptr */
    class bad_weak_ptr : public exception {
    public:
        const char* what() const noexcept override;
    };

    /* 20.11.3 Class template shared_ptr */
    // Forward declaration. Implemented directly below.
    template<class T> class weak_ptr;
    
    template<class T> class shared_ptr {
    public:
        using element_type = T;
        using weak_type = weak_ptr<T>;

    private:
        element_type* ptr;
        
    };
}