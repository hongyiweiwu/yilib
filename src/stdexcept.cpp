#include "stdexcept.hpp"
#include "string.hpp"
#include "exception.hpp"
#include "cstring.hpp"

namespace std {
    logic_error::logic_error(const string& what_arg) : exception(), what_str(make_unique<char[]>(what_arg.length() + 1)) {
        std::memcpy(what_str.get(), what_arg.c_str(), what_arg.length() + 1);
    }

    logic_error::logic_error(const char* what_arg) : exception(), what_str(make_unique<char[]>(std::strlen(what_arg) + 1)) {
        std::strcpy(what_str.get(), what_arg);
    }

    const char* logic_error::what() const noexcept { return what_str.get(); }

    domain_error::domain_error(const string& what_arg) : logic_error(what_arg) {}
    domain_error::domain_error(const char* what_arg) : logic_error(what_arg) {}

    invalid_argument::invalid_argument(const string& what_arg) : logic_error(what_arg) {}
    invalid_argument::invalid_argument(const char* what_arg) : logic_error(what_arg) {}

    length_error::length_error(const string& what_arg) : logic_error(what_arg) {}
    length_error::length_error(const char* what_arg) : logic_error(what_arg) {}

    out_of_range::out_of_range(const string& what_arg) : logic_error(what_arg) {}
    out_of_range::out_of_range(const char* what_arg) : logic_error(what_arg) {}

    runtime_error::runtime_error(const string& what_arg) : exception(), what_str(make_unique<char[]>(what_arg.length() + 1)) {
        std::memcpy(what_str.get(), what_arg.c_str(), what_arg.length() + 1);
    }

    runtime_error::runtime_error(const char* what_arg) : exception(), what_str(make_unique<char[]>(std::strlen(what_arg) + 1)) {
        std::strcpy(what_str.get(), what_arg);
    }

    const char* runtime_error::what() const noexcept { return what_str.get(); }

    range_error::range_error(const string& what_arg) : runtime_error(what_arg) {}
    range_error::range_error(const char* what_arg) : runtime_error(what_arg) {}

    overflow_error::overflow_error(const string& what_arg) : runtime_error(what_arg) {}
    overflow_error::overflow_error(const char* what_arg) : runtime_error(what_arg) {}

    underflow_error::underflow_error(const string& what_arg) : runtime_error(what_arg) {}
    underflow_error::underflow_error(const char* what_arg) : runtime_error(what_arg) {}
}