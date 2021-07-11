#pragma once

#include "wchar.h"

namespace std {
    using size_t = ::size_t;
    using mbstate_t = ::mbstate_t;
    using wint_t = ::wint_t;

    // Forward declaration. Declared in "ctime.hpp".
    using ::tm;

    using ::fwprintf;
    using ::fwscanf;
    using ::swprintf;
    using ::swscanf;
    using ::vfwprintf;
    using ::vfwscanf;
    using ::vswprintf;
    using ::vswscanf;
    using ::vwprintf;
    using ::vwscanf;
    using ::wprintf;
    using ::wscanf;
    using ::fgetwc;
    using ::fgetws;
    using ::fputwc;
    using ::fputws;
    using ::fwide;
    using ::getwc;
    using ::getwchar;
    using ::putwc;
    using ::putwchar;
    using ::ungetwc;
    using ::wcstod;
    using ::wcstof;
    using ::wcstold;
    using ::wcstol;
    using ::wcstoll;
    using ::wcstoul;
    using ::wcstoull;
    using ::wcscpy;
    using ::wcsncpy;
    using ::wmemcpy;
    using ::wmemmove;
    using ::wcscat;
    using ::wcsncat;
    using ::wcscmp;
    using ::wcscoll;
    using ::wcsncmp;
    using ::wcsxfrm;
    using ::wmemcmp;
    
    const wchar_t* wcschr(const wchar_t* s, wchar_t c);
    wchar_t* wcschr(wchar_t* s, wchar_t c);
    using ::wcscspn;
    const wchar_t* wcspbrk(const wchar_t* s1, const wchar_t* s2);
    wchar_t* wcspbrk(wchar_t* s1, const wchar_t* s2);
    const wchar_t* wcsrchr(const wchar_t* s, wchar_t c);
    wchar_t* wcsrchr(wchar_t* s, wchar_t c);

    using ::wcsspn;
    const wchar_t* wcsstr(const wchar_t* s1, const wchar_t* s2);
    wchar_t* wcsstr(wchar_t* s1, const wchar_t* s2);

    using ::wcstok;
    const wchar_t* wmemchr(const wchar_t* s, wchar_t c, std::size_t n);
    wchar_t* wmemchr(wchar_t* s, wchar_t c, std::size_t n);

    using ::wcslen;
    using ::wmemset;
    using ::wcsftime;
    using ::btowc;
    using ::wctob;

    using ::mbsinit;
    using ::mbrlen;
    using ::mbrtowc;
    using ::wcrtomb;
    using ::mbsrtowcs;
    using ::wcsrtombs;
}