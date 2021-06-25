#include "stdexcept.hpp"
#include "string.hpp"
#include "exception.hpp"

namespace std {
    logic_error::logic_error(const string& what_arg) : exception(), what_str(what_arg) {}
    logic_error::logic_error(const char* what_arg) : exception(), what_str(what_arg) {}

    const char* logic_error::what() const noexcept { return what_str.c_str(); }

    domain_error::domain_error(const string& what_arg) : logic_error(what_arg) {}
    domain_error::domain_error(const char* what_arg) : logic_error(what_arg) {}

    invalid_argument::invalid_argument(const string& what_arg) : logic_error(what_arg) {}
    invalid_argument::invalid_argument(const char* what_arg) : logic_error(what_arg) {}

    length_error::length_error(const string& what_arg) : logic_error(what_arg) {}
    length_error::length_error(const char* what_arg) : logic_error(what_arg) {}

    out_of_range::out_of_range(const string& what_arg) : logic_error(what_arg) {}
    out_of_range::out_of_range(const char* what_arg) : logic_error(what_arg) {}

    runtime_error::runtime_error(const string& what_arg) : exception(), what_str(what_arg) {}
    runtime_error::runtime_error(const char* what_arg) : exception(), what_str(what_arg) {}

    const char* runtime_error::what() const noexcept { return what_str.c_str(); }

    range_error::range_error(const string& what_arg) : runtime_error(what_arg) {}
    range_error::range_error(const char* what_arg) : runtime_error(what_arg) {}

    overflow_error::overflow_error(const string& what_arg) : runtime_error(what_arg) {}
    overflow_error::overflow_error(const char* what_arg) : runtime_error(what_arg) {}

    underflow_error::underflow_error(const string& what_arg) : runtime_error(what_arg) {}
    underflow_error::underflow_error(const char* what_arg) : runtime_error(what_arg) {}
}