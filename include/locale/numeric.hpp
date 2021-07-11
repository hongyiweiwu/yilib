#pragma once

#include "ios.hpp"
#include "type_traits.hpp"
#include "cstddef.hpp"
#include "memory.hpp"
#include "concepts.hpp"
#include "cstring.hpp"
#include "utility.hpp"
#include "cstdlib.hpp"
#include "cstdio.hpp"
#include "cwchar.hpp"
#include "limits.hpp"

namespace std {
    /* 28.4.3 The numeric category */
    namespace __internal {
        /* Base class for all num_get classes. Provides implementation for all public methods in the class, which simply calls do_xxx version
         * of themselves. */
        template<class charT, class InputIterator = istreambuf_iterator<charT>>
        class __num_get_impl {
        public:
            using char_type = charT;
            using iter_type = InputIterator;

            iter_type get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, bool& v) const { return do_get(in, end, str, err, v); }
            iter_type get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, long& v) const { return do_get(in, end, str, err, v); }
            iter_type get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, long long& v) const { return do_get(in, end, str, err, v); }
            iter_type get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned short& v) const { return do_get(in, end, str, err, v); }
            iter_type get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned int& v) const { return do_get(in, end, str, err, v); }
            iter_type get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned long& v) const { return do_get(in, end, str, err, v); }
            iter_type get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned long long& v) const { return do_get(in, end, str, err, v); }
            iter_type get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, float& v) const { return do_get(in, end, str, err, v); }
            iter_type get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, double& v) const { return do_get(in, end, str, err, v); }
            iter_type get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, long double& v) const { return do_get(in, end, str, err, v); }
            iter_type get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, void*& v) const { return do_get(in, end, str, err, v); }

        protected:
            virtual iter_type do_get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, bool& v) const = 0;
            virtual iter_type do_get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, long& v) const = 0;
            virtual iter_type do_get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, long long& v) const = 0;
            virtual iter_type do_get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, unsigned short& v) const = 0;
            virtual iter_type do_get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, unsigned int& v) const = 0;
            virtual iter_type do_get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, unsigned long& v) const = 0;
            virtual iter_type do_get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, unsigned long long& v) const = 0;
            virtual iter_type do_get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, float& v) const = 0;
            virtual iter_type do_get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, double& v) const = 0;
            virtual iter_type do_get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, long double& v) const = 0;
            virtual iter_type do_get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, void*& v) const = 0;

            /* Returns the base that the supplied ios_base suggests to use. */
            static std::size_t get_base(ios_base& str) noexcept {
                switch (str.flags() & ios_base::basefield) {
                    case ios_base::oct:
                        return 8;

                    case ios_base::hex:
                        return 16;

                    default:
                        return 0;
                }
            }

            /* Checks whether the placements of the thousands-separator in thousands_sep_pos_arr follows the specified grouping. The thousnads_sep_pos_arr field
            * should be terminated by -1. If the grouping isn't properly observed, set err to failbit. */
            static constexpr void check_thousands_sep_grouping(const string&, int*, ios_base::iostate&) noexcept  {
                // TODO: Implement.
            }
        };
    }

    template<class charT, class InputIterator = istreambuf_iterator<charT>>
    class num_get : public locale::facet, public __internal::__num_get_impl<charT, InputIterator> {
    public:
        using char_type = charT;
        using iter_type = InputIterator;

        explicit num_get(std::size_t refs = 0) : locale::facet(refs), __internal::__num_get_impl<charT, InputIterator>() {}

        static inline locale::id id;

    protected:
        ~num_get() = default;
        iter_type do_get(iter_type in, iter_type, ios_base&, ios_base::iostate&, bool&) const override { return in; }
        iter_type do_get(iter_type in, iter_type, ios_base&, ios_base::iostate&, long&) const override { return in; }
        iter_type do_get(iter_type in, iter_type, ios_base&, ios_base::iostate&, long long&) const override { return in; }
        iter_type do_get(iter_type in, iter_type, ios_base&, ios_base::iostate&, unsigned short&) const override { return in; }
        iter_type do_get(iter_type in, iter_type, ios_base&, ios_base::iostate&, unsigned int&) const override { return in; }
        iter_type do_get(iter_type in, iter_type, ios_base&, ios_base::iostate&, unsigned long&) const override { return in; }
        iter_type do_get(iter_type in, iter_type, ios_base&, ios_base::iostate&, unsigned long long&) const override { return in; }
        iter_type do_get(iter_type in, iter_type, ios_base&, ios_base::iostate&, float&) const override { return in; }
        iter_type do_get(iter_type in, iter_type, ios_base&, ios_base::iostate&, double&) const override { return in; }
        iter_type do_get(iter_type in, iter_type, ios_base&, ios_base::iostate&, long double&) const override { return in; }
        iter_type do_get(iter_type in, iter_type, ios_base&, ios_base::iostate&, void*&) const override { return in; }
    };

    template<class InputIterator>
    class num_get<char, InputIterator> : public locale::facet, public __internal::__num_get_impl<char, InputIterator> {
    public:
        using char_type = char;
        using iter_type = InputIterator;

        explicit num_get(std::size_t refs = 0) : locale::facet(refs), __internal::__num_get_impl<char, InputIterator>() {}

        static inline locale::id id;

    private:
        /* Checks whether the given character is accepted as valid character by the C-style str-to-T function in the given base. */
        template<signed_integral T>
        static constexpr bool is_valid_char(char c, std::size_t base) noexcept {
            switch (base) {
                case 8:
                    return std::strchr("01234567+-", c) != nullptr;
                case 10:
                    return std::strchr("0123456789+-", c) != nullptr;
                default:
                    return true;
            }
        }

        /* Checks whether the given character is accepted as valid character by the C-style str-to-T function in the given base. */
        template<class T> requires unsigned_integral<T> || is_same_v<T, void*>
        static constexpr bool is_valid_char(char c, std::size_t base) noexcept  {
            switch (base) {
                case 8:
                    return std::strchr("01234567+", c) != nullptr;
                case 10:
                    return std::strchr("0123456789+", c) != nullptr;
                default:
                    return std::strchr("0123456789abcdefxABCDEFX+", c) != nullptr;
            }
        }

        /* Checks whether the given character is accepted as valid character by the C-style str-to-T function in the given base. */
        template<floating_point T>
        static constexpr bool is_valid_char(char, std::size_t) noexcept {
            return true;
        }

        /* Builds a C-string that will be passed into the C-style str-to-T function. This corresponds to the stage 2 of do_get() in the standard.
         * Returns the built string as well as an int array containing where all the thousands seperators appeared, followed by an -1 as a terminate
         * sentinel. */
        template<class T>
        static pair<unique_ptr<char[]>, unique_ptr<int[]>> build_source_str(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err) noexcept;

        /* Calls the C-style string parsing functions to parse the numeric string stored in buf to a number of type T. Stores the result in v. 
         * This method corresponds to the "Stage-3" conversion for do_get as described in the standard. */
        template<signed_integral T>
        static constexpr void do_conversion(const char* buf, std::size_t base, T& v, ios_base::iostate& err) noexcept  {
            char* str_end_arg = nullptr;
            const long long ret = std::strtoll(buf, &str_end_arg, base);
            if (errno == ERANGE || str_end_arg == nullptr) {
                v = ret;
                err = ios_base::failbit;
            } else if (v > numeric_limits<T>::max()) {
                v = numeric_limits<T>::max();
                err = ios_base::failbit;
            } else if (v < numeric_limits<T>::min()) {
                v = numeric_limits<T>::min();
                err = ios_base::failbit;
            } else {
                v = ret;
            }
        }

        /* Calls the C-style string parsing functions to parse the numeric string stored in buf to a number of type T. Stores the result in v. 
         * This method corresponds to the "Stage-3" conversion for do_get as described in the standard. */
        template<unsigned_integral T>
        static constexpr void do_conversion(const char* buf, std::size_t base, T& v, ios_base::iostate& err) noexcept {
            char* str_end_arg = nullptr;
            const unsigned long long ret = std::strtoull(buf, &str_end_arg, base);
            if (errno == ERANGE || str_end_arg == nullptr) {
                v = ret;
                err = ios_base::failbit;
            } else if (v > numeric_limits<T>::max()) {
                v = numeric_limits<T>::max();
                err = ios_base::failbit;
            } else {
                v = ret;
            }
        }

        /* Calls the C-style string parsing functions to parse the numeric string stored in buf to a number of type T. Stores the result in v. 
         * This method corresponds to the "Stage-3" conversion for do_get as described in the standard. */
        template<floating_point T>
        static constexpr void do_conversion(const char* buf, std::size_t, T& v, ios_base::iostate& err) noexcept {
            char* str_end_arg = nullptr;

            if constexpr (is_same_v<T, float>) {
                v = std::strtof(buf, &str_end_arg);
            } else if constexpr (is_same_v<T, double>) {
                v = std::strtod(buf, &str_end_arg);
            } else {
                v = std::strtold(buf, &str_end_arg);
            }

            if (errno == ERANGE || str_end_arg == nullptr) {
                v = 0;
                err = ios_base::failbit;
            }
        }

    protected:
        ~num_get() = default;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, bool& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, long& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, long long& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned short& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned int& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned long& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned long long& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, float& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, double& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, long double& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, void*& v) const override;
    };

    extern template class num_get<char>;

    template<class InputIterator>
    class num_get<wchar_t, InputIterator> : public locale::facet, public __internal::__num_get_impl<wchar_t, InputIterator> {
    public:
        using char_type = wchar_t;
        using iter_type = InputIterator;

        explicit num_get(std::size_t refs = 0) : locale::facet(refs), __internal::__num_get_impl<wchar_t, InputIterator>() {}

        static inline locale::id id;

    private:
        /* Checks whether the given character is accepted as valid character by the C-style str-to-T function in the given base. */
        template<signed_integral T>
        static constexpr bool is_valid_char(wchar_t c, std::size_t base) noexcept {
            switch (base) {
                case 8:
                    return std::wcschr(L"01234567+-", c) != nullptr;
                case 10:
                    return std::wcschr(L"0123456789+-", c) != nullptr;
                default:
                    return true;
            }
        }

        /* Checks whether the given character is accepted as valid character by the C-style str-to-T function in the given base. */
        template<class T> requires unsigned_integral<T> || is_same_v<T, void*>
        static constexpr bool is_valid_char(wchar_t c, std::size_t base) noexcept  {
            switch (base) {
                case 8:
                    return std::wcschr(L"01234567+", c) != nullptr;
                case 10:
                    return std::wcschr(L"0123456789+", c) != nullptr;
                default:
                    return std::wcschr(L"0123456789abcdefxABCDEFX+", c) != nullptr;
            }
        }

        /* Checks whether the given character is accepted as valid character by the C-style str-to-T function in the given base. */
        template<floating_point T>
        static constexpr bool is_valid_char(wchar_t, std::size_t) noexcept {
            return true;
        }

        /* Builds a C-string that will be passed into the C-style str-to-T function. This corresponds to the stage 2 of do_get() in the standard.
         * Returns the built string as well as an int array containing where all the thousands seperators appeared, followed by an -1 as a terminate
         * sentinel. */
        template<class T>
        static pair<unique_ptr<wchar_t[]>, unique_ptr<int[]>> build_source_str(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err) noexcept;

        /* Calls the C-style string parsing functions to parse the numeric string stored in buf to a number of type T. Stores the result in v. 
         * This method corresponds to the "Stage-3" conversion for do_get as described in the standard. */
        template<signed_integral T>
        static constexpr void do_conversion(const wchar_t* buf, std::size_t base, T& v, ios_base::iostate& err) noexcept  {
            wchar_t* str_end_arg = nullptr;
            const long long ret = std::wcstoll(buf, &str_end_arg, base);
            if (errno == ERANGE || str_end_arg == nullptr) {
                v = ret;
                err = ios_base::failbit;
            } else if (v > numeric_limits<T>::max()) {
                v = numeric_limits<T>::max();
                err = ios_base::failbit;
            } else if (v < numeric_limits<T>::min()) {
                v = numeric_limits<T>::min();
                err = ios_base::failbit;
            } else {
                v = ret;
            }
        }

        /* Calls the C-style string parsing functions to parse the numeric string stored in buf to a number of type T. Stores the result in v. 
         * This method corresponds to the "Stage-3" conversion for do_get as described in the standard. */
        template<unsigned_integral T>
        static constexpr void do_conversion(const wchar_t* buf, std::size_t base, T& v, ios_base::iostate& err) noexcept {
            wchar_t* str_end_arg = nullptr;
            const unsigned long long ret = std::wcstoull(buf, &str_end_arg, base);
            if (errno == ERANGE || str_end_arg == nullptr) {
                v = ret;
                err = ios_base::failbit;
            } else if (v > numeric_limits<T>::max()) {
                v = numeric_limits<T>::max();
                err = ios_base::failbit;
            } else {
                v = ret;
            }
        }

        /* Calls the C-style string parsing functions to parse the numeric string stored in buf to a number of type T. Stores the result in v. 
         * This method corresponds to the "Stage-3" conversion for do_get as described in the standard. */
        template<floating_point T>
        static constexpr void do_conversion(const wchar_t* buf, std::size_t, T& v, ios_base::iostate& err) noexcept {
            wchar_t* str_end_arg = nullptr;

            if constexpr (is_same_v<T, float>) {
                v = std::wcstof(buf, &str_end_arg);
            } else if constexpr (is_same_v<T, double>) {
                v = std::wcstod(buf, &str_end_arg);
            } else {
                v = std::wcstold(buf, &str_end_arg);
            }

            if (errno == ERANGE || str_end_arg == nullptr) {
                v = 0;
                err = ios_base::failbit;
            }
        }

    protected:
        ~num_get() = default;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, bool& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, long& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, long long& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned short& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned int& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned long& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned long long& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, float& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, double& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, long double& v) const override;
        iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, void*& v) const override;
    };

    extern template class num_get<wchar_t>;

    /* 28.4.4 The numeric punctuation facet */
    namespace __internal {
        /* Base class for all numpunct classes. Provides implementation for all public methods in the class, which simply calls do_xxx version of 
         * themselves. */
        template<class charT>
        struct __numpunct_impl {
        public:
            using char_type = charT;
            using string_type = basic_string<charT>;

            char_type decimal_point() const {
                return do_decimal_point();
            }

            char_type thousands_sep() const {
                return do_thousands_sep();
            }

            string grouping() const {
                return do_grouping();
            }

            string_type truename() const {
                return do_truename();
            }

            string_type falsename() const {
                return do_falsename();
            }

        protected:
            virtual char_type do_decimal_point() const = 0;
            virtual char_type do_thousands_sep() const = 0;
            virtual string do_grouping() const = 0;
            virtual string_type do_truename() const = 0;
            virtual string_type do_falsename() const = 0;
        };
    }

    template<class charT>
    class numpunct : public locale::facet, public __internal::__numpunct_impl<charT> {
    public:
        using char_type = typename __internal::__numpunct_impl<charT>::char_type;
        using string_type = typename __internal::__numpunct_impl<charT>::string_type;
        explicit numpunct(std::size_t refs = 0) : locale::facet(refs), __internal::__numpunct_impl<charT>() {}

        static locale::id id;

    protected:
        ~numpunct() = default;
        char_type do_decimal_point() const override { return 0; }
        char_type do_thousands_sep() const override { return 0; }
        string do_grouping() const override { return ""; }
        string_type do_truename() const override { return string_type(); }
        string_type do_falsename() const override { return string_type(); }
    };

    template<>
    class numpunct<char> : public locale::facet, public __internal::__numpunct_impl<char> {
    public:
        using char_type = typename __internal::__numpunct_impl<char>::char_type;
        using string_type = typename __internal::__numpunct_impl<char>::string_type;
        explicit numpunct(std::size_t refs = 0);

        static locale::id id;

    protected:
        char_type do_decimal_point() const override;
        char_type do_thousands_sep() const override;
        string do_grouping() const override;
        string_type do_truename() const override;
        string_type do_falsename() const override;
    };

    template<>
    class numpunct<wchar_t> : public locale::facet, public __internal::__numpunct_impl<wchar_t> {
    public:
        using char_type = typename __internal::__numpunct_impl<wchar_t>::char_type;
        using string_type = typename __internal::__numpunct_impl<wchar_t>::string_type;
        explicit numpunct(std::size_t refs = 0);

        static locale::id id;

    protected:
        char_type do_decimal_point() const override;
        char_type do_thousands_sep() const override;
        string do_grouping() const override;
        string_type do_truename() const override;
        string_type do_falsename() const override;
    };

    template<class charT>
    class numpunct_byname : public numpunct<charT>, public __internal::__locale_container<> {
    public:
        using char_type = typename numpunct<charT>::char_type;
        using string_type = typename numpunct<charT>::string_type;

        explicit numpunct_byname(const char* loc, std::size_t refs = 0) : numpunct<charT>(refs), 
            __internal::__locale_container<>(loc, LC_NUMERIC) {}
        explicit numpunct_byname(const string& loc, std::size_t refs = 0) : numpunct_byname(loc.c_str(), refs) {}

    protected:
        ~numpunct_byname() = default;
    };

    template<>
    class numpunct_byname<char> : public numpunct<char>, public __internal::__locale_container<> {
    public:
        using char_type = typename numpunct<char>::char_type;
        using string_type = typename numpunct<char>::string_type;

        explicit numpunct_byname(const char* loc, std::size_t refs = 0);
        explicit numpunct_byname(const string& loc, std::size_t refs = 0);

    protected:
        ~numpunct_byname() = default;

        char_type do_decimal_point() const override;
        char_type do_thousands_sep() const override;
        string do_grouping() const override;
        string_type do_truename() const override;
        string_type do_falsename() const override;
    };

    template<>
    class numpunct_byname<wchar_t> : public numpunct<wchar_t>, public __internal::__locale_container<> {
    public:
        using char_type = typename numpunct<wchar_t>::char_type;
        using string_type = typename numpunct<wchar_t>::string_type;

        explicit numpunct_byname(const char* loc, std::size_t refs = 0);
        explicit numpunct_byname(const string& loc, std::size_t refs = 0);

    protected:
        ~numpunct_byname() = default;

        char_type do_decimal_point() const override;
        char_type do_thousands_sep() const override;
        string do_grouping() const override;
        string_type do_truename() const override;
        string_type do_falsename() const override;
    };

    /* Implementation */
    template<class InputIterator>
    template<class T>
    pair<unique_ptr<char[]>, unique_ptr<int[]>> num_get<char, InputIterator>::build_source_str(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err) noexcept {
        unique_ptr<char[]> buf = make_unique<char[]>(16);
        std::size_t buf_size = 16;
        std::size_t buf_curr_pos = 0;

        unique_ptr<int[]> thousands_sep_pos_arr = make_unique<int[]>(4);
        std::size_t thousands_sep_pos_arr_size = 4;
        std::size_t thousands_sep_pos_arr_curr_pos = 0;

        const locale& loc = str.getloc();
        bool dot_accumulated = false;

        static constexpr auto emplace_to_array = []<class ElemT>(unique_ptr<ElemT[]>& arr, ElemT elem, std::size_t curr_pos, std::size_t arr_size) {
            if (curr_pos == arr_size) {
                // Needs to expand the size of the buffer.
                unique_ptr<ElemT[]> new_arr = make_unique<ElemT[]>(arr_size * 2);
                std::memcpy(new_arr.get(), arr.get(), sizeof(ElemT) * arr_size);
                arr.swap(new_arr);
                arr_size *= 2;
            }

            arr[curr_pos] = elem;
            curr_pos++;
        };

        while (in != end) {
            const char ct = *in;
            if (std::strchr("0123456789abcdefxABCDEFX+-", ct) != nullptr) {
                if (is_valid_char<T>(ct, num_get<char, InputIterator>::get_base(str))) {
                    emplace_to_array(buf, ct, buf_curr_pos, buf_size);
                    buf_curr_pos++;
                } else {
                    break;
                }
            } else if (ct == use_facet<numpunct<char>>(loc).decimal_point()) {
                if (is_valid_char<T>('.', num_get<char, InputIterator>::get_base(str))) {
                    emplace_to_array(buf, '.', buf_curr_pos, buf_size);
                    buf_curr_pos++;
                    dot_accumulated = true;
                } else {
                    break;
                }
            } else if (ct == use_facet<numpunct<char>>(loc).thousands_sep() && use_facet<numpunct<char>>(loc).grouping().length() != 0) {
                if (dot_accumulated) {
                    break;
                } else {
                    emplace_to_array(thousands_sep_pos_arr, static_cast<int>(buf_curr_pos), thousands_sep_pos_arr_curr_pos, thousands_sep_pos_arr_size);
                }
            }

            ++in;
        }

        if (in == end) {
            err |= ios_base::eofbit;
        }

        emplace_to_array(buf, '\0', buf_curr_pos, buf_size);
        emplace_to_array(thousands_sep_pos_arr, -1, thousands_sep_pos_arr_curr_pos, thousands_sep_pos_arr_size);
        return { move(buf), move(thousands_sep_pos_arr) };
    }

    template<class InputIterator>
    InputIterator num_get<char, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, bool& v) const {
        if ((str.flags() & ios_base::boolalpha) == 0) {
            long tmp = 0;
            do_get(in, end, str, err, tmp);
            v = tmp;
            if (tmp != 0 && tmp != 1) {
                err = ios_base::failbit;
            }

            return in;
        } else {
            const string truename = use_facet<numpunct<char>>(str.getloc()).truename();
            const string falsename = use_facet<numpunct<char>>(str.getloc()).falsename();

            const char* truename_cstr = truename.c_str();
            const char* falsename_cstr = falsename.c_str();

            bool truename_works = true;
            bool falsename_works = true;
            bool resolved = false;

            while (in != end && !resolved) {
                const char ct = *in;
                if (truename_works) {
                    if (ct != *truename_cstr) {
                        truename_works = false;
                    } else {
                        truename_cstr++;
                        if (*truename_cstr == '\0') {
                            // We found a hit.
                            v = true;
                            resolved = true;
                        }
                    }
                }

                if (falsename_works) {
                    if (ct != *falsename_cstr) {
                        falsename_works = false;
                    } else {
                        falsename_cstr++;
                        if (*falsename_cstr == '\0') {
                            v = false;
                            if (resolved) {
                                // We have a nonunique match. This counts as failure.
                                err = ios_base::failbit;
                                resolved = false;
                            } else {
                                resolved = true;
                            }
                        }
                    }
                }

                ++in;
            }

            err = resolved ? ios_base::goodbit : ios_base::failbit;
            if (in == end) {
                err |= ios_base::eofbit;
            }
            return in;
        }
    }

    template<class InputIterator>
    InputIterator num_get<char, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, long& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<long>(in, end, str, err);
        num_get<char, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<char>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        do_conversion<long>(buf.get(), num_get<char, InputIterator>::get_base(str), v, err);
        return in;
    }

    template<class InputIterator>
    InputIterator num_get<char, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, long long& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<long long>(in, end, str, err);
        num_get<char, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<char>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        do_conversion<long long>(buf.get(), num_get<char, InputIterator>::get_base(str), v, err);
        return in;
    }

    template<class InputIterator>
    InputIterator num_get<char, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned short& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<unsigned short>(in, end, str, err);
        num_get<char, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<char>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        do_conversion<unsigned short>(buf.get(), num_get<char, InputIterator>::get_base(str), v, err);
        return in;
    }

    template<class InputIterator>
    InputIterator num_get<char, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned int& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<unsigned int>(in, end, str, err);
        num_get<char, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<char>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        do_conversion<unsigned int>(buf.get(), num_get<char, InputIterator>::get_base(str), v, err);
        return in;
    }

    template<class InputIterator>
    InputIterator num_get<char, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned long& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<unsigned long>(in, end, str, err);
        num_get<char, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<char>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        do_conversion<unsigned long>(buf.get(), num_get<char, InputIterator>::get_base(str), v, err);
        return in;
    }

    template<class InputIterator>
    InputIterator num_get<char, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned long long& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<unsigned long long>(in, end, str, err);
        num_get<char, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<char>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        do_conversion<unsigned long long>(buf.get(), num_get<char, InputIterator>::get_base(str), v, err);
        return in;
    }

    template<class InputIterator>
    InputIterator num_get<char, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, float& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<float>(in, end, str, err);
        num_get<char, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<char>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        do_conversion<float>(buf.get(), num_get<char, InputIterator>::get_base(str), v, err);
        return in;
    }

    template<class InputIterator>
    InputIterator num_get<char, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, double& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<double>(in, end, str, err);
        num_get<char, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<char>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        do_conversion<double>(buf.get(), num_get<char, InputIterator>::get_base(str), v, err);
        return in;
    }

    template<class InputIterator>
    InputIterator num_get<char, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, long double& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<long double>(in, end, str, err);
        num_get<char, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<char>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        do_conversion<long double>(buf.get(), num_get<char, InputIterator>::get_base(str), v, err);
        return in;

    }

    template<class InputIterator>
    InputIterator num_get<char, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, void*& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<void*>(in, end, str, err);
        num_get<char, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<char>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        const int result = std::sscanf(buf.get(), "%p", &v);
        if (result == EOF) {
            v = nullptr;
            err = ios_base::failbit;
        }
        return in;
    }

    template<class InputIterator>
    template<class T>
    pair<unique_ptr<wchar_t[]>, unique_ptr<int[]>> num_get<wchar_t, InputIterator>::build_source_str(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err) noexcept {
        unique_ptr<wchar_t[]> buf = make_unique<wchar_t[]>(16);
        std::size_t buf_size = 16;
        std::size_t buf_curr_pos = 0;

        unique_ptr<int[]> thousands_sep_pos_arr = make_unique<int[]>(4);
        std::size_t thousands_sep_pos_arr_size = 4;
        std::size_t thousands_sep_pos_arr_curr_pos = 0;

        const locale& loc = str.getloc();
        bool dot_accumulated = false;

        static constexpr auto emplace_to_array = []<class ElemT>(unique_ptr<ElemT[]>& arr, ElemT elem, std::size_t curr_pos, std::size_t arr_size) {
            if (curr_pos == arr_size) {
                // Needs to expand the size of the buffer.
                unique_ptr<ElemT[]> new_arr = make_unique<ElemT[]>(arr_size * 2);
                std::memcpy(new_arr.get(), arr.get(), sizeof(ElemT) * arr_size);
                arr.swap(new_arr);
                arr_size *= 2;
            }

            arr[curr_pos] = elem;
            curr_pos++;
        };

        while (in != end) {
            const wchar_t ct = *in;
            if (std::wcschr(L"0123456789abcdefxABCDEFX+-", ct) != nullptr) {
                if (is_valid_char<T>(ct, num_get<wchar_t, InputIterator>::get_base(str))) {
                    emplace_to_array(buf, ct, buf_curr_pos, buf_size);
                    buf_curr_pos++;
                } else {
                    break;
                }
            } else if (ct == use_facet<numpunct<wchar_t>>(loc).decimal_point()) {
                if (is_valid_char<T>(L'.', num_get<wchar_t, InputIterator>::get_base(str))) {
                    emplace_to_array(buf, L'.', buf_curr_pos, buf_size);
                    buf_curr_pos++;
                    dot_accumulated = true;
                } else {
                    break;
                }
            } else if (ct == use_facet<numpunct<wchar_t>>(loc).thousands_sep() && use_facet<numpunct<wchar_t>>(loc).grouping().length() != 0) {
                if (dot_accumulated) {
                    break;
                } else {
                    emplace_to_array(thousands_sep_pos_arr, static_cast<int>(buf_curr_pos), thousands_sep_pos_arr_curr_pos, thousands_sep_pos_arr_size);
                }
            }

            ++in;
        }

        if (in == end) {
            err |= ios_base::eofbit;
        }

        emplace_to_array(buf, L'\0', buf_curr_pos, buf_size);
        emplace_to_array(thousands_sep_pos_arr, -1, thousands_sep_pos_arr_curr_pos, thousands_sep_pos_arr_size);
        return { move(buf), move(thousands_sep_pos_arr) };
    }

    template<class InputIterator>
    InputIterator num_get<wchar_t, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, bool& v) const {
        if ((str.flags() & ios_base::boolalpha) == 0) {
            long tmp = 0;
            do_get(in, end, str, err, tmp);
            v = tmp;
            if (tmp != 0 && tmp != 1) {
                err = ios_base::failbit;
            }

            return in;
        } else {
            const wstring truename = use_facet<numpunct<wchar_t>>(str.getloc()).truename();
            const wstring falsename = use_facet<numpunct<wchar_t>>(str.getloc()).falsename();

            const wchar_t* truename_cstr = truename.c_str();
            const wchar_t* falsename_cstr = falsename.c_str();

            bool truename_works = true;
            bool falsename_works = true;
            bool resolved = false;

            while (in != end && !resolved) {
                const wchar_t ct = *in;
                if (truename_works) {
                    if (ct != *truename_cstr) {
                        truename_works = false;
                    } else {
                        truename_cstr++;
                        if (*truename_cstr == L'\0') {
                            // We found a hit.
                            v = true;
                            resolved = true;
                        }
                    }
                }

                if (falsename_works) {
                    if (ct != *falsename_cstr) {
                        falsename_works = false;
                    } else {
                        falsename_cstr++;
                        if (*falsename_cstr == L'\0') {
                            v = false;
                            if (resolved) {
                                // We have a nonunique match. This counts as failure.
                                err = ios_base::failbit;
                                resolved = false;
                            } else {
                                resolved = true;
                            }
                        }
                    }
                }

                ++in;
            }

            err = resolved ? ios_base::goodbit : ios_base::failbit;
            if (in == end) {
                err |= ios_base::eofbit;
            }
            return in;
        }
    }

    template<class InputIterator>
    InputIterator num_get<wchar_t, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, long& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<long>(in, end, str, err);
        num_get<wchar_t, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<wchar_t>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        do_conversion<long>(buf.get(), num_get<wchar_t, InputIterator>::get_base(str), v, err);
        return in;
    }

    template<class InputIterator>
    InputIterator num_get<wchar_t, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, long long& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<long long>(in, end, str, err);
        num_get<wchar_t, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<wchar_t>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        do_conversion<long long>(buf.get(), num_get<wchar_t, InputIterator>::get_base(str), v, err);
        return in;
    }

    template<class InputIterator>
    InputIterator num_get<wchar_t, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned short& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<unsigned short>(in, end, str, err);
        num_get<wchar_t, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<wchar_t>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        do_conversion<unsigned short>(buf.get(), num_get<wchar_t, InputIterator>::get_base(str), v, err);
        return in;
    }

    template<class InputIterator>
    InputIterator num_get<wchar_t, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned int& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<unsigned int>(in, end, str, err);
        num_get<wchar_t, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<wchar_t>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        do_conversion<unsigned int>(buf.get(), num_get<wchar_t, InputIterator>::get_base(str), v, err);
        return in;
    }

    template<class InputIterator>
    InputIterator num_get<wchar_t, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned long& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<unsigned long>(in, end, str, err);
        num_get<wchar_t, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<wchar_t>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        do_conversion<unsigned long>(buf.get(), num_get<wchar_t, InputIterator>::get_base(str), v, err);
        return in;
    }

    template<class InputIterator>
    InputIterator num_get<wchar_t, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, unsigned long long& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<unsigned long long>(in, end, str, err);
        num_get<wchar_t, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<wchar_t>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        do_conversion<unsigned long long>(buf.get(), num_get<wchar_t, InputIterator>::get_base(str), v, err);
        return in;
    }

    template<class InputIterator>
    InputIterator num_get<wchar_t, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, float& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<float>(in, end, str, err);
        num_get<wchar_t, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<wchar_t>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        do_conversion<float>(buf.get(), num_get<wchar_t, InputIterator>::get_base(str), v, err);
        return in;
    }

    template<class InputIterator>
    InputIterator num_get<wchar_t, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, double& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<double>(in, end, str, err);
        num_get<wchar_t, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<wchar_t>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        do_conversion<double>(buf.get(), num_get<wchar_t, InputIterator>::get_base(str), v, err);
        return in;
    }

    template<class InputIterator>
    InputIterator num_get<wchar_t, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, long double& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<long double>(in, end, str, err);
        num_get<wchar_t, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<wchar_t>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        do_conversion<long double>(buf.get(), num_get<wchar_t, InputIterator>::get_base(str), v, err);
        return in;

    }

    template<class InputIterator>
    InputIterator num_get<wchar_t, InputIterator>::do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, void*& v) const {
        const auto [buf, thousands_sep_pos_arr] = build_source_str<void*>(in, end, str, err);
        num_get<wchar_t, InputIterator>::check_thousands_sep_grouping(use_facet<numpunct<wchar_t>>(str.getloc()).grouping(), thousands_sep_pos_arr.get(), err);
        const int result = std::swscanf(buf.get(), L"%p", &v);
        if (result == EOF) {
            v = nullptr;
            err = ios_base::failbit;
        }
        return in;
    }
}