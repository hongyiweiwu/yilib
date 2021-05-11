#pragma once

#include "util/macros.hpp"
#include "cstdint.hpp"

namespace yilib {
// TODO: Only Clang compatible for now.
#if __has_intrinsics_for(builtin_LINE) && __has_intrinsics_for(builtin_FUNCTION) && __has_intrinsics_for(builtin_FILE) && __has_intrinsics_for(builtin_COLUMN)
    struct source_location {
    private:
        constexpr source_location(uint_least32_t line, uint_least32_t column, const char* file_name, const char* function_name)
            : line_v(line), column_v(column), file_name_v(file_name), function_name_v(function_name) {}

    public:
        // TODO: Wait for clang's full support for consteval functions.
        static constexpr source_location current(
            uint_least32_t line = __builtin_LINE(), 
            uint_least32_t column = __builtin_COLUMN(), 
            const char* file_name = __builtin_FILE(), 
            const char* function_name = __builtin_FUNCTION()) noexcept {
            return source_location(line, column, file_name, function_name);
        }

        constexpr source_location() noexcept
            : line_v(0), column_v(0), file_name_v(""), function_name_v("") {}

        constexpr uint_least32_t line() const noexcept { return line_v; }
        constexpr uint_least32_t column() const noexcept { return column_v; };
        constexpr const char* file_name() const noexcept { return file_name_v; };
        constexpr const char* function_name() const noexcept { return function_name_v; };

    private:
        uint_least32_t line_v;
        uint_least32_t column_v;
        const char* file_name_v;
        const char* function_name_v;
    };
#endif
}