#include "cwchar.hpp"

#include "wchar.h"

namespace std {
    const wchar_t* wcschr(const wchar_t* s, wchar_t c) {
        return ::wcschr(s, c);
    }

    wchar_t* wcschr(wchar_t* s, wchar_t c) {
        return ::wcschr(s, c);
    }

    const wchar_t* wcspbrk(const wchar_t* s1, const wchar_t* s2) {
        return ::wcspbrk(s1, s2);
    }

    wchar_t* wcspbrk(wchar_t* s1, const wchar_t* s2) {
        return ::wcspbrk(s1, s2);
    }

    const wchar_t* wcsrchr(const wchar_t* s, wchar_t c) {
        return ::wcsrchr(s, c);
    }

    wchar_t* wcsrchr(wchar_t* s, wchar_t c) {
        return ::wcsrchr(s, c);
    }

    const wchar_t* wcsstr(const wchar_t* s1, const wchar_t* s2) {
        return ::wcsstr(s1, s2);
    }

    wchar_t* wcsstr(wchar_t* s1, const wchar_t* s2) {
        return ::wcsstr(s1, s2);
    }

    const wchar_t* wmemchr(const wchar_t* s, wchar_t c, std::size_t n) {
        return ::wmemchr(s, c, n);
    }

    wchar_t* wmemchr(wchar_t* s, wchar_t c, std::size_t n) {
        return ::wmemchr(s, c, n);
    }
}