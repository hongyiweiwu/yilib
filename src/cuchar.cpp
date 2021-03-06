#include "cuchar.hpp"

#if !__APPLE__
#include "uchar.h"
#endif

namespace std {
#if !__APPLE__
    // TODO: Implement.
    std::size_t mbrtoc8(char8_t* pc8, const char* s, std::size_t n, mbstate_t* ps);
    std::size_t c8rtomb(char* s, char8_t c8, mbstate_t* ps);
#endif
}



