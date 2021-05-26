#include "system_error.hpp"
#include "compare.hpp"
#include "cstring.hpp"
#include "string.hpp"
#include "cstddef.hpp"

namespace std {
    error_condition error_category::default_error_condition(int ev) const noexcept {
        return error_condition(ev, *this);
    }

    bool error_category::equivalent(int code, const error_condition& condition) const noexcept {
        return default_error_condition(code) == condition;
    }

    bool error_category::equivalent(const error_code& code, int condition) const noexcept {
        return *this == code.category() && code.value() == condition;
    }

    bool error_category::operator==(const error_category& rhs) const noexcept {
        return this == &rhs;
    }

    strong_ordering error_category::operator<=>(const error_category& rhs) const noexcept {
        return compare_three_way()(this, &rhs);
    }

    namespace __internal {
        class generic_category : public error_category {
        public:
            constexpr generic_category() noexcept = default;

            const char* name() const noexcept override { return "generic"; }
            string message(int ev) const override { return std::strerror(ev); }
        };
    }

    const error_category& generic_category() noexcept {
        static constinit const __internal::generic_category singleton;
        return singleton;
    }

    namespace __internal {
        class system_category : public error_category {
        public:
            constexpr system_category() noexcept = default;
            const char* name() const noexcept override { return "system"; }
            string message(int ev) const override { return std::strerror(ev); }
            error_condition default_error_condition(int ev) const noexcept override {
                return error_condition(ev, std::generic_category());
            }
        };
    }

    const error_category& system_category() noexcept {
        static constinit const __internal::system_category singleton;
        return singleton;
    }

    error_code::error_code() noexcept : val(0), cat(&system_category()) {}
    error_code::error_code(int val, const error_category& cat) noexcept : val(val), cat(&cat) {}

    void error_code::assign(int val, const error_category& cat) noexcept {
        this->val = val;
        this->cat = &cat;
    }

    void error_code::clear() {
        val = 0;
        cat = &system_category();
    }

    int error_code::value() const noexcept { return val; }
    const error_category& error_code::category() const noexcept { return *cat; }
    error_condition error_code::default_error_condition() const noexcept { return category().default_error_condition(value()); }
    string error_code::message() const { return category().message(value()); }
    error_code::operator bool() const noexcept { return value() != 0; }

    system_error::system_error(error_code ec, const string& what_arg) : runtime_error(what_arg), ec(ec) {}
    system_error::system_error(error_code ec, const char* what_arg) : runtime_error(what_arg), ec(ec) {}
    system_error::system_error(error_code ec) : runtime_error("std::system_error"), ec(ec) {}
    system_error::system_error(int ev, const error_category& ecat, const string& what_arg) : runtime_error(what_arg), ec(error_code(ev, ecat)) {}
    system_error::system_error(int ev, const error_category& ecat, const char* what_arg) : runtime_error(what_arg), ec(error_code(ev, ecat)) {}
    system_error::system_error(int ev, const error_category& ecat) : runtime_error("std::system_error"), ec(error_code(ev, ecat)) {}
    const error_code& system_error::code() const noexcept { return ec; }
    const char* system_error::what() const noexcept { return runtime_error::what(); }

    error_code make_error_code(errc e) noexcept {
        return error_code(static_cast<int>(e), generic_category());
    }

    error_condition::error_condition() noexcept : val(0), cat(&system_category()) {}
    error_condition::error_condition(int val, const error_category& cat) noexcept : val(val), cat(&cat) {}

    void error_condition::assign(int val, const error_category& cat) noexcept {
        this->val = val;
        this->cat = &cat;
    }

    void error_condition::clear() {
        val = 0;
        cat = &system_category();
    }

    int error_condition::value() const noexcept { return val; }
    const error_category& error_condition::category() const noexcept { return *cat; }
    string error_condition::message() const { return category().message(value()); }
    error_condition::operator bool() const noexcept { return value() != 0; }

    error_condition make_error_condition(errc e) noexcept {
        return error_condition(static_cast<int>(e), generic_category());
    }

    bool operator==(const error_code& lhs, const error_code& rhs) noexcept {
        return lhs.category() == rhs.category() && lhs.value() == rhs.value();
    }

    bool operator==(const error_code& lhs, const error_condition& rhs) noexcept {
        return lhs.category().equivalent(lhs.value(), rhs) || rhs.category().equivalent(lhs, rhs.value());
    }

    bool operator==(const error_condition& lhs, const error_condition& rhs) noexcept {
        return lhs.category() == rhs.category() && lhs.value() == rhs.value();
    }

    strong_ordering operator<=>(const error_code& lhs, const error_code& rhs) noexcept {
        if (auto c = lhs.category() <=> rhs.category(); c != 0) return c;
        return lhs.value() <=> rhs.value();
    }

    strong_ordering operator<=>(const error_condition& lhs, const error_condition& rhs) noexcept {
        if (auto c = lhs.category() <=> rhs.category(); c != 0) return c;
        return lhs.value() <=> rhs.value();
    }

    size_t hash<error_code>::operator()(const error_code& code) const noexcept {
        return static_cast<size_t>(code.value());
    }

    size_t hash<error_condition>::operator()(const error_condition& cond) const noexcept {
        return static_cast<size_t>(cond.value());
    }
}