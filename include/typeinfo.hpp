#pragma once

#include "exception.hpp"
#include "cstddef.hpp"

namespace std {
    class type_info {
    private:
        const char* type_name;
    protected:
        explicit type_info(const char* name);
    public:
        virtual ~type_info() = default;
        bool operator==(const type_info& rhs) const noexcept;
        bool before(const type_info& rhs) const noexcept;
        std::size_t hash_code() const noexcept;
        const char* name() const noexcept;

        type_info(const type_info&) = delete;
        type_info& operator=(const type_info&) = delete;
    };

    class bad_cast : public exception {
    public:
        const char* what() const noexcept override;
    };

    class bad_typeid : public exception {
    public:
        const char* what() const noexcept override;
    };
}