#include "cstring.hpp"

#include "string.h"

namespace std {
const void* memchr(const void* s, int c, size_t n) {
    return ::memchr(s, c, n);
}

void* memchr(void* s, int c, size_t n) {
    return ::memchr(s, c, n);
}

const char* strchr(const char* s, int c) {
    return ::strchr(s, c);
}

char* strchr(char* s, int c) {
    return ::strchr(s, c);
}

const char* strpbrk(const char* s1, const char* s2) {
    return ::strpbrk(s1, s2);
}

char* strpbrk(char* s1, const char* s2) {
    return ::strpbrk(s1, s2);
}

const char* strrchr(const char* s, int c) {
    return ::strrchr(s, c);
}

char* strrchr(char* s, int c) {
    return ::strrchr(s, c);
}

const char* strstr(const char* s1, const char* s2) {
    return ::strstr(s1, s2);
}

char* strstr(char* s1, const char* s2) {
    return ::strstr(s1, s2);
}
}