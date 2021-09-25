#pragma once

#include "iosfwd.hpp"
#include "ios.hpp"
#include "utility.hpp"

namespace std {
    template<class charT, class traits>
    class basic_streambuf {
    private:
        charT* ixbeg;
        charT* ixnext;
        charT* ixend;
        charT* oxbeg;
        charT* oxnext;
        charT* oxend;
        locale loc;
    public:
        using char_type = charT;
        using int_type = typename traits::int_type;
        using pos_type = typename traits::pos_type;
        using off_type = typename traits::off_type;
        using traits_type = traits;

        virtual ~basic_streambuf() = default;

        /* 29.6.3.3.1 Locales */
        locale pubimbue(const locale& loc) {
            imbue(loc);
            locale old = move(this->loc);
            this->loc = loc;
            return old;
        }

        locale getloc() const { return loc; }

        /* 29.6.3.3.2 Buffer and positioning */
        basic_streambuf* pubsetbuf(char_type* s, streamsize n) { return setbuf(s, n); }
        pos_type pubseekoff(off_type off, ios_base::seekdir way, 
                            ios_base::openmode which = ios_base::in | ios_base::out) { return seekoff(off, way, which); }

        pos_type pubseekpos(pos_type sp, ios_base::openmode which = ios_base::in | ios_base::out) { return seekpos(sp, which); }
        int pubsync() { return sync(); }

        /* 29.6.3.3.3 Get area */
        streamsize in_avail() { return ixnext < ixend ? ixend - ixnext : showmanyc(); }
        int_type snextc() { return sbumpc() == traits_type::eof() ? traits_type::eof() : sgetc(); }
        int_type sbumpc() { return ixnext < ixend ? traits_type::to_int_type(*ixnext++) : uflow(); }
        int_type sgetc() { return ixnext < ixend ? traits_type::to_int_type(*ixnext) : underflow(); }
        streamsize sgetn(char_type* s, streamsize n) { return xsgetn(s, n); }

        /* 29.6.3.3.4 Putback */
        int_type sputbackc(char_type c) {
            if (ixbeg < ixnext && traits_type::eq(c, *(ixnext - 1))) {
                return traits_type::to_int_type(*--ixnext);
            } else {
                const int_type result = pbackfail(traits_type::to_int_type(c));
                return result == traits_type::eof() ? traits_type::eof() : traits::to_int_type(*ixnext);
            }
        }
        int_type sungetc() { return ixbeg < ixnext ? traits_type::to_int_type(*ixnext--) : pbackfail(); }

        /* 29.6.3.3.5 Put area */
        int_type sputc(char_type c) {
            if (oxnext < oxend) {
                *oxnext++ = c;
                return traits_type::to_int_type(c);
            } else {
                return overflow(traits_type::to_int_type(c));
            }
        }
        streamsize sputn(const char_type* s, streamsize n) { return xsputn(s, n); }

    protected:
        basic_streambuf() : ixbeg(nullptr), ixnext(nullptr), ixend(nullptr), oxbeg(nullptr), oxnext(nullptr), oxend(nullptr), loc() {}
        basic_streambuf(const basic_streambuf& rhs) = default;
        basic_streambuf& operator=(const basic_streambuf& rhs) {
            ixbeg = rhs.ixbeg;
            ixnext = rhs.ixnext;
            ixend = rhs.ixend;
            oxbeg = rhs.oxbeg;
            oxnext = rhs.oxnext;
            oxend = rhs.oxend;
            loc = rhs.loc;
            return *this;
        }
    
        void swap(basic_streambuf& rhs) {
            std::swap(ixbeg, rhs.ixbeg);
            std::swap(ixnext, rhs.ixnext);
            std::swap(ixend, rhs.ixend);
            std::swap(oxbeg, rhs.oxbeg);
            std::swap(oxnext, rhs.oxnext);
            std::swap(oxend, rhs.oxend);
            std::swap(loc, rhs.loc);
        }

        /* 29.6.3.4.2 Get area access */
        char_type* eback() const { return ixbeg; }
        char_type* gptr() const { return ixnext; }
        char_type* egptr() const { return ixend; }
        void gbump(int n) { ixnext += n; }
        void setg(char_type* gbeg, char_type* gnext, char_type* gend) {
            ixbeg = gbeg;
            ixnext = gnext;
            ixend = gend;
        }

        /* 29.6.3.4.3 Put area access */
        char_type* pbase() const { return oxbeg; }
        char_type* pptr() const { return oxnext; }
        char_type* epptr() const { return oxend; }
        void pbump(int n) { oxnext += n; }
        void setp(char_type* pbeg, char_type* pend) {
            oxbeg = pbeg;
            oxnext = pbeg;
            oxend = pend;
        }

        /* 29.6.3.5.1 Locales */
        virtual void imbue(const locale&) {}

        /* 29.6.3.5.2 Buffer management and positioning */
        virtual basic_streambuf* setbuf(char_type*, streamsize) { return this; }
        virtual pos_type seekoff(off_type, ios_base::seekdir, 
                                 ios_base::openmode = ios_base::in | ios_base::out) { return pos_type(off_type(-1)); }
        virtual pos_type seekpos(pos_type, ios_base::openmode = ios_base::in | ios_base::out) { return pos_type(off_type(-1)); }
        virtual int sync() { return 0; }

        /* 29.6.3.5.3 Get area */
        virtual streamsize showmanyc() { return 0; }
        virtual streamsize xsgetn(char_type* s, streamsize n) {
            for (streamsize i = 0; i < n; i++) {
                const int_type new_char = sbumpc();
                if (new_char != traits_type::eof()) {
                    *(s + i) = traits_type::to_char_type(new_char);
                } else {
                    return i;
                }
            }

            return n;
        }

        virtual int_type underflow() { return traits::eof(); }
        virtual int_type uflow() { 
            int_type result = underflow();
            gbump(1);
            return result;
        }

        /* 29.6.3.5.4 Putback */
        virtual int_type pbackfail(int_type = traits::eof()) { return traits::eof(); }

        /* 29.6.3.5.5 Put area */
        virtual streamsize xsputn(const char_type* s, streamsize n) {
            for (streamsize i = 0; i < n; i++) {
                const int_type result = sputc(*(s + i));
                if (result == traits_type::eof()) return i;
            }
            return n;
        }
        virtual int_type overflow(int_type = traits::eof()) { return traits::eof(); }
    };
}