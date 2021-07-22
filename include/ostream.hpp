#pragma once

#include "iosfwd.hpp"
#include "ios.hpp"
#include "exception.hpp"
#include "type_traits.hpp"
#include "utility.hpp"

namespace std {
    template<class charT, class traits>
    class basic_ostream : virtual public basic_ios<charT, traits> {
    public:
        using char_type = charT;
        using int_type = typename traits::int_type;
        using pos_type = typename traits::pos_type;
        using off_type = typename traits::off_type;
        using traits_type = traits;

        explicit basic_ostream(basic_streambuf<char_type, traits>* sb) : basic_ios<charT, traits>() {
            basic_ios<charT, traits>::init(sb);
        }

        virtual ~basic_ostream() = default;

        class sentry {
        private:
            bool ok;
            basic_ostream* os;
        public:;
            explicit sentry(basic_ostream& os) : ok(false), os(&os) {
                if (!os.good()) {
                    return;
                } else if (os.tie()) {
                    os.tie()->flush();
                }

                ok = true;
            }

            ~sentry() {
                if ((os->flags() & ios_base::unitbuf) && !uncaught_exceptions() && os->good()) {
                    try {
                        const int res = os->rdbuf()->pubsync();
                        if (res == -1) {
                            os->iostate = os->rdstate() | ios_base::badbit;
                        }
                    } catch (...) {}
                }
            }

            explicit operator bool() const {
                return ok;
            }

            sentry(const sentry&) = delete;
            sentry* operator=(const sentry&) = delete;
        };

        template<class Invocable>
        void __formatted_output_function(Invocable fn) {
            ios_base::iostate err = ios_base::goodbit;
            const sentry sent{*this};

            if (sent) {
                try {
                    fn(err);
                } catch (...) {
                    err |= ios_base::badbit;
                    if ((this->exceptions() & ios_base::badbit) != 0) {
                        ios_base::rdstate |= err;
                        throw;
                    }
                }

                this->setstate(err);
            }
        };

        basic_ostream& operator<<(basic_ostream& (*pf)(basic_ostream&)) {
            return pf(*this);
        }

        basic_ostream& operator<<(basic_ios<charT, traits>& (*pf)(basic_ios<charT, traits>&)) {
            pf(*this);
            return *this;
        }

        basic_ostream& operator<<(ios_base& (*pf)(ios_base&)) {
            pf(*this);
            return *this;
        }

        basic_ostream& operator<<(bool n) {
            __formatted_output_function([&](ios_base::iostate& err) {
                if (use_facet<num_put<charT, ostreambuf_iterator<charT, traits>>>(this->getloc()).put(*this, *this, this->fill(), n).failed()) {
                    err |= ios_base::failbit;
                }
            });

            return *this;
        }

        basic_ostream& operator<<(short n) {
            __formatted_output_function([&](ios_base::iostate& err) {
                const bool failed = use_facet<num_put<charT, ostreambuf_iterator<charT, traits>>>(this->getloc())
                    .put(*this, *this, this->fill(), this->baseflags == ios_base::oct || this->baseflags == ios_base::hex
                        ? static_cast<long>(static_cast<unsigned short>(n))
                        : static_cast<long>(n)).failed();

                if (failed) {
                    err |= ios_base::failbit;
                }
            });

            return *this;
        }

        basic_ostream& operator<<(unsigned short n) {
            __formatted_output_function([&](ios_base::iostate& err) {
                if (use_facet<num_put<charT, ostreambuf_iterator<charT, traits>>>(this->getloc()).put(*this, *this, this->fill(), static_cast<unsigned long>(n)).failed()) {
                    err |= ios_base::failbit;
                }
            });

            return *this;
        }

        basic_ostream& operator<<(int n) {
            __formatted_output_function([&](ios_base::iostate& err) {
                const bool failed = use_facet<num_put<charT, ostreambuf_iterator<charT, traits>>>(this->getloc())
                    .put(*this, *this, this->fill(), this->baseflags == ios_base::oct || this->baseflags == ios_base::hex
                        ? static_cast<long>(static_cast<unsigned int>(n))
                        : static_cast<long>(n)).failed();

                if (failed) {
                    err |= ios_base::failbit;
                }
            });

            return *this;
        }

        basic_ostream& operator<<(unsigned int n) {
            __formatted_output_function([&](ios_base::iostate& err) {
                if (use_facet<num_put<charT, ostreambuf_iterator<charT, traits>>>(this->getloc()).put(*this, *this, this->fill(), static_cast<unsigned long>(n)).failed()) {
                    err |= ios_base::failbit;
                }
            });

            return *this;
        }

        basic_ostream& operator<<(long n) {
            __formatted_output_function([&](ios_base::iostate& err) {
                if (use_facet<num_put<charT, ostreambuf_iterator<charT, traits>>>(this->getloc()).put(*this, *this, this->fill(), n).failed()) {
                    err |= ios_base::failbit;
                }
            });

            return *this;
        }

        basic_ostream& operator<<(unsigned long n) {
            __formatted_output_function([&](ios_base::iostate& err) {
                if (use_facet<num_put<charT, ostreambuf_iterator<charT, traits>>>(this->getloc()).put(*this, *this, this->fill(), n).failed()) {
                    err |= ios_base::failbit;
                }
            });

            return *this;
        }

        basic_ostream& operator<<(long long n) {
            __formatted_output_function([&](ios_base::iostate& err) {
                if (use_facet<num_put<charT, ostreambuf_iterator<charT, traits>>>(this->getloc()).put(*this, *this, this->fill(), n).failed()) {
                    err |= ios_base::failbit;
                }
            });

            return *this;
        }

        basic_ostream& operator<<(unsigned long long n) {
            __formatted_output_function([&](ios_base::iostate& err) {
                if (use_facet<num_put<charT, ostreambuf_iterator<charT, traits>>>(this->getloc()).put(*this, *this, this->fill(), n).failed()) {
                    err |= ios_base::failbit;
                }
            });

            return *this;
        }

        basic_ostream& operator<<(float f) {
            __formatted_output_function([&](ios_base::iostate& err) {
                if (use_facet<num_put<charT, ostreambuf_iterator<charT, traits>>>(this->getloc()).put(*this, *this, this->fill(), static_cast<double>(f)).failed()) {
                    err |= ios_base::failbit;
                }
            });

            return *this;
        }

        basic_ostream& operator<<(double f) {
            __formatted_output_function([&](ios_base::iostate& err) {
                if (use_facet<num_put<charT, ostreambuf_iterator<charT, traits>>>(this->getloc()).put(*this, *this, this->fill(), f).failed()) {
                    err |= ios_base::failbit;
                }
            });

            return *this;
        }

        basic_ostream& operator<<(long double f) {
            __formatted_output_function([&](ios_base::iostate& err) {
                if (use_facet<num_put<charT, ostreambuf_iterator<charT, traits>>>(this->getloc()).put(*this, *this, this->fill(), f).failed()) {
                    err |= ios_base::failbit;
                }
            });

            return *this;
        }

        basic_ostream& operator<<(const void* p) {
            __formatted_output_function([&](ios_base::iostate& err) {
                if (use_facet<num_put<charT, ostreambuf_iterator<charT, traits>>>(this->getloc()).put(*this, *this, this->fill(), p).failed()) {
                    err |= ios_base::failbit;
                }
            });

            return *this;
        }

        basic_ostream& operator<<(nullptr_t) {
            return *this << "nullptr";
        }

        basic_ostream& operator<<(basic_streambuf<charT, traits>* sb) {
            ios_base::iostate err = ios_base::goodbit;
            const sentry sent{*this};

            if (sent) {
                if (!sb) {
                    err |= ios_base::badbit;
                } else {
                    try {
                        bool character_extracted = false;

                        while (true) {
                            const typename traits::int_type c = sb->sgetc();
                            if (traits::eq_int_type(c, traits::eof())) {
                                err |= ios_base::eofbit;
                                break;
                            }

                            try {
                                if (traits::eq(traits::eof(), this->rdbuf()->sputc(traits::to_char_type(c)))) {
                                    break;
                                } else {
                                    sb->sbumpc();
                                    character_extracted = true;
                                }
                            } catch (...) {
                                err |= ios_base::failbit;
                                if ((this->exceptions() & ios_base::failbit) != 0) {
                                    ios_base::rdstate |= err;
                                    throw;
                                }
                                break;
                            }
                        }

                        if (!character_extracted) {
                            err |= ios_base::failbit;
                        }
                    } catch (...) {
                        err |= ios_base::badbit;
                        if ((this->exceptions() & ios_base::badbit) != 0) {
                            ios_base::rdstate |= err;
                            throw;
                        }
                    }
                }

                this->setstate(err);
            }

            return *this;
        }

        basic_ostream& put(char_type c) {
            ios_base::iostate err = ios_base::goodbit;
            const sentry sent{*this};

            if (sent) {
                try {
                    if (traits::eq_int_type(traits::eof(), this->rdbuf()->sputc(c))) {
                        err |= ios_base::badbit;
                    }
                } catch (...) {
                    err |= ios_base::badbit;
                    if ((this->exceptions() & ios_base::badbit) != 0) {
                        ios_base::rdstate |= err;
                        throw;
                    }
                }

                this->setstate(err);
            }

            return *this;
        }

        basic_ostream& write(const char_type* s, streamsize n) {
            ios_base::iostate err = ios_base::goodbit;
            const sentry sent{*this};

            if (sent) {
                try {
                    for (streamsize i = 0; i < n; i++, s++) {
                        if (traits::eq_int_type(traits::eof(), this->rdbuf()->sputc(*s))) {
                            err |= ios_base::badbit;
                            break;
                        }
                    }
                } catch (...) {
                    err |= ios_base::badbit;
                    if ((this->exceptions() & ios_base::badbit) != 0) {
                        ios_base::rdstate |= err;
                        throw;
                    }
                }

                this->setstate(err);
            }

            return *this;
        }

        basic_ostream<charT, traits>& flush() {
            if (!this->rdbuf()) {
                return *this;
            }

            ios_base::iostate err = ios_base::goodbit;
            const sentry sent{*this};
            if (sent) {
                try {
                    if (this->rdbuf()->pubsync() == -1) {
                        err |= ios_base::badbit;
                    }
                } catch (...) {
                    err |= ios_base::badbit;
                    if ((this->exceptions() & ios_base::badbit) != 0) {
                        ios_base::rdstate |= err;
                        throw;
                    }
                }
            
                this->setstate(err);
            }

            return *this;
        }

        pos_type tellp() {
            const sentry sent{*this};
            ios_base::iostate err = ios_base::goodbit;

            if (sent) {
                try {
                    if (this->fail()) {
                        return pos_type(-1);
                    } else {
                        return this->rdbuf()->pubseekoff(0, ios_base::cur, ios_base::out);
                    }
                } catch (...) {
                    err |= ios_base::badbit;
                    if ((this->exceptions() & ios_base::badbit) != 0) {
                        ios_base::rdstate |= err;
                        throw;
                    }
                }

                this->setstate(err);
            }

            return pos_type(-1);
        }

        basic_ostream& seekp(pos_type pos) {
            const sentry sent{*this};
            ios_base::iostate err = ios_base::goodbit;

            if (sent) {
                try {
                    if (!this->fail() && this->rdbuf()->pubseekpos(pos, ios_base::out) == pos_type(off_type(-1))) {
                        err |= ios_base::failbit;
                    }
                } catch (...) {
                    err |= ios_base::badbit;
                    if ((this->exceptions() & ios_base::badbit) != 0) {
                        ios_base::rdstate |= err;
                        throw;
                    }
                }

                this->setstate(err);
            }

            return *this;
        }

        basic_ostream& seekp(off_type off, ios_base::seekdir dir) {
            const sentry sent{*this};
            ios_base::iostate err = ios_base::goodbit;

            if (sent) {
                try {
                    if (!this->fail() && this->rdbuf()->pubseekoff(off, dir, ios_base::out) == pos_type(off_type(-1))) {
                        err |= ios_base::failbit;
                    }
                } catch (...) {
                    err |= ios_base::badbit;
                    if ((this->exceptions() & ios_base::badbit) != 0) {
                        ios_base::rdstate |= err;
                        throw;
                    }
                }

                this->setstate(err);
            }

            return *this;
        }


    protected:
        basic_ostream(const basic_ostream&) = delete;

        basic_ostream(basic_ostream&& rhs) : basic_ios<charT, traits>() {
            basic_ios<charT, traits>::move(rhs);
        }

        basic_ostream& operator=(const basic_ostream&) = delete;

        basic_ostream& operator=(basic_ostream&& rhs) {
            swap(rhs);
            return *this;
        }

        void swap(basic_ostream& rhs) {
            basic_ios<charT, traits>::swap(rhs);
        }
    };

    template<class charT, class traits>
    basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, charT c) {
        os.__formatted_output_function([&](ios_base::iostate& err) {
            const streamsize padding = os.width() >= 1 ? os.width() - 1 : 0;
            if (padding != 0 && ((os.flags() & ios_base::adjustfield) != ios_base::left)) {
                for (streamsize i = 0; i < padding; i++) {
                    if (traits::eq_int_type(os.rdbuf()->sputc(os.fill()), traits::eof())) {
                        err |= ios_base::eofbit;
                        return;
                    }
                }
            }

            if (traits::eq_int_type(os.rdbuf()->sputc(c), traits::eof())) {
                err |= ios_base::eofbit;
                return;
            }

            if (padding != 0 && ((os.flags() & ios_base::adjustfield) == ios_base::left)) {
                for (streamsize i = 0; i < padding; i++) {
                    if (traits::eq_int_type(os.rdbuf()->sputc(os.fill()), traits::eof())) {
                        err |= ios_base::eofbit;
                        return;
                    }
                }
            }

            os.width(0);
        });
        return os;
    }

    template<class charT, class traits>
    basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, char c) {
        return os << os.widen(c);
    }

    template<class traits>
    basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>& os, char c) {
        os.__formatted_output_function([&](ios_base::iostate& err) {
            const streamsize padding = os.width() >= 1 ? os.width() - 1 : 0;
            if (padding != 0 && ((os.flags() & ios_base::adjustfield) != ios_base::left)) {
                for (streamsize i = 0; i < padding; i++) {
                    if (traits::eq_int_type(os.rdbuf()->sputc(os.fill()), traits::eof())) {
                        err |= ios_base::eofbit;
                        return;
                    }
                }
            }

            if (traits::eq_int_type(os.rdbuf()->sputc(c), traits::eof())) {
                err |= ios_base::eofbit;
                return;
            }

            if (padding != 0 && ((os.flags() & ios_base::adjustfield) == ios_base::left)) {
                for (streamsize i = 0; i < padding; i++) {
                    if (traits::eq_int_type(os.rdbuf()->sputc(os.fill()), traits::eof())) {
                        err |= ios_base::eofbit;
                        return;
                    }
                }
            }

            os.width(0);
        });
        return os;
    }


    template<class traits>
    basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>& os, signed char c) {
        return os << static_cast<char>(c);
    }

    template<class traits>
    basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>& os, unsigned char c) {
        return os << static_cast<char>(c);
    }

    template<class traits>
    basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>&, wchar_t) = delete;
    template<class traits>
    basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>&, char8_t) = delete;
    template<class traits>
    basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>&, char16_t) = delete;
    template<class traits>
    basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>&, char32_t) = delete;
    template<class traits>
    basic_ostream<wchar_t, traits>& operator<<(basic_ostream<wchar_t, traits>&, char8_t) = delete;
    template<class traits>
    basic_ostream<wchar_t, traits>& operator<<(basic_ostream<wchar_t, traits>&, char16_t) = delete;
    template<class traits>
    basic_ostream<wchar_t, traits>& operator<<(basic_ostream<wchar_t, traits>&, char32_t) = delete;

    template<class charT, class traits>
    basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const charT* s) {
        os.__formatted_output_function([&](ios_base::iostate& err) {
            const typename traits::size_type slen = traits::length(s);
            const streamsize padding = os.width() - slen >= 0 ? os.width() - slen : 0;
            if (padding != 0 && ((os.flags() & ios_base::adjustfield) != ios_base::left)) {
                for (streamsize i = 0; i < padding; i++) {
                    if (traits::eq_int_type(os.rdbuf()->sputc(os.fill()), traits::eof())) {
                        err |= ios_base::eofbit;
                        return;
                    }
                }
            }

            for (streamsize i = 0; i < slen; i++) {
                if (traits::eq_int_type(os.rdbuf()->sputc(s[i]), traits::eof())) {
                    err |= ios_base::eofbit;
                    return;
                }
            }

            if (padding != 0 && ((os.flags() & ios_base::adjustfield) == ios_base::left)) {
                for (streamsize i = 0; i < padding; i++) {
                    if (traits::eq_int_type(os.rdbuf()->sputc(os.fill()), traits::eof())) {
                        err |= ios_base::eofbit;
                        return;
                    }
                }
            }

            os.width(0);
        });

        return os;
    }

    template<class charT, class traits>
    basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const char* s) {
        os.__formatted_output_function([&](ios_base::iostate& err) {
            const typename traits::size_type slen = char_traits<char>::length(s);
            const streamsize padding = os.width() - slen >= 0 ? os.width() - slen : 0;
            if (padding != 0 && ((os.flags() & ios_base::adjustfield) != ios_base::left)) {
                for (streamsize i = 0; i < padding; i++) {
                    if (traits::eq_int_type(os.rdbuf()->sputc(os.fill()), traits::eof())) {
                        err |= ios_base::eofbit;
                        return;
                    }
                }
            }

            for (streamsize i = 0; i < slen; i++) {
                if (traits::eq_int_type(os.rdbuf()->sputc(os.widen(s[i])), traits::eof())) {
                    err |= ios_base::eofbit;
                    return;
                }
            }

            if (padding != 0 && ((os.flags() & ios_base::adjustfield) == ios_base::left)) {
                for (streamsize i = 0; i < padding; i++) {
                    if (traits::eq_int_type(os.rdbuf()->sputc(os.fill()), traits::eof())) {
                        err |= ios_base::eofbit;
                        return;
                    }
                }
            }

            os.width(0);
        });
        
        return os;
    }

    template<class traits>
    basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>& os, const char* s) {
        os.__formatted_output_function([&](ios_base::iostate& err) {
            const typename traits::size_type slen = traits::length(s);
            const streamsize padding = os.width() - slen >= 0 ? os.width() - slen : 0;
            if (padding != 0 && ((os.flags() & ios_base::adjustfield) != ios_base::left)) {
                for (streamsize i = 0; i < padding; i++) {
                    if (traits::eq_int_type(os.rdbuf()->sputc(os.fill()), traits::eof())) {
                        err |= ios_base::eofbit;
                        return;
                    }
                }
            }

            for (streamsize i = 0; i < slen; i++) {
                if (traits::eq_int_type(os.rdbuf()->sputc(s[i]), traits::eof())) {
                    err |= ios_base::eofbit;
                    return;
                }
            }

            if (padding != 0 && ((os.flags() & ios_base::adjustfield) == ios_base::left)) {
                for (streamsize i = 0; i < padding; i++) {
                    if (traits::eq_int_type(os.rdbuf()->sputc(os.fill()), traits::eof())) {
                        err |= ios_base::eofbit;
                        return;
                    }
                }
            }

            os.width(0);
        });
        
        return os;
    }

    template<class traits>
    basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>& os, const signed char* s) {
        return os << reinterpret_cast<const char*>(s);
    }

    template<class traits>
    basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>& os, const unsigned char* s) {
        return os << reinterpret_cast<const char*>(s);
    }

    template<class traits>
    basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>&, const wchar_t*) = delete;
    template<class traits>
    basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>&, const char8_t*) = delete;
    template<class traits>
    basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>&, const char16_t*) = delete;
    template<class traits>
    basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>&, const char32_t*) = delete;
    template<class traits>
    basic_ostream<wchar_t, traits>& operator<<(basic_ostream<wchar_t, traits>&, const char8_t*) = delete;
    template<class traits>
    basic_ostream<wchar_t, traits>& operator<<(basic_ostream<wchar_t, traits>&, const char16_t*) = delete;
    template<class traits>
    basic_ostream<wchar_t, traits>& operator<<(basic_ostream<wchar_t, traits>&, const char32_t*) = delete;

    using ostream = basic_ostream<char>;
    using wostream = basic_ostream<wchar_t>;

    template<class charT, class traits>
    basic_ostream<charT, traits>& endl(basic_ostream<charT, traits>& os) {
        os.put(os.widen('\n'));
        os.flush();
        return os;
    }

    template<class charT, class traits>
    basic_ostream<charT, traits>& ends(basic_ostream<charT, traits>& os) {
        os.put(charT());
        return os;
    }

    template<class charT, class traits>
    basic_ostream<charT, traits>& flush(basic_ostream<charT, traits>& os) {
        os.flush();
        return os;
    }

    // TODO: Implement after <syncstream>.
    /*
    template<class charT, class traits>
    basic_ostream<charT, traits>& emit_on_flush(basic_ostream<charT, traits>& os) {
        return os;
    }
    template<class charT, class traits>
    basic_ostream<charT, traits>& noemit_on_flush(basic_ostream<charT, traits>& os) {
        return os;
    }
    template<class charT, class traits>
    basic_ostream<charT, traits>& flush_emit(basic_ostream<charT, traits>& os) {
        return os;
    } */

    template<class Ostream, class T>
    Ostream&& operator<<(Ostream&& os, const T& x)
    requires requires { os << x; } && is_convertible_v<Ostream&, ios_base&> {
        os << x;
        return move(os);
    }
}