#include "string.hpp"
#include "cstddef.hpp"
#include "string_view.hpp"
#include "cstdlib.hpp"
#include "cerrno.hpp"
#include "stdexcept.hpp"
#include "limits.hpp"
#include "memory.hpp"
#include "cstdio.hpp"
#include "cwchar.hpp"

namespace std {
    int stoi(const string& str, std::size_t* idx, int base) {
        char* conv_end = nullptr;
        const long res = std::strtol(str.data(), &conv_end, base);
        if (errno == ERANGE || res > numeric_limits<int>::max()) {
            throw out_of_range("Calling stoi with out-of-range arguments.");
        } else if (conv_end == str.c_str()) {
            throw invalid_argument("Calling stoi with invalid arguments.");
        } else {
            *idx = conv_end - str.begin();
            return res;
        }
    }

    long stol(const string& str, std::size_t* idx, int base) {
        char* conv_end = nullptr;
        const long res = std::strtol(str.data(), &conv_end, base);
        if (errno == ERANGE) {
            throw out_of_range("Calling stol with out-of-range arguments.");
        } else if (conv_end == str.c_str()) {
            throw invalid_argument("Calling stol with invalid arguments.");
        } else {
            *idx = conv_end - str.begin();
            return res;
        }
    }

    unsigned long stoul(const string& str, std::size_t* idx, int base) {
        char* conv_end = nullptr;
        const unsigned long res = std::strtoul(str.data(), &conv_end, base);
        if (errno == ERANGE) {
            throw out_of_range("Calling stol with out-of-range arguments.");
        } else if (conv_end == str.c_str()) {
            throw invalid_argument("Calling stol with invalid arguments.");
        } else {
            *idx = conv_end - str.begin();
            return res;
        }
    }

    long long stoll(const string& str, std::size_t* idx, int base) {
        char* conv_end = nullptr;
        const long long res = std::strtoll(str.data(), &conv_end, base);
        if (errno == ERANGE) {
            throw out_of_range("Calling stol with out-of-range arguments.");
        } else if (conv_end == str.c_str()) {
            throw invalid_argument("Calling stol with invalid arguments.");
        } else {
            *idx = conv_end - str.begin();
            return res;
        }
    }

    unsigned long long stoull(const string& str, std::size_t* idx, int base) {
        char* conv_end = nullptr;
        const unsigned long long res = std::strtoull(str.data(), &conv_end, base);
        if (errno == ERANGE) {
            throw out_of_range("Calling stol with out-of-range arguments.");
        } else if (conv_end == str.c_str()) {
            throw invalid_argument("Calling stol with invalid arguments.");
        } else {
            *idx = conv_end - str.begin();
            return res;
        }
    }

    float stof(const string& str, std::size_t* idx) {
        char* conv_end = nullptr;
        const float res = std::strtof(str.data(), &conv_end);
        if (errno == ERANGE) {
            throw out_of_range("Calling stol with out-of-range arguments.");
        } else if (conv_end == str.c_str()) {
            throw invalid_argument("Calling stol with invalid arguments.");
        } else {
            *idx = conv_end - str.begin();
            return res;
        }
    }

    double stod(const string& str, std::size_t* idx) {
        char* conv_end = nullptr;
        const double res = std::strtod(str.data(), &conv_end);
        if (errno == ERANGE) {
            throw out_of_range("Calling stol with out-of-range arguments.");
        } else if (conv_end == str.c_str()) {
            throw invalid_argument("Calling stol with invalid arguments.");
        } else {
            *idx = conv_end - str.begin();
            return res;
        }
    }

    long double stold(const string& str, std::size_t* idx) {
        char* conv_end = nullptr;
        const long double res = std::strtold(str.data(), &conv_end);
        if (errno == ERANGE) {
            throw out_of_range("Calling stol with out-of-range arguments.");
        } else if (conv_end == str.c_str()) {
            throw invalid_argument("Calling stol with invalid arguments.");
        } else {
            *idx = conv_end - str.begin();
            return res;
        }
    }
    
    string to_string(int val) {
        char buf[numeric_limits<int>::digits10 + 1];
        std::sprintf(buf, "%d", val);
        return string(buf);
    }

    string to_string(unsigned int val) {
        char buf[numeric_limits<unsigned int>::digits10 + 1];
        std::sprintf(buf, "%u", val);
        return string(buf);
    }

    string to_string(long val) {
        char buf[numeric_limits<long>::digits10 + 1];
        std::sprintf(buf, "%ld", val);
        return string(buf);
    }

    string to_string(unsigned long val) {
        char buf[numeric_limits<unsigned long>::digits10 + 1];
        std::sprintf(buf, "%lu", val);
        return string(buf);
    }

    string to_string(long long val) {
        char buf[numeric_limits<long long>::digits10 + 1];
        std::sprintf(buf, "%lld", val);
        return string(buf);
    }

    string to_string(unsigned long long val) {
        char buf[numeric_limits<unsigned long long>::digits10 + 1];
        std::sprintf(buf, "%llu", val);
        return string(buf);
    }

    string to_string(float val) {
        char buf[numeric_limits<float>::max_digits10 + 1];
        std::sprintf(buf, "%f", val);
        return string(buf);
    }

    string to_string(double val) {
        char buf[numeric_limits<double>::max_digits10 + 1];
        std::sprintf(buf, "%f", val);
        return string(buf);
    }

    string to_string(long double val) {
        char buf[numeric_limits<long double>::max_digits10 + 1];
        std::sprintf(buf, "%Lf", val);
        return string(buf);
    }

    int stoi(const wstring& str, std::size_t* idx, int base) {
        wchar_t* conv_end = nullptr;
        const long res = std::wcstol(str.data(), &conv_end, base);
        if (errno == ERANGE || res > numeric_limits<int>::max()) {
            throw out_of_range("Calling stoi with out-of-range arguments.");
        } else if (conv_end == str.c_str()) {
            throw invalid_argument("Calling stoi with invalid arguments.");
        } else {
            *idx = conv_end - str.begin();
            return res;
        }
    }

    long stol(const wstring& str, std::size_t* idx, int base) {
        wchar_t* conv_end = nullptr;
        const long res = std::wcstol(str.data(), &conv_end, base);
        if (errno == ERANGE) {
            throw out_of_range("Calling stol with out-of-range arguments.");
        } else if (conv_end == str.c_str()) {
            throw invalid_argument("Calling stol with invalid arguments.");
        } else {
            *idx = conv_end - str.begin();
            return res;
        }
    }

    unsigned long stoul(const wstring& str, std::size_t* idx, int base) {
        wchar_t* conv_end = nullptr;
        const unsigned long res = std::wcstoul(str.data(), &conv_end, base);
        if (errno == ERANGE) {
            throw out_of_range("Calling stol with out-of-range arguments.");
        } else if (conv_end == str.c_str()) {
            throw invalid_argument("Calling stol with invalid arguments.");
        } else {
            *idx = conv_end - str.begin();
            return res;
        }
    }

    long long stoll(const wstring& str, std::size_t* idx, int base) {
        wchar_t* conv_end = nullptr;
        const long long res = std::wcstoll(str.data(), &conv_end, base);
        if (errno == ERANGE) {
            throw out_of_range("Calling stol with out-of-range arguments.");
        } else if (conv_end == str.c_str()) {
            throw invalid_argument("Calling stol with invalid arguments.");
        } else {
            *idx = conv_end - str.begin();
            return res;
        }
    }

    unsigned long long stoull(const wstring& str, std::size_t* idx, int base) {
        wchar_t* conv_end = nullptr;
        const unsigned long long res = std::wcstoull(str.data(), &conv_end, base);
        if (errno == ERANGE) {
            throw out_of_range("Calling stol with out-of-range arguments.");
        } else if (conv_end == str.c_str()) {
            throw invalid_argument("Calling stol with invalid arguments.");
        } else {
            *idx = conv_end - str.begin();
            return res;
        }
    }

    float stof(const wstring& str, std::size_t* idx) {
        wchar_t* conv_end = nullptr;
        const float res = std::wcstof(str.data(), &conv_end);
        if (errno == ERANGE) {
            throw out_of_range("Calling stol with out-of-range arguments.");
        } else if (conv_end == str.c_str()) {
            throw invalid_argument("Calling stol with invalid arguments.");
        } else {
            *idx = conv_end - str.begin();
            return res;
        }
    }

    double stod(const wstring& str, std::size_t* idx) {
        wchar_t* conv_end = nullptr;
        const double res = std::wcstod(str.data(), &conv_end);
        if (errno == ERANGE) {
            throw out_of_range("Calling stol with out-of-range arguments.");
        } else if (conv_end == str.c_str()) {
            throw invalid_argument("Calling stol with invalid arguments.");
        } else {
            *idx = conv_end - str.begin();
            return res;
        }
    }

    long double stold(const wstring& str, std::size_t* idx) {
        wchar_t* conv_end = nullptr;
        const long double res = std::wcstold(str.data(), &conv_end);
        if (errno == ERANGE) {
            throw out_of_range("Calling stol with out-of-range arguments.");
        } else if (conv_end == str.c_str()) {
            throw invalid_argument("Calling stol with invalid arguments.");
        } else {
            *idx = conv_end - str.begin();
            return res;
        }
    }
    
    wstring to_wstring(int val) {
        wchar_t buf[numeric_limits<int>::digits10 + 1];
        std::swprintf(buf, extent_v<decltype(buf)>, L"%d", val);
        return wstring(buf);
    }

    wstring to_wstring(unsigned int val) {
        wchar_t buf[numeric_limits<unsigned int>::digits10 + 1];
        std::swprintf(buf, extent_v<decltype(buf)>, L"%u", val);
        return wstring(buf);
    }

    wstring to_wstring(long val) {
        wchar_t buf[numeric_limits<long>::digits10 + 1];
        std::swprintf(buf, extent_v<decltype(buf)>, L"%ld", val);
        return wstring(buf);
    }

    wstring to_wstring(unsigned long val) {
        wchar_t buf[numeric_limits<unsigned long>::digits10 + 1];
        std::swprintf(buf, extent_v<decltype(buf)>, L"%lu", val);
        return wstring(buf);
    }

    wstring to_wstring(long long val) {
        wchar_t buf[numeric_limits<long long>::digits10 + 1];
        std::swprintf(buf, extent_v<decltype(buf)>, L"%lld", val);
        return wstring(buf);
    }

    wstring to_wstring(unsigned long long val) {
        wchar_t buf[numeric_limits<unsigned long long>::digits10 + 1];
        std::swprintf(buf, extent_v<decltype(buf)>, L"%llu", val);
        return wstring(buf);
    }

    wstring to_wstring(float val) {
        wchar_t buf[numeric_limits<float>::max_digits10 + 1];
        std::swprintf(buf, extent_v<decltype(buf)>, L"%f", val);
        return wstring(buf);
    }

    wstring to_wstring(double val) {
        wchar_t buf[numeric_limits<double>::max_digits10 + 1];
        std::swprintf(buf, extent_v<decltype(buf)>, L"%f", val);
        return wstring(buf);
    }

    wstring to_wstring(long double val) {
        wchar_t buf[numeric_limits<long double>::max_digits10 + 1];
        std::swprintf(buf, extent_v<decltype(buf)>, L"%Lf", val);
        return wstring(buf);
    }

    std::size_t hash<string>::operator()(const string& key) const noexcept {
        return hash<string_view>()(key);
    }

    std::size_t hash<u8string>::operator()(const u8string& key) const noexcept {
        return hash<u8string_view>()(key);
    }

    std::size_t hash<u16string>::operator()(const u16string& key) const noexcept {
        return hash<u16string_view>()(key);
    }

    std::size_t hash<u32string>::operator()(const u32string& key) const noexcept {
        return hash<u32string_view>()(key);
    }

    std::size_t hash<wstring>::operator()(const wstring& key) const noexcept {
        return hash<wstring_view>()(key);
    }

    std::size_t hash<pmr::string>::operator()(const pmr::string& key) const noexcept {
        return hash<string_view>()(key);
    }

    std::size_t hash<pmr::u8string>::operator()(const pmr::u8string& key) const noexcept {
        return hash<u8string_view>()(key);
    }

    std::size_t hash<pmr::u16string>::operator()(const pmr::u16string& key) const noexcept {
        return hash<u16string_view>()(key);
    }

    std::size_t hash<pmr::u32string>::operator()(const pmr::u32string& key) const noexcept {
        return hash<u32string_view>()(key);
    }

    std::size_t hash<pmr::wstring>::operator()(const pmr::wstring& key) const noexcept {
        return hash<wstring_view>()(key);
    }
}