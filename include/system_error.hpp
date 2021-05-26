#pragma once

#include "compare.hpp"
#include "cerrno.hpp"
#include "string.hpp"
#include "type_traits.hpp"
#include "cstddef.hpp"
#include "functional.hpp"
#include "stdexcept.hpp"

namespace std {
    // Forward declaration. Implementation given below.
    class error_code;
    class error_condition;
    /* 19.5.3 Class error_category */
    class error_category {
    public:
        constexpr error_category() noexcept = default;
        virtual ~error_category() = default;
        error_category(const error_category&) = delete;
        error_category& operator=(const error_category&) = delete;
        virtual const char* name() const noexcept = 0;
        virtual error_condition default_error_condition(int ev) const noexcept;
        virtual bool equivalent(int code, const error_condition& condition) const noexcept;
        virtual bool equivalent(const error_code& code, int condition) const noexcept;
        virtual string message(int ev) const = 0;

        bool operator==(const error_category& rhs) const noexcept;
        strong_ordering operator<=>(const error_category& rhs) const noexcept;
    };

    const error_category& generic_category() noexcept;
    const error_category& system_category() noexcept;

    template<class T> struct is_error_code_enum : public false_type {};
    template<class T> inline constexpr bool is_error_code_enum_v = is_error_code_enum<T>::value;

    /* 19.5.4 Class error_code */
    class error_code {
    public:
        error_code() noexcept;
        error_code(int val, const error_category& cat) noexcept;
        template<class ErrorCodeEnum> requires is_error_code_enum_v<ErrorCodeEnum>
        error_code(ErrorCodeEnum e) noexcept : val(static_cast<int>(e)), cat(&generic_category()) {}

        void assign(int val, const error_category& cat) noexcept;
        template<class ErrorCodeEnum> requires is_error_code_enum_v<ErrorCodeEnum>
        error_code& operator=(ErrorCodeEnum e) noexcept {
            val = (static_cast<int>(e));
            cat = &generic_category();
            return *this;
        }

        void clear();

        int value() const noexcept;
        const error_category& category() const noexcept;
        error_condition default_error_condition() const noexcept;
        string message() const;
        explicit operator bool() const noexcept;

    private:
        int val;
        const error_category* cat;
    };

    template<class T> struct is_error_condition_enum : public false_type {};
    template<class T> inline constexpr bool is_error_condition_enum_v = is_error_condition_enum<T>::value;

    /* 19.5.5 Class error_condition */
    class error_condition {
    public:
        error_condition() noexcept;
        error_condition(int val, const error_category& cat) noexcept;
        template<class ErrorConditionEnum> requires is_error_condition_enum_v<ErrorConditionEnum>
        error_condition(ErrorConditionEnum e) noexcept : val(static_cast<int>(e)), cat(&generic_category()) {}

        void assign(int val, const error_category& cat) noexcept;
        template<class ErrorConditionEnum> requires is_error_condition_enum_v<ErrorConditionEnum>
        error_condition& operator=(ErrorConditionEnum e) noexcept {
            val = (static_cast<int>(e));
            cat = &generic_category();
            return *this;
        }

        void clear();

        int value() const noexcept;
        const error_category& category() const noexcept;
        string message() const;
        explicit operator bool() const noexcept;

    private:
        int val;
        const error_category* cat;
    };

    class system_error : public runtime_error {
    public:
        system_error(error_code ec, const string& what_arg);
        system_error(error_code ec, const char* what_arg);
        system_error(error_code ec);
        system_error(int ev, const error_category& ecat, const string& what_arg);
        system_error(int ev, const error_category& ecat, const char* what_arg);
        system_error(int ev, const error_category& ecat);
        const error_code& code() const noexcept;
        const char* what() const noexcept override;

    private:
        error_code ec;
    };

    enum class errc {
        address_family_not_supported = EAFNOSUPPORT,
        address_in_use = EADDRINUSE,
        address_not_available = EADDRNOTAVAIL,
        already_connected = EISCONN,
        argument_list_too_long = E2BIG,
        argument_out_of_domain = EDOM,
        bad_address = EFAULT,
        bad_file_descriptor = EBADF,
        bad_message = EBADMSG,
        broken_pipe = EPIPE,
        connection_aborted = ECONNABORTED,
        connection_already_in_progress = EALREADY,
        connection_refused = ECONNREFUSED,
        connection_reset = ECONNRESET,
        cross_device_link = EXDEV,
        destination_address_required = EDESTADDRREQ,
        device_or_resource_busy = EBUSY,
        directory_not_empty = ENOTEMPTY,
        executable_format_error = ENOEXEC,
        file_exists = EEXIST,
        file_too_large = EFBIG,
        filename_too_long = ENAMETOOLONG,
        function_not_supported = ENOSYS,
        host_unreachable = EHOSTUNREACH,
        identifier_removed = EIDRM,
        illegal_byte_sequence = EILSEQ,
        inappropriate_io_control_operation = ENOTTY,
        interrupted = EINTR,
        invalid_argument = EINVAL,
        invalid_seek = ESPIPE,
        io_error = EIO,
        is_a_directory = EISDIR,
        message_size = EMSGSIZE,
        network_down = ENETDOWN,
        network_reset = ENETRESET,
        network_unreachable = ENETUNREACH,
        no_buffer_space = ENOBUFS,
        no_child_process = ECHILD,
        no_link = ENOLINK,
        no_lock_available = ENOLCK,
        no_message_available = ENODATA,
        no_message = ENOMSG,
        no_protocol_option = ENOPROTOOPT,
        no_space_on_device = ENOSPC,
        no_stream_resources = ENOSR,
        no_such_device_or_address = ENXIO,
        no_such_device = ENODEV,
        no_such_file_or_directory = ENOENT,
        no_such_process = ESRCH,
        not_a_directory = ENOTDIR,
        not_a_socket = ENOTSOCK,
        not_a_stream = ENOSTR,
        not_connected = ENOTCONN,
        not_enough_memory = ENOMEM,
        not_supported = ENOTSUP,
        operation_canceled = ECANCELED,
        operation_in_progress = EINPROGRESS,
        operation_not_permitted = EPERM,
        operation_not_supported = EOPNOTSUPP,
        operation_would_block = EWOULDBLOCK,
        owner_dead = EOWNERDEAD,
        permission_denied = EACCES,
        protocol_error = EPROTO,
        protocol_not_supported = EPROTONOSUPPORT,
        read_only_file_system = EROFS,
        resource_deadlock_would_occur = EDEADLK,
        resource_unavailable_try_again = EAGAIN,
        result_out_of_range = ERANGE,
        state_not_recoverable = ENOTRECOVERABLE,
        stream_timeout = ETIME,
        text_file_busy = ETXTBSY,
        timed_out = ETIMEDOUT,
        too_many_files_open_in_system = ENFILE,
        too_many_files_open = EMFILE,
        too_many_links = EMLINK,
        too_many_symbolic_link_levels = ELOOP,
        value_too_large = EOVERFLOW,
        wrong_protocol_type = EPROTOTYPE
    };

    template<> struct is_error_condition_enum<errc> : true_type {};


    error_code make_error_code(errc e) noexcept;

    // TODO: Uncomment after <iostream> is implemented.
    /*
    template<class charT, class traits>
    basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const error_code& ec) {
        return os << ec.category().name() << ';' << ec.value();
    } */

    error_condition make_error_condition(errc e) noexcept;

    /* 19.5.6 Comparison operator functions */
    bool operator==(const error_code& lhs, const error_code& rhs) noexcept;
    bool operator==(const error_code& lhs, const error_condition& rhs) noexcept;
    bool operator==(const error_condition& lhs, const error_condition& rhs) noexcept;
    strong_ordering operator<=>(const error_code& lhs, const error_code& rhs) noexcept;
    strong_ordering operator<=>(const error_condition& lhs, const error_condition& rhs) noexcept;

    /* 19.5.7 Hash support */
    template<> struct hash<error_code> {
        size_t operator()(const error_code& code) const noexcept;
    };

    template<> struct hash<error_condition> {
        size_t operator()(const error_condition& cond) const noexcept;
    };
}