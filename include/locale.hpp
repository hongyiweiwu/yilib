#pragma once

#include "string.hpp"
#include "cstddef.hpp"
#include "clocale.hpp"
#include "cwchar.hpp"
#include "cstring.hpp"
#include "typeinfo.hpp"
#include "stdexcept.hpp"

// For posix-specific extended locale APIs.
#if defined(__APPLE__) || defined(_GNU_SOURCE)
#define YILIB_HAS_XLOCALE 1
#else
#define YILIB_HAS_XLOCALE 0
#endif

#if YILIB_HAS_XLOCALE && defined(__APPLE__)
#include "xlocale.h"
#endif

namespace std {
    /* 28.3.1 locale */
    struct locale {
    public:
        class facet {
        private:
            unsigned long refs;

        protected:
            explicit facet(size_t refs = 0);
            virtual ~facet() = default;
            facet(const facet&) = delete;
            void operator=(const facet&) = delete;

        public:
            unsigned long increment_ref() noexcept;
            unsigned long decrement_ref() noexcept;
        };

        class id {
        public:
            size_t n;
            static size_t last_assigned_n;

            id() : n(__atomic_fetch_add(&last_assigned_n, 1, __ATOMIC_SEQ_CST)) {}
            void operator=(const id&) = delete;
            id(const id&) = delete;
        };

        using category = int;

        static constexpr category none = 0;
        static constexpr category collate = 1;
        static constexpr category ctype = 1 << 1;
        static constexpr category monetary = 1 << 2;
        static constexpr category numeric = 1 << 3;
        static constexpr category time = 1 << 4;
        static constexpr category messages = 1 << 5;
        static constexpr category all = collate | ctype | monetary | numeric | time | messages;
   
        locale() noexcept;
        locale(const locale& other) noexcept;
        explicit locale(const char* std_name);
        explicit locale(const string& std_name);
        locale(const locale& other, const char* std_name, category);
        locale(const locale& other, const string& std_name, category);
        template<class Facet> locale(const locale& other, Facet* f) : locale(other) {
            n = "*";
            if (!f) return;
            else if (Facet::id.n >= facets_arr_size) {
                locale::facet** new_facets_arr = new locale::facet*[Facet::id.n + 1]();
                std::memcpy(new_facets_arr, facets, sizeof(locale::facet*) * facets_arr_size);
                delete[] facets;
                facets = new_facets_arr;
                facets_arr_size = Facet::id.n + 1;
            }

            if (facets[Facet::id.n]) facets[Facet::id.n]->decrement_ref();
            f->increment_ref();
            facets[Facet::id.n] = f;
        }

        locale(const locale& other, const locale& one, category);
        ~locale();
        const locale& operator=(const locale& other) noexcept;
        template<class Facet> locale combine(const locale& other) const {
            locale::facet* new_facet = other.facets[Facet::id.n];
            if (!new_facet) throw runtime_error("Invalid argument for combine: other doesn't implement the given facet.");
            
            return locale(*this, new_facet);
        }

        string name() const;

        bool operator==(const locale& other) const;

        template<class charT, class traits, class Allocator>
        bool operator()(const basic_string<charT, traits, Allocator>& s1,
                        const basic_string<charT, traits, Allocator>& s2) const;

        static locale global(const locale&);
        static const locale& classic();

    private:
        /* Backdoor constructor for the classic locale. */
        locale(locale::facet** facets, size_t facets_arr_size, const char* name = "C");

        string n;
        locale::facet** facets;
        size_t facets_arr_size;

        template<class Facet> friend const Facet& use_facet(const locale&);
        template<class Facet> friend bool has_facet(const locale& loc) noexcept;
    };

    template<class Facet>
    const Facet& use_facet(const locale& loc) {
        const locale::facet* facet = loc.facets[Facet::id.n];
        return facet ? *dynamic_cast<const Facet*>(facet) : throw bad_cast();
    }

    template<class Facet>
    bool has_facet(const locale& loc) noexcept { return loc.facets[Facet::id.n]; }

    /* 28.3.3 Convenience interfaces */
    template<class charT> bool isspace(charT c, const locale& loc);
    template<class charT> bool isprint(charT c, const locale& loc);
    template<class charT> bool iscntrl(charT c, const locale& loc);
    template<class charT> bool isupper(charT c, const locale& loc);
    template<class charT> bool islower(charT c, const locale& loc);
    template<class charT> bool isalpha(charT c, const locale& loc);
    template<class charT> bool isdigit(charT c, const locale& loc);
    template<class charT> bool ispunct(charT c, const locale& loc);
    template<class charT> bool isxdigit(charT c, const locale& loc);
    template<class charT> bool isalnum(charT c, const locale& loc);
    template<class charT> bool isgraph(charT c, const locale& loc);
    template<class charT> bool isblank(charT c, const locale& loc);
    template<class charT> charT toupper(charT c, const locale& loc);
    template<class charT> charT tolower(charT c, const locale& loc);


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

        explicit ctype(size_t refs = 0) : locale::facet(refs), __internal::__ctype_impl<charT>() {}

        static locale::id id;

    protected:
        ~ctype() = default;
    };

    template<> struct ctype<char> : public locale::facet, public __internal::__ctype_impl<char> {
    public:
        using char_type = char;

        explicit ctype(const mask* tab = nullptr, bool del = false, size_t refs = 0);

        bool is(mask m, char c) const;
        const char* is(const char* low, const char* high, mask* vec) const;
        const char* scan_is(mask m, const char* low, const char* high) const;
        const char* scan_not(mask m, const char* low, const char* high) const;

        static locale::id id;
        static constexpr size_t table_size = 256;

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

        explicit ctype(size_t refs = 0);

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
        class locale_container {};

#if YILIB_HAS_XLOCALE
        template<> class locale_container<true> {
        protected:
            mutable locale_t loc;

        public:
            locale_container(const char* loc, int category);
            virtual ~locale_container();

            [[nodiscard]] locale_t get_locale() const noexcept;
        };

#else
        template<> class locale_container<false> {
        protected:
            const char* loc;
        private:
            int category;
        public:
            locale_container(const char* loc, int category);
            virtual ~locale_container() = default;

            struct global_locale_switch {
            private:
                int category;
                const char* old_loc;
            public:
                global_locale_switch(const char* loc, int category);
                ~global_locale_switch();
            };

            [[nodiscard]] global_locale_switch get_locale() const;
        };
#endif 
    }

    template<class charT>
    class ctype_byname : public ctype<charT>, public __internal::locale_container<> {
    public:
        using mask = typename ctype<charT>::mask;
        explicit ctype_byname(const char* loc, size_t refs = 0)
            : ctype<charT>(refs), __internal::locale_container<>(loc, LC_CTYPE) {}
            
        explicit ctype_byname(const string& loc, size_t refs = 0) : ctype_byname(loc.c_str(), refs) {}
    protected:
        ~ctype_byname() = default;
    };

    template<> class ctype_byname<wchar_t> : public ctype<wchar_t>, public __internal::locale_container<> {
    public:
        using mask = typename ctype<wchar_t>::mask;
        explicit ctype_byname(const char* loc, size_t refs = 0)
            : ctype<wchar_t>(refs), __internal::locale_container<>(loc, LC_CTYPE) {}
            
        explicit ctype_byname(const string& loc, size_t refs = 0) : ctype_byname(loc.c_str(), refs) {}
    protected:
        ~ctype_byname() = default;

        const wchar_t* do_is(const wchar_t* low, const wchar_t* high, mask* vec) const override;

        wchar_t do_toupper(wchar_t c) const override;
        wchar_t do_tolower(wchar_t c) const override;

        wchar_t do_widen(char c) const override;
        char do_narrow(wchar_t c, char dfault) const override;
    };

    template<> class ctype_byname<char> : public ctype<char>, public __internal::locale_container<> {
    public:
        using mask = typename ctype<char>::mask;
        explicit ctype_byname(const char* loc, size_t refs = 0)
            : ctype<char>(nullptr, false, refs), __internal::locale_container<>(loc, LC_CTYPE) {}
            
        explicit ctype_byname(const string& loc, size_t refs = 0) : ctype_byname(loc.c_str(), refs) {}
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
            int length(stateT& state, const externT* from, const externT* end, size_t max) const {
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
            virtual int do_length(stateT&, const externT* from, const externT* end, size_t max) const = 0;
            virtual int do_max_length() const noexcept = 0;
        };
    }

    template<class internT, class externT, class stateT>
    class codecvt : public locale::facet, public __internal::__codecvt_impl<internT, externT, stateT> {
    public:
        using intern_type = internT;
        using extern_type = externT;
        using state_type = stateT;

        explicit codecvt(size_t refs = 0) : locale::facet(refs) {}

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

        explicit codecvt(size_t refs = 0);

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
        int do_length(std::mbstate_t&, const char* from, const char* end, size_t max) const override;
        int do_max_length() const noexcept override;
    };

    template<>
    class codecvt<wchar_t, char, std::mbstate_t> : public locale::facet, public __internal::__codecvt_impl<wchar_t, char, std::mbstate_t> {
    public:
        using intern_type = wchar_t;
        using extern_type = char;
        using state_type = std::mbstate_t;

        explicit codecvt(size_t refs = 0);

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
        int do_length(std::mbstate_t& state, const char* from, const char* end, size_t max) const override;
        int do_max_length() const noexcept override;
    };

    template<class internT, class externT, class stateT>
    class codecvt_byname : public codecvt<internT, externT, stateT>, public __internal::locale_container<> {
    public:
        explicit codecvt_byname(const char* loc, size_t refs = 0)
            : codecvt<internT, externT, stateT>(refs), __internal::locale_container<>(loc, LC_CTYPE) {}
        explicit codecvt_byname(const string& loc, size_t refs = 0) : codecvt_byname(loc.c_str(), refs) {}

    protected:
        ~codecvt_byname() = default;
    };

    template<> 
    class codecvt_byname<char, char, std::mbstate_t> : public codecvt<char, char, std::mbstate_t>, public __internal::locale_container<> {
    public:
        explicit codecvt_byname(const char* loc, size_t refs = 0);
        explicit codecvt_byname(const string& loc, size_t refs = 0);

    protected:
        ~codecvt_byname() = default;
    };

    template<> 
    class codecvt_byname<wchar_t, char, std::mbstate_t> : public codecvt<wchar_t, char, std::mbstate_t>, public __internal::locale_container<> {
    public:
        explicit codecvt_byname(const char* loc, size_t refs = 0);
        explicit codecvt_byname(const string& loc, size_t refs = 0);

    protected:
        ~codecvt_byname() = default;
        result do_out(std::mbstate_t& state, const wchar_t* from, const wchar_t* from_end, const wchar_t*& from_next,
                   char* to, char* to_end, char*& to_next) const override;

        result do_in(std::mbstate_t& state, const char* from, const char* from_end, const char*& from_next,
                   wchar_t* to, wchar_t* to_end, wchar_t*& to_next) const override;

        int do_encoding() const noexcept override;
        int do_length(std::mbstate_t& state, const char* from, const char* end, size_t max) const override;
        int do_max_length() const noexcept override;
    };

    template<class charT>
    struct collate {};
}

#include "locale.tpp"
