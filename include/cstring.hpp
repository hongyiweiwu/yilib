#pragma once

#include "string.h"

namespace std {
    using size_t = ::size_t;
    
    using ::memcpy;
    using ::memmove;
    using ::strcpy;
    using ::strncpy;
    using ::strcat;
    using ::strncat;
    using ::memcmp;
    using ::strcmp;
    using ::strcoll;
    using ::strncmp;
    using ::strxfrm;
    
    const void* memchr(const void* s, int c, size_t n);
    void* memchr(void* s, int c, size_t n);
    const char* strchr(const char* s, int c);
    char* strchr(char* s, int c);

    using ::strcspn;
    const char* strpbrk(const char* s1, const char* s2);
    char* strpbrk(char* s1, const char* s2);
    const char* strrchr(const char* s, int c);
    char* strrchr(char* s, int c);

    using ::strspn;
    const char* strstr(const char* s1, const char* s2);
    char* strstr(char* s1, const char* s2);
    
    using ::strtok;
    using ::memset;
    using ::strerror;
    using ::strlen;
}
