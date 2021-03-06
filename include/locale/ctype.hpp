#pragma once

#include "clocale.hpp"
#include "cstddef.hpp"
#include "tuple.hpp"

#include "locale/locale.hpp"
#include "locale/xlocale.hpp"

namespace std {
    /* 28.4.2 ctype */
    class ctype_base {
    public:
        using mask = unsigned long;

        static constexpr mask space = 1 << 0;
        static constexpr mask print = 1 << 1;
        static constexpr mask cntrl = 1 << 2;
        static constexpr mask upper = 1 << 3;
        static constexpr mask lower = 1 << 4;
        static constexpr mask alpha = 1 << 5;
        static constexpr mask digit = 1 << 6;
        static constexpr mask punct = 1 << 7;
        static constexpr mask xdigit = 1 << 8;
        static constexpr mask blank = 1 << 9;
        static constexpr mask alnum = alpha | digit;
        static constexpr mask graph = alnum | punct;
    };

    namespace __internal {
        /* Provides implementation of all public functions in ctype, since they all just call the protected do_xxx version
         * within the same class. */
        template<class charT>
        struct __ctype_impl : public ctype_base {
        public:
            bool is(mask m, charT c) const { return do_is(m, c); }
            const charT* is(const charT* low, const charT* high, mask* vec) const { return do_is(low, high, vec); }
            const charT* scan_is(mask m, const charT* low, const charT* high) const { return do_scan_is(m, low, high); }
            const charT* scan_not(mask m, const charT* low, const charT* high) const { return do_scan_not(m, low, high); }
            charT toupper(charT c) const { return do_toupper(c); }
            const charT* toupper(charT* low, const charT* high) const { return do_toupper(low, high); }
            charT tolower(charT c) const { return do_tolower(c); }
            const charT* tolower(charT* low, const charT* high) const { return do_tolower(low, high); }

            charT widen(char c) const { return do_widen(c); }
            const char* widen(const char* low, const char* high, charT* to) const { return do_widen(low, high, to); }
            char narrow(charT c, char dfault) const { return do_narrow(c, dfault); }
            const charT* narrow(const charT* low, const charT* high, char dfault, char* to) const { return do_narrow(low, high, dfault, to); }

        protected:
            virtual bool do_is(mask, charT) const { return false; };
            virtual const charT* do_is(const charT*, const charT*, mask*) const { return nullptr; }
            virtual const charT* do_scan_is(mask, const charT*, const charT*) const { return nullptr; }
            virtual const charT* do_scan_not(mask, const charT*, const charT*) const { return nullptr; }
            virtual charT do_toupper(charT) const { return 0; }
            virtual const charT* do_toupper(charT*, const charT*) const { return nullptr; }
            virtual charT do_tolower(charT) const { return 0; }
            virtual const charT* do_tolower(charT*, const charT*) const { return nullptr; }

            virtual charT do_widen(char) const { return 0; }
            virtual const char* do_widen(const char*, const char*, charT*) const { return nullptr; };
            virtual char do_narrow(charT, char) const { return 0; };
            virtual const charT* do_narrow(const charT*, const charT*, char, char*) const { return nullptr; }; 
        };
    }

    template<class charT> struct ctype : public locale::facet, public __internal::__ctype_impl<charT> {
    public:
        using char_type = charT;

        explicit ctype(std::size_t refs = 0) : locale::facet(refs), __internal::__ctype_impl<charT>() {}

        static locale::id id;

    protected:
        ~ctype() = default;
    };

    template<> struct ctype<char> : public locale::facet, public __internal::__ctype_impl<char> {
    public:
        using char_type = char;

        explicit ctype(const mask* tab = nullptr, bool del = false, std::size_t refs = 0);

        bool is(mask m, char c) const;
        const char* is(const char* low, const char* high, mask* vec) const;
        const char* scan_is(mask m, const char* low, const char* high) const;
        const char* scan_not(mask m, const char* low, const char* high) const;

        static locale::id id;
        static constexpr std::size_t table_size = 256;

        const mask* table() const noexcept;
        static const mask* classic_table() noexcept;

    protected:
        ~ctype();
        virtual char do_toupper(char c) const;
        virtual const char* do_toupper(char* low, const char* high) const;
        virtual char do_tolower(char c) const;
        virtual const char* do_tolower(char* low, const char* high) const;

        virtual char do_widen(char c) const;
        virtual const char* do_widen(const char* low, const char* high, char* to) const;
        virtual char do_narrow(char c, char dfault) const;
        virtual const char* do_narrow(const char* low, const char* high, char dfault, char* to) const;

    private:
        const mask* tab;
        bool del_tab;
    };

    template<> struct ctype<wchar_t> : public locale::facet, public __internal::__ctype_impl<wchar_t> {
    public:
        using char_type = wchar_t;

        explicit ctype(std::size_t refs = 0);

        static locale::id id;

    protected:
        ~ctype() = default;
        virtual bool do_is(mask m, wchar_t c) const;
        virtual const wchar_t* do_is(const wchar_t* low, const wchar_t* high, mask* vec) const;
        virtual const wchar_t* do_scan_is(mask m, const wchar_t* low, const wchar_t* high) const;
        virtual const wchar_t* do_scan_not(mask m, const wchar_t* low, const wchar_t* high) const;
        virtual wchar_t do_toupper(wchar_t c) const;
        virtual const wchar_t* do_toupper(wchar_t* low, const wchar_t* high) const;
        virtual wchar_t do_tolower(wchar_t c) const;
        virtual const wchar_t* do_tolower(wchar_t* low, const wchar_t* high) const;

        virtual wchar_t do_widen(char c) const;
        virtual const char* do_widen(const char* low, const char* high, wchar_t* to) const;
        virtual char do_narrow(wchar_t c, char dfault) const;
        virtual const wchar_t* do_narrow(const wchar_t* low, const wchar_t* high, char dfault, char* to) const;
    };

    namespace __internal {
        /* A macro-controlled container for a C-style locale. It takes in a const char* representing a locale. If the system uses POSIX,
         * it stores a locale_t type created from the newlocale method. If not, it stores const char*.
         * The class mainly exposes a get_locale() method. If the system uses POSIX, the stored locale_t object is returned. Otherwise,
         * a global_locale_switch object is returned, which temporarily sets the global locale to the stored one, and changes it back to
         * the old one whenever it goes out of scope.
         *
         * The behavior is undefined if the supplied loc name doesn't correspond to an actual locale.
         */
        template<bool HasXLocale = YILIB_HAS_XLOCALE>
        class __locale_container {};

#if YILIB_HAS_XLOCALE
        template<> class __locale_container<true> {
        protected:
            mutable locale_t loc;

        public:
            __locale_container(const char* loc, int category);
            ~__locale_container();

            [[nodiscard]] locale_t get_locale() const noexcept;
            [[nodiscard]] std::lconv* get_lconv() const noexcept;
        };

#else
        template<> class locale_container<false> {
        protected:
            const char* loc;
        private:
            int category;
        public:
            locale_container(const char* loc, int category);
            ~locale_container() = default;

            struct global_locale_switch {
            private:
                int category;
                const char* old_loc;
            public:
                global_locale_switch(const char* loc, int category);
                ~global_locale_switch();
            };

            [[nodiscard]] global_locale_switch get_locale() const;
            [[nodiscard]] std::lconv* get_lconv() const noexcept;
        };
#endif 
    }

    template<class charT>
    class ctype_byname : public ctype<charT>, public __internal::__locale_container<> {
    public:
        using mask = typename ctype<charT>::mask;
        explicit ctype_byname(const char* loc, std::size_t refs = 0)
            : ctype<charT>(refs), __internal::__locale_container<>(loc, LC_CTYPE) {}
            
        explicit ctype_byname(const string& loc, std::size_t refs = 0);
    protected:
        ~ctype_byname() = default;
    };

    template<> 
    class ctype_byname<wchar_t> : public ctype<wchar_t>, public __internal::__locale_container<> {
    public:
        using mask = typename ctype<wchar_t>::mask;
        explicit ctype_byname(const char* loc, std::size_t refs = 0);
        explicit ctype_byname(const string& loc, std::size_t refs = 0);
    protected:
        ~ctype_byname() = default;

        const wchar_t* do_is(const wchar_t* low, const wchar_t* high, mask* vec) const override;

        wchar_t do_toupper(wchar_t c) const override;
        wchar_t do_tolower(wchar_t c) const override;

        wchar_t do_widen(char c) const override;
        char do_narrow(wchar_t c, char dfault) const override;
    };

    template<> 
    class ctype_byname<char> : public ctype<char>, public __internal::__locale_container<> {
    public:
        using mask = typename ctype<char>::mask;
        explicit ctype_byname(const char* loc, std::size_t refs = 0);
        explicit ctype_byname(const string& loc, std::size_t refs = 0);
    protected:
        ~ctype_byname() = default;

        char do_toupper(char c) const override;
        char do_tolower(char c) const override;
    };

    class codecvt_base {
    public:
        enum result { ok, partial, error, noconv };
    };

    namespace __internal {
        /* Provides implementation of all public functions in codecvt, since they all just call the protected do_xxx version
         * within the same class. */
        template<class internT, class externT, class stateT>
        class __codecvt_impl : public codecvt_base {
        public:
            result out(stateT& state, const internT* from, const internT* from_end, const internT*& from_next,
                       externT* to, externT* to_end, externT*& to_next) const {
                return do_out(state, from, from_end, from_next, to, to_end, to_next);
            }

            result unshift(stateT& state, externT* to, externT* to_end, externT*& to_next) const {
                return do_unshift(state, to, to_end, to_next);
            }

            result in(stateT& state, const externT* from, const externT* from_end, const externT*& from_next,
                      internT* to, internT* to_end, internT*& to_next) const {
                return in(state, from, from_end, from_next, to, to_end, to_next);
            }

            int encoding() const noexcept { return do_encoding(); }
            bool always_noconv() const noexcept { return do_always_noconv(); }
            int length(stateT& state, const externT* from, const externT* end, std::size_t max) const {
                return do_length(state, from, end, max);
            }
            int max_length() const noexcept { return do_max_length(); }

        protected:
            virtual result do_out(stateT& state, const internT* from, const internT* from_end, const internT*& from_next,
                   externT* to, externT* to_end, externT*& to_next) const = 0;

            virtual result do_unshift(stateT& state, externT* to, externT* to_end, externT*& to_next) const = 0;

            virtual result do_in(stateT& state, const externT* from, const externT* from_end, const externT*& from_next,
                    internT* to, internT* to_end, internT*& to_next) const = 0;

            virtual int do_encoding() const noexcept = 0;
            virtual bool do_always_noconv() const noexcept = 0;
            virtual int do_length(stateT&, const externT* from, const externT* end, std::size_t max) const = 0;
            virtual int do_max_length() const noexcept = 0;
        };
    }

    template<class internT, class externT, class stateT>
    class codecvt : public locale::facet, public __internal::__codecvt_impl<internT, externT, stateT> {
    public:
        using intern_type = internT;
        using extern_type = externT;
        using state_type = stateT;

        explicit codecvt(std::size_t refs = 0) : locale::facet(refs) {}

        static locale::id id;

    protected:
        ~codecvt() = default;
    };

    template<>
    class codecvt<char, char, std::mbstate_t> : public locale::facet, public __internal::__codecvt_impl<char, char, std::mbstate_t> {
    public:
        using intern_type = char;
        using extern_type = char;
        using state_type = std::mbstate_t;

        explicit codecvt(std::size_t refs = 0);

        static locale::id id;

    protected:
        ~codecvt() = default;
        result do_out(std::mbstate_t& state, const char* from, const char* from_end, const char*& from_next,
                   char* to, char* to_end, char*& to_next) const override;

        result do_unshift(std::mbstate_t& state, char* to, char* to_end, char*& to_next) const override;

        result do_in(std::mbstate_t& state, const char* from, const char* from_end, const char*& from_next,
                   char* to, char* to_end, char*& to_next) const override;

        int do_encoding() const noexcept override;
        bool do_always_noconv() const noexcept override;
        int do_length(std::mbstate_t&, const char* from, const char* end, std::size_t max) const override;
        int do_max_length() const noexcept override;
    };

    template<>
    class codecvt<char16_t, char8_t, std::mbstate_t> : public locale::facet, public __internal::__codecvt_impl<char16_t, char8_t, std::mbstate_t> {
    public:
        using intern_type = char16_t;
        using extern_type = char8_t;
        using state_type = std::mbstate_t;

        explicit codecvt(std::size_t refs = 0);

        static locale::id id;

        /* Converts the first valid UTF-16 character in the array [buf, buf + max) into its UTF-32 representation. Returns a tuple containing the conversion
         * result, the converted character if successful, and the number of UTF-16 entries consumed if successful. */
        static constexpr tuple<result, char32_t, std::size_t> utf16_to_utf32(const char16_t* buf, std::size_t max) noexcept;
        /* Converts the given UTF-32 character into its UTF-16 representation, writing into buf. buf must have at least 2 entries. Returns a tuple containing
         * the result of the conversion and how many entries in buf was used. */
        static constexpr tuple<result, std::size_t> utf32_to_utf16(char32_t c, char16_t* buf) noexcept;
        /* Converts the first valid UTF-8 character in the array [from_buf, from_buf + from_max) into its UTF-16 representation, writing into buf. buf must
         * have at least 2 entries. Returns a tuple containing the result of the conversion, the number of entries consumed from from_buf, and the number of 
         * entries used in buf. */
        static constexpr tuple<result, std::size_t, std::size_t> utf8_to_utf16(const char8_t* from_buf, std::size_t from_max, char16_t* buf) noexcept;
        /* Converts the first valid UTF-16 character in the array [from_buf, from_buf + from_max) into its UTF-8 representation, writing into buf. buf must
         * have at least 4 entries. Returns a tuple containing the result of the conversion, the number of entries consumed from from_buf, and the number of 
         * entries used in buf. */
        static constexpr tuple<result, std::size_t, std::size_t> utf16_to_utf8(const char16_t* from_buf, std::size_t from_max, char8_t* buf) noexcept;

    protected:
        ~codecvt() = default;
        result do_out(std::mbstate_t&, const char16_t* from, const char16_t* from_end, const char16_t*& from_next,
                   char8_t* to, char8_t* to_end, char8_t*& to_next) const override;

        result do_unshift(std::mbstate_t&, char8_t* to, char8_t*, char8_t*& to_next) const override;

        result do_in(std::mbstate_t&, const char8_t* from, const char8_t* from_end, const char8_t*& from_next,
                   char16_t* to, char16_t* to_end, char16_t*& to_next) const override;

        int do_encoding() const noexcept override;
        bool do_always_noconv() const noexcept override;
        int do_length(std::mbstate_t&, const char8_t* from, const char8_t* end, std::size_t max) const override;
        int do_max_length() const noexcept override;
    };

    template<>
    class codecvt<char32_t, char8_t, std::mbstate_t> : public locale::facet, public __internal::__codecvt_impl<char32_t, char8_t, std::mbstate_t> {
    public:
        using intern_type = char32_t;
        using extern_type = char8_t;
        using state_type = std::mbstate_t;

        explicit codecvt(std::size_t refs = 0);

        static locale::id id;

        /* Converts the given UTF-32 character to an array of UTF-8 characters. The array must be at least 4 entries long. Returns a tuple containing the conversion
         * result and the number of UTF-8 entries actually taken. */
        static constexpr tuple<result, std::size_t> utf32_to_utf8(char32_t c, char8_t* buf) noexcept;

        /* Converts the first valid UTF-8 character in array [buf, buf + max) to the UTF-32 character it represents. Returns a tuple containing the conversion status,
         * the converted character if successful, and the number of UTF-8 bytes that are consumed if successful. */
        static constexpr tuple<result, char32_t, std::size_t> utf8_to_utf32(const char8_t* buf, std::size_t max) noexcept;

    protected:
        ~codecvt() = default;
        result do_out(std::mbstate_t&, const char32_t* from, const char32_t* from_end, const char32_t*& from_next,
                   char8_t* to, char8_t* to_end, char8_t*& to_next) const override;

        result do_unshift(std::mbstate_t&, char8_t* to, char8_t*, char8_t*& to_next) const override;

        result do_in(std::mbstate_t&, const char8_t* from, const char8_t* from_end, const char8_t*& from_next,
                   char32_t* to, char32_t* to_end, char32_t*& to_next) const override;

        int do_encoding() const noexcept override;
        bool do_always_noconv() const noexcept override;
        int do_length(std::mbstate_t&, const char8_t* from, const char8_t* end, std::size_t max) const override;
        int do_max_length() const noexcept override;
    };

    template<>
    class codecvt<wchar_t, char, std::mbstate_t> : public locale::facet, public __internal::__codecvt_impl<wchar_t, char, std::mbstate_t> {
    public:
        using intern_type = wchar_t;
        using extern_type = char;
        using state_type = std::mbstate_t;

        explicit codecvt(std::size_t refs = 0);

        static locale::id id;

    protected:
        ~codecvt() = default;
        result do_out(std::mbstate_t& state, const wchar_t* from, const wchar_t* from_end, const wchar_t*& from_next,
                   char* to, char* to_end, char*& to_next) const override;

        result do_unshift(std::mbstate_t& state, char* to, char* to_end, char*& to_next) const override;

        result do_in(std::mbstate_t& state, const char* from, const char* from_end, const char*& from_next,
                   wchar_t* to, wchar_t* to_end, wchar_t*& to_next) const override;

        int do_encoding() const noexcept override;
        bool do_always_noconv() const noexcept override;
        int do_length(std::mbstate_t& state, const char* from, const char* end, std::size_t max) const override;
        int do_max_length() const noexcept override;
    };

    template<class internT, class externT, class stateT>
    class codecvt_byname : public codecvt<internT, externT, stateT>, public __internal::__locale_container<> {
    public:
        explicit codecvt_byname(const char* loc, std::size_t refs = 0)
            : codecvt<internT, externT, stateT>(refs), __internal::__locale_container<>(loc, LC_CTYPE) {}
        explicit codecvt_byname(const string& loc, std::size_t refs = 0);

    protected:
        ~codecvt_byname() = default;
    };

    template<> 
    class codecvt_byname<char, char, std::mbstate_t> : public codecvt<char, char, std::mbstate_t>, public __internal::__locale_container<> {
    public:
        explicit codecvt_byname(const char* loc, std::size_t refs = 0);
        explicit codecvt_byname(const string& loc, std::size_t refs = 0);

    protected:
        ~codecvt_byname() = default;
    };

    template<>
    class codecvt_byname<char16_t, char8_t, std::mbstate_t> : public codecvt<char16_t, char8_t, std::mbstate_t> {
    public:
        explicit codecvt_byname(const char*, std::size_t refs = 0);
        explicit codecvt_byname(const string& loc, std::size_t refs = 0);

    protected:
        ~codecvt_byname() = default;
    };

    template<>
    class codecvt_byname<char32_t, char8_t, std::mbstate_t> : public codecvt<char32_t, char8_t, std::mbstate_t> {
    public:
        explicit codecvt_byname(const char*, std::size_t refs = 0);
        explicit codecvt_byname(const string& loc, std::size_t refs = 0);

    protected:
        ~codecvt_byname() = default;
    };

    template<> 
    class codecvt_byname<wchar_t, char, std::mbstate_t> : public codecvt<wchar_t, char, std::mbstate_t>, public __internal::__locale_container<> {
    public:
        explicit codecvt_byname(const char* loc, std::size_t refs = 0);
        explicit codecvt_byname(const string& loc, std::size_t refs = 0);

    protected:
        ~codecvt_byname() = default;
        result do_out(std::mbstate_t& state, const wchar_t* from, const wchar_t* from_end, const wchar_t*& from_next,
                   char* to, char* to_end, char*& to_next) const override;

        result do_in(std::mbstate_t& state, const char* from, const char* from_end, const char*& from_next,
                   wchar_t* to, wchar_t* to_end, wchar_t*& to_next) const override;

        int do_encoding() const noexcept override;
        int do_length(std::mbstate_t& state, const char* from, const char* end, std::size_t max) const override;
        int do_max_length() const noexcept override;
    };
}