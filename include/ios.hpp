#pragma once

#include "iosfwd.hpp"
#include "type_traits.hpp"
#include "system_error.hpp"
#include "memory.hpp"
#include "utility.hpp"
#include "bit.hpp"
#include "cstring.hpp"

#include "locale/locale.hpp"
#include "locale/ctype.hpp"

namespace std {
    /* 29.5.4 Class template fpos */
    template<class stateT> requires is_default_constructible_v<stateT>
        && is_copy_constructible_v<stateT> && is_copy_assignable_v<stateT>
        && is_nothrow_destructible_v<stateT>
    class fpos {
    public:
        stateT state() const { return st; }
        void state(stateT s) { st = s; }
    private:
        stateT st;
    };

    /* 29.5.7 Error reporting */
    enum class io_errc {
        stream = 1
    };

    template<> struct is_error_code_enum<io_errc> : public true_type {};
    error_code make_error_code(io_errc e) noexcept;
    error_condition make_error_condition(io_errc e) noexcept;
    const error_category& iostream_category() noexcept;

    /* 29.5.3 Class ios_base */
    class ios_base {
    public:
        class failure : public system_error {
        public:
            explicit failure(const string& msg, const error_code& ec = io_errc::stream);
            explicit failure(const char* msg, const error_code& ec = io_errc::stream);
        };

        /* 29.5.3.2.2 fmtflags */
        using fmtflags = unsigned int;
        static constexpr fmtflags boolalpha = 1 << 0;
        static constexpr fmtflags dec = 1 << 1;
        static constexpr fmtflags fixed = 1 << 2;
        static constexpr fmtflags hex = 1 << 3;
        static constexpr fmtflags internal = 1 << 4;
        static constexpr fmtflags left = 1 << 5;
        static constexpr fmtflags oct = 1 << 6;
        static constexpr fmtflags right = 1 << 7;
        static constexpr fmtflags scientific = 1 << 8;
        static constexpr fmtflags showbase = 1 << 9;
        static constexpr fmtflags showpoint = 1 << 10;
        static constexpr fmtflags showpos = 1 << 11;
        static constexpr fmtflags skipws = 1 << 12;
        static constexpr fmtflags unitbuf = 1 << 13;
        static constexpr fmtflags uppercase = 1 << 14;

        static constexpr fmtflags adjustfield = left | right | internal;
        static constexpr fmtflags basefield = dec | oct | hex;
        static constexpr fmtflags floatfield = scientific | fixed;

        /* 29.5.3.2.3 iostate */
        using iostate = unsigned int;
        static constexpr iostate badbit = 1 << 0;
        static constexpr iostate eofbit = 1 << 1;
        static constexpr iostate failbit = 1 << 2;
        static constexpr iostate goodbit = 0;

        /* 29.5.3.2.4 openmode */
        using openmode = unsigned int;
        static constexpr openmode app = 1 << 0;
        static constexpr openmode ate = 1 << 1;
        static constexpr openmode binary = 1 << 2;
        static constexpr openmode in = 1 << 3;
        static constexpr openmode out = 1 << 4;
        static constexpr openmode trunc = 1 << 5;

        /* 29.5.3.2.5 seekdir */
        using seekdir = unsigned int;
        static constexpr seekdir beg = 1 << 0;
        static constexpr seekdir cur = 1 << 1;
        static constexpr seekdir end = 1 << 2;

        /* 29.5.3.2.6 Class ios_base::Init */
        // TODO: Ensure that this doesn't need to be implemented.
        class Init {
        public:
            Init() = default;
            Init(const Init&) = default;
            ~Init() = default;
            Init& operator=(const Init&) = default;
        };

        /* 29.5.3.3 fmtflags state */
        fmtflags flags() const;
        fmtflags flags(fmtflags fmtfl);
        fmtflags setf(fmtflags fmtfl);
        fmtflags setf(fmtflags fmtfl, fmtflags mask);
        void unsetf(fmtflags mask);

        streamsize precision() const;
        streamsize precision(streamsize prec);
        streamsize width() const;
        streamsize width(streamsize wide);

        /* 29.5.3.4 Locales */
        locale imbue(const locale& loc);
        locale getloc() const;

        /* 29.5.3.6 Storage */
        static int xalloc();
        long& iword(int idx);
        void*& pword(int idx);

        // This is not in the standard. Used to satisfy requirement for "iword" and "pword".
        virtual void setstate(iostate state);

        virtual ~ios_base();

        /* 29.5.3.7 Callbacks */
        enum event { erase_event, imbue_event, copyfmt_event };
        using event_callback = void (*)(event, ios_base&, int idx);
        void register_callback(event_callback fn, int idx);

        ios_base(const ios_base&) = delete;
        ios_base& operator=(const ios_base&) = delete;
        
        /* 29.5.3.5 Static members */
        static bool sync_with_stdio(bool sync = true);

    protected:
        ios_base() = default;

        fmtflags fmtfl;
        streamsize prec;
        streamsize wide;

        event_callback* callbacks;
        int* callback_indices;
        std::size_t callback_count;

        locale* loc;

        static constinit std::size_t index;
        long* iarray;
        std::size_t iarray_size;
        void** parray;
        std::size_t parray_size;

        iostate rdstate;
        iostate exception;
    };

    /* 29.5.5 Class template basic_ios */
    template<class charT, class traits>
    class basic_ios : public ios_base {
    public:
        using char_type = charT;
        using int_type = typename traits::int_type;
        using pos_type = typename traits::pos_type;
        using off_type = typename traits::off_type;
        using traits_type = traits;

        /* 29.5.5.4 Flags functions */
        explicit operator bool() const { return (rdstate() & (badbit | failbit)) == 0; }
        bool operator!() const { return !operator bool(); }
        iostate rdstate() const { return ios_base::rdstate; }
        void clear(iostate state = goodbit) {
            ios_base::rdstate = state | (rdbuf() ? goodbit : badbit);
            if ((ios_base::rdstate & exceptions()) != 0)
                throw failure("Error when calling clear() in basic_ios.");
        }

        void setstate(iostate state) override { clear(rdstate() | state); }
        bool good() const { return rdstate() == goodbit; }
        bool eof() const { return (rdstate() & eofbit) != 0; }
        bool fail() const { return (rdstate() & (badbit | failbit)) != 0; };
        bool bad() const { return (rdstate() & badbit) != 0; }

        iostate exceptions() const { return ios_base::exception; }
        void exceptions(iostate except) {
            clear(rdstate());
            ios_base::exception = except;
        }

        /* 29.5.5.2 Constructor/destructor */
        explicit basic_ios(basic_streambuf<charT, traits>* sb) : buf(sb) {}
        virtual ~basic_ios() = default;

        /* 29.5.5.3 Members */
        basic_ostream<charT, traits>* tie() const { return tiestr; }
        basic_ostream<charT, traits>* tie(basic_ostream<charT, traits>* tiestr) {
            basic_ostream<charT, traits>* old = this->tiestr;
            this->tiestr = tiestr;
            return old;
        }

        basic_streambuf<charT, traits>* rdbuf() const { return buf; }
        basic_streambuf<charT, traits>* rdbuf(basic_streambuf<charT, traits>* sb) {
            basic_streambuf<charT, traits> old = this->buf;
            this->buf = sb;
            return old;
        }

        basic_ios& copyfmt(const basic_ios& rhs) {
            if (this == addressof(rhs)) return *this;

            for (int i = rhs.callback_count - 1; i >= 0; i--)
                (*rhs.callbacks[i])(erase_event, *this, rhs.callback_indices[i]);

            fmtfl = rhs.flags();
            prec = rhs.precision();
            wide = rhs.width();

            delete[] callbacks;
            delete[] callback_indices;
            callback_count = rhs.callback_count;
            callbacks = new event_callback[callback_count];
            callback_indices = new int[callback_count];
            std::memcpy(callbacks, rhs.callbacks, sizeof(event_callback) * callback_count);
            std::memcpy(callback_indices, rhs.callback_indices, sizeof(int) * callback_count);

            loc = rhs.getloc();

            delete[] iarray;
            delete[] parray;
            iarray_size = rhs.iarray_size;
            parray_size = rhs.parray_size;
            if (rhs.iarray) {
                iarray = new long[bit_ceil(iarray_size)];
                std::memcpy(iarray, rhs.iarray, sizeof(long) * bit_ceil(iarray_size));
            } else {
                iarray = nullptr;
            }

            if (rhs.parray) {
                parray = new void*[bit_ceil(parray_size)];
                std::memcpy(parray, rhs.parray, sizeof(void*) * bit_ceil(parray_size));
            } else {
                parray = nullptr;
            }

            tiestr = rhs.tie();
            fillch = rhs.fill();

            for (int i = callback_count - 1; i >= 0; i--)
                (*callbacks[i])(copyfmt_event, *this, callback_indices[i]);

            exceptions(rhs.exceptions());
        }

        char_type fill() const { return fillch; }
        char_type fill(char_type ch) { 
            char_type old = fillch;
            fillch = ch;
            return old;
        }

        locale imbue(const locale& loc) {
            locale old = ios_base::imbue(loc);
            if (rdbuf()) rdbuf()->pubimbue(loc);
            return old;
        }

        char narrow(char_type c, char dfault) const { return use_facet<ctype<char_type>>(getloc()).narrow(c, dfault); }
        char_type widen(char c) const { return use_facet<ctype<char_type>>(getloc()).widen(c); }

        basic_ios(const basic_ios&) = delete;
        basic_ios& operator=(const basic_ios&) = delete;

    protected:
        basic_ios() = default;
        void init(basic_streambuf<charT, traits>* sb) {
            fmtfl = skipws | dec;
            prec = 6;
            wide = 0;

            callbacks = new event_callback[0];
            callback_indices = new int[0];
            callback_count = 0;

            loc = nullptr;

            iarray = nullptr;
            iarray_size = 0;
            parray = nullptr;
            parray_size = 0;

            ios_base::rdstate = sb ? badbit : goodbit;
            exception = goodbit;

            buf = sb;
            tiestr = nullptr;
            fillch = widen(' ');
        }

        void move(basic_ios& rhs) { return move(std::move(rhs)); }
        void move(basic_ios&& rhs) {
            std::swap(rhs);
            buf = nullptr;
            rhs.tiestr = nullptr;
        }
        void swap(basic_ios& rhs) noexcept {
            std::swap(fmtfl, rhs.fmtfl);
            std::swap(prec, rhs.prec);
            std::swap(wide, rhs.wide);

            std::swap(callbacks, rhs.callbacks);
            std::swap(callback_indices, rhs.callback_indices);
            std::swap(callback_count, rhs.callback_count);

            std::swap(loc, rhs.loc);

            std::swap(callbacks, rhs.callbacks);

            std::swap(iarray, rhs.iarray);
            std::swap(iarray_size, rhs.iarray_size);
            std::swap(parray, rhs.parray);
            std::swap(parray_size, rhs.parray_size);

            std::swap(ios_base::rdstate, rhs.ios_base::rdstate);
            std::swap(exception, rhs.exception);

            std::swap(tiestr, rhs.tiestr);
            std::swap(fillch, rhs.fillch);
        }
        void set_rdbuf(basic_streambuf<charT, traits>* sb) { buf = sb; }

    private:
        basic_streambuf<charT, traits>* buf;
        basic_ostream<charT, traits>* tiestr;
        char_type fillch;
    };

    /* 29.5.6 Manipulators */
    ios_base& boolalpha(ios_base& str);
    ios_base& noboolalpha(ios_base& str);
    ios_base& showbase(ios_base& str);
    ios_base& noshowbase(ios_base& str);
    ios_base& showpoint(ios_base& str);
    ios_base& noshowpoint(ios_base& str);
    ios_base& showpos(ios_base& str);
    ios_base& noshowpos(ios_base& str);
    ios_base& skipws(ios_base& str);
    ios_base& noskipws(ios_base& str);
    ios_base& uppercase(ios_base& str);
    ios_base& nouppercase(ios_base& str);
    ios_base& unitbuf(ios_base& str);
    ios_base& nounitbuf(ios_base& str);
    ios_base& internal(ios_base& str);
    ios_base& left(ios_base& str);
    ios_base& right(ios_base& str);
    ios_base& dec(ios_base& str);
    ios_base& hex(ios_base& str);
    ios_base& oct(ios_base& str);
    ios_base& fixed(ios_base& str);
    ios_base& scientific(ios_base& str);
    ios_base& hexfloat(ios_base& str);
    ios_base& defaultfloat(ios_base& str);
}