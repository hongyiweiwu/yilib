#pragma once

#include "iosfwd.hpp"
#include "ios.hpp"
#include "locale.hpp"
#include "streambuf.hpp"

namespace std {
    template<class charT, class traits>
    class basic_istream : virtual public basic_ios<charT, traits> {
    public:
        /* Executes a callable using the FormattedInputFunction named requirement routine. The callable must be a void-returning function that takes
         * a reference to an ios_base::iostate variable. */
        template<class Invocable>
        void __formatted_input_function(Invocable fn) {
            const sentry s{*this, false};
            if (bool(s)) {
                ios_base::iostate err = ios_base::goodbit;
                try {
                    fn(err);
                } catch (...) {
                    err |= ios_base::badbit;
                    ios_base::rdstate |= err;
                    if (this->exception() & ios_base::badbit) {
                        throw;
                    }
                }

                this->setstate(err);
            }
        }

        using char_type = typename basic_ios<charT, traits>::char_type;
        using int_type = typename basic_ios<charT, traits>::int_type;
        using pos_type = typename basic_ios<charT, traits>::pos_type;
        using off_type = typename basic_ios<charT, traits>::off_type;
        using traits_type = typename basic_ios<charT, traits>::traits_type;

        explicit basic_istream(basic_streambuf<charT, traits>* sb): basic_ios<charT, traits>(), gcnt(0) {
            basic_ios<charT, traits>::init(sb);
        }

        virtual ~basic_istream() = default;

        class sentry {
        private:
            bool ok;
        public:
            explicit sentry(basic_istream& is, bool noskipws = false) : ok(false) {
                if (!is.good()) {
                    is.setstate(ios_base::failbit);
                    return;
                }

                if (is.tie() != nullptr) {
                    is.tie()->flush();
                }

                if (!noskipws && (is.flags() & ios_base::skipws) != 0) {
                    const std::ctype<charT>& ctype = use_facet<std::ctype<charT>>(is.getloc());
                    while (true) {
                        const typename traits::int_type c = is.rdbuf()->sgetc();
                        if (ctype.is(ctype.space, c) != 0) {
                            break;
                        } else if (traits::eq(traits::eof(), c)) {
                            is.setstate(ios_base::failbit | ios_base::eofbit);
                            return;
                        }
                        is.rdbuf()->sbumpc();
                    }
                }

                ok = true;
            }

            ~sentry() = default;

            explicit operator bool() const {
                return ok;
            }

            sentry(const sentry&) = delete;
            sentry& operator=(const sentry&) = delete;
        };

        basic_istream& operator>>(basic_istream& (*pf)(basic_istream&)) {
            return pf(*this);
        }

        basic_istream& operator>>(basic_ios<charT, traits>& (*pf)(basic_ios<charT, traits>&)) {
            pf(*this);
            return *this;
        }

        basic_istream& operator>>(ios_base& (*pf)(ios_base&)) {
            pf(*this);
            return *this;
        }

        basic_istream& operator>>(bool& n) {
            __formatted_input_function([&](ios_base::iostate& err) {
                use_facet<num_get<charT, istreambuf_iterator<charT, traits>>>(this->getloc()).get(*this, 0, *this, err, n);
            });

            return *this;
        }

        basic_istream& operator>>(short& n) {
            __formatted_input_function([&](ios_base::iostate& err) {
                long lval;
                use_facet<num_get<charT, istreambuf_iterator<charT, traits>>>(this->getloc()).get(*this, 0, *this, err, lval);
                if (lval < numeric_limits<short>::min()) {
                    err |= ios_base::failbit;
                    n = numeric_limits<short>::min();
                } else if (numeric_limits<short>::max() < lval) {
                    err |= ios_base::failbit;
                    n = numeric_limits<short>::max();
                } else {
                    n = static_cast<short>(lval);
                }
            });

            return *this;
        }

        basic_istream& operator>>(unsigned short& n) {
            __formatted_input_function([&](ios_base::iostate& err) {
                use_facet<num_get<charT, istreambuf_iterator<charT, traits>>>(this->getloc()).get(*this, 0, *this, err, n);
            });

            return *this;
        }
        
        basic_istream& operator>>(int& n) {
            __formatted_input_function([&](ios_base::iostate& err) {
                long lval;
                use_facet<num_get<charT, istreambuf_iterator<charT, traits>>>(this->getloc()).get(*this, 0, *this, err, lval);
                if (lval < numeric_limits<int>::min()) {
                    err |= ios_base::failbit;
                    n = numeric_limits<int>::min();
                } else if (numeric_limits<int>::max() < lval) {
                    err |= ios_base::failbit;
                    n = numeric_limits<int>::max();
                } else {
                    n = static_cast<int>(lval);
                }
            });

            return *this;
        }

        basic_istream& operator>>(unsigned int& n) {
            __formatted_input_function([&](ios_base::iostate& err) {
                use_facet<num_get<charT, istreambuf_iterator<charT, traits>>>(this->getloc()).get(*this, 0, *this, err, n);
            });

            return *this;
        }

        basic_istream& operator>>(long& n) {
            __formatted_input_function([&](ios_base::iostate& err) {
                use_facet<num_get<charT, istreambuf_iterator<charT, traits>>>(this->getloc()).get(*this, 0, *this, err, n);
            });

            return *this;
        }

        basic_istream& operator>>(unsigned long& n) {
            __formatted_input_function([&](ios_base::iostate& err) {
                use_facet<num_get<charT, istreambuf_iterator<charT, traits>>>(this->getloc()).get(*this, 0, *this, err, n);
            });

            return *this;
        }

        basic_istream& operator>>(long long& n) {
            __formatted_input_function([&](ios_base::iostate& err) {
                use_facet<num_get<charT, istreambuf_iterator<charT, traits>>>(this->getloc()).get(*this, 0, *this, err, n);
            });

            return *this;
        }

        basic_istream& operator>>(unsigned long long& n) {
            __formatted_input_function([&](ios_base::iostate& err) {
                use_facet<num_get<charT, istreambuf_iterator<charT, traits>>>(this->getloc()).get(*this, 0, *this, err, n);
            });

            return *this;
        }
        
        basic_istream& operator>>(float& f) {
            __formatted_input_function([&](ios_base::iostate& err) {
                use_facet<num_get<charT, istreambuf_iterator<charT, traits>>>(this->getloc()).get(*this, 0, *this, err, f);
            });

            return *this;
        }
        
        basic_istream& operator>>(double& f) {
            __formatted_input_function([&](ios_base::iostate& err) {
                use_facet<num_get<charT, istreambuf_iterator<charT, traits>>>(this->getloc()).get(*this, 0, *this, err, f);
            });

            return *this;
        }
        
        basic_istream& operator>>(long double& f) {
            __formatted_input_function([&](ios_base::iostate& err) {
                use_facet<num_get<charT, istreambuf_iterator<charT, traits>>>(this->getloc()).get(*this, 0, *this, err, f);
            });

            return *this;
        }

        basic_istream& operator>>(void*& p) {
            __formatted_input_function([&](ios_base::iostate& err) {
                use_facet<num_get<charT, istreambuf_iterator<charT, traits>>>(this->getloc()).get(*this, 0, *this, err, p);
            });

            return *this;
        }

        basic_istream& operator>>(basic_streambuf<char_type, traits>* sb) {
            ios_base::iostate err = ios_base::goodbit;
            gcnt = 0;
            sentry sent{*this, true};

            if (sent) {
                try {
                    if (!sb) {
                        err |= ios_base::failbit;
                    } else {
                        std::size_t n_read = 0;
            
                        while (true) {
                            const typename traits::int_type c = this->rdbuf()->sgetc();
                            if (traits::eq_int_type(c, traits::eof())) {
                                err |= ios_base::eofbit;
                                break;
                            } else if (traits::eq_int_type(sb->sputc(traits::to_char_type(c)), traits::eof())) {
                                break;
                            }
            
                            n_read++;
                            this->rdbuf()->sbumpc();
            
                            gcnt = n_read;
                            if (gcnt == 0) {
                                err |= ios_base::failbit;
                            }
                        }  
                    }
                } catch (...) {
                    err |= ios_base::badbit;

                    if (gcnt == 0) {
                        err |= ios_base::failbit;
                    }

                    ios_base::rdstate |= err;
                    if (this->exception() & ios_base::badbit || this->exception() & ios_base::failbit) {
                        throw;
                    }
                }

                this->setstate(err);
            }

            return *this;
        }

        streamsize gcount() const {
            return gcnt;
        }

        int_type get() {
            int_type res = traits::eof();

            ios_base::iostate err = ios_base::goodbit;
            gcnt = 0;
            sentry s{*this, true};
            if (s) {
                try {
                    res = this->rdbuf()->sbumpc();
                    if (traits::eq_int_type(res, traits::eof())) {
                        err |= ios_base::failbit | ios_base::eofbit;
                    } else {
                        gcnt = 1;
                    }
                } catch (...) {
                    err |= ios_base::badbit;
                    ios_base::rdstate |= err;
                    if (this->exception() & ios_base::badbit) {
                        throw;
                    }
                }

                this->setstate(err);
            }

            return res;
        }

        basic_istream& get(char_type& c) {
            const int_type res = get();

            if (!traits::eq_int_type(res, traits::eof())) {
                c = traits::to_char_type(res);
            }

            return *this;
        }

        basic_istream& get(char_type* s, streamsize n) {
            return get(s, n, this->widen('\n'));
        }

        basic_istream& get(char_type* s, streamsize n, char_type delim) {
            ios_base::iostate err = ios_base::goodbit;
            gcnt = 0;
            sentry sent{*this, true};

            if (sent) {
                std::size_t n_read = 0;
                try {
                    while (static_cast<streamsize>(n_read) < n) {
                        const int_type c = this->rdbuf()->sgetc();
                        if (traits::eq_int_type(c, traits::eof())) {
                            err |= ios_base::eofbit;
                            break;
                        } else if (traits::eq(traits::to_char_type(c), delim)) {
                            break;
                        } else {
                            *s = traits::to_char_type(c);
                            s++;
                            n_read++;
                            this->rdbuf()->sbumpc();
                        }
                    }

                    gcnt = n_read;
                } catch (...) {
                    err |= ios_base::badbit;
                    ios_base::rdstate |= err;
                    if (this->exceptions() & ios_base::badbit) {
                        if (n > 0) {
                            *s = this->widen('\0');
                        }
                        throw;
                    }
                }
            }

            if (n > 0) {
                *s = this->widen('\0');
            }
            if (gcnt == 0) {
                err |= ios_base::failbit;
            }
            this->setstate(err);
            return *this;
        }

        basic_istream& get(basic_streambuf<char_type, traits>& sb) {
            return get(sb, this->widen('\n'));
        }

        basic_istream& get(basic_streambuf<char_type, traits>& sb, char_type delim) {
            ios_base::iostate err = ios_base::goodbit;
            gcnt = 0;
            sentry s{*this, true};
            if (s) {
                try {
                    std::size_t n_read = 0;
                    while (true) {
                        const int_type c = this->rdbuf()->sgetc();
                        if (traits::eq_int_type(c, traits::eof())) {
                            err |= ios_base::eofbit;
                            break;
                        } else if (traits::eq(traits::to_char_type(c), delim)) {
                            break;
                        } else if (traits::eq_int_type(sb.sputc(traits::to_char_type(c)), traits::eof())) {
                            break;
                        } else {
                            n_read++;
                            this->rdbuf()->sbumpc();
                        }
                    }

                    gcnt = n_read;
                } catch (...) {
                    err |= ios_base::badbit;
                }
            }

            if (gcnt == 0) {
                err |= ios_base::failbit;
            }
            this->setstate(err);

            return *this;
        }

        basic_istream& getline(char_type* s, streamsize n) {
            return getline(s, n, this->widen('\n'));
        }

        basic_istream& getline(char_type* s, streamsize n, char_type delim) {
            ios_base::iostate err = ios_base::goodbit;
            gcnt = 0;
            sentry sent{*this, true};

            if (sent) {
                std::size_t n_read = 0;
                try {
                    while (true) {
                        const int_type c = this->rdbuf()->sgetc();
                        if (traits::eq_int_type(c, traits::eof())) {
                            err |= ios_base::eofbit;
                            break;
                        } else if (traits::eq(traits::to_char_type(c), delim)) {
                            n_read++;
                            this->rdbuf()->sbumpc();
                            break;
                        } else if (n < 1 || n_read == n - 1) {
                            err |= ios_base::failbit;
                            break;
                        } else {
                            *s = traits::to_char_type(this->rdbuf()->sbumpc());
                            s++;
                            n_read++;
                        }
                    }

                    gcnt = n_read;
                } catch (...) {
                    err |= ios_base::badbit;
                    ios_base::rdstate |= err;
                    if (this->exceptions() & ios_base::badbit) {
                        if (n > 0) {
                            *s = this->widen('\0');
                        }
                        throw;
                    }
                }
            }

            if (n > 0) {
                *s = this->widen('\0');
            }

            if (gcnt == 0) {
                err |= ios_base::failbit;
            }
            this->setstate(err);
            return *this;
        }

        basic_istream& ignore(streamsize n = 1, int_type delim = traits::eof()) {
            ios_base::iostate err = ios_base::goodbit;
            gcnt = 0;
            sentry sent{*this, true};

            if (sent) {
                streamsize n_read = 0;
                try {
                    while (true) {
                        if (n != numeric_limits<streamsize>::max() && n_read == n) {
                            break;
                        }

                        const int_type c = this->rdbuf()->sgetc();
                        if (traits::eq_int_type(c, traits::eof())) {
                            err |= ios_base::eofbit;
                            break;
                        } 
                        
                        this->rdbuf()->sbumpc();
                        n_read++;
                        if (traits::eq_int_type(c, delim)) {
                            break;
                        }
                    }
                } catch (...) {
                    err |= ios_base::badbit;
                    ios_base::rdstate |= err;
                    if (this->exceptions() & ios_base::badbit) {
                        throw;
                    }
                }

                gcnt = n_read;
                this->setstate(err);
            }

            return *this;
        }

        int_type peek() {
            ios_base::iostate err = ios_base::goodbit;
            sentry sent{*this, true};

            int_type c = traits::eof();
            if (sent) {
                try {
                    c = this->rdbuf()->sgetc();
                    if (traits::eq_int_type(c, traits::eof())) {
                        err |= ios_base::eofbit;
                    }   
                } catch (...) {
                    err |= ios_base::badbit;
                    ios_base::rdstate |= err;
                    if (this->exceptions() & ios_base::badbit) {
                        throw;
                    }
                }
            }

            return c;
        }
        
        basic_istream& read(char_type* s, streamsize n) {
            gcnt = 0;
            ios_base::iostate err = ios_base::goodbit;
            sentry sent{*this, true};

            if (sent) {
                try {
                    std::size_t i = 0; 
                    while (i < n) {
                        const int_type c = this->rdbuf()->sgetc();
                        if (traits::eq_int_type(c, traits::eof())) {
                            err |= ios_base::failbit | ios_base::eofbit;
                        } else {
                            i++;
                            *s = traits::to_char_type(c);
                            s++;
                            this->rdbuf()->sbumpc();
                        }
                    }

                    gcnt = i;
                } catch (...) {
                    err |= ios_base::badbit;
                    ios_base::rdstate |= err;
                    if (this->exceptions() & ios_base::badbit) {
                        throw;
                    }
                }
            } else {
                err |= ios_base::failbit;
            }

            this->setstate(err);
            return *this;
        }

        streamsize readsome(char_type* s, streamsize n) {
            gcnt = 0;
            ios_base::iostate err = ios_base::goodbit;
            sentry sent{*this, true};

            if (sent) {
                try {
                    std::size_t i = 0;
                    const streamsize avail = this->rdbuf()->in_avail();
                    if (avail == -1 || avail == 0) {
                        err |= ios_base::eofbit;
                    } else {
                        while (i < min(avail, n)) {
                            *s = traits::to_char_type(this->rdbuf()->sgetc());
                            s++;
                            i++;
                            this->rdbuf()->sbumpc();
                        }

                        if (i < n) {
                            err |= ios_base::eofbit | ios_base::failbit;
                        }
                    }

                    gcnt = i;
                } catch (...) {
                    err |= ios_base::badbit;
                    ios_base::rdstate |= err;
                    if (this->exceptions() & ios_base::badbit) {
                        throw;
                    }
                }
            } else {
                err |= ios_base::failbit;
            }

            this->setstate(err);
            return *this;
        }

        basic_istream& putback(char_type c) {
            ios_base::rdstate &= ~ios_base::eofbit;
            gcnt = 0;
            ios_base::iostate err = ios_base::goodbit;
            sentry sent{*this, true};

            if (sent) {
                try {
                    if (!this->good()) {
                        err |= ios_base::failbit;
                    } else if (!this->rdbuf()) {
                        err |= ios_base::badbit;
                    } else if (const typename traits::int_type res = this->rdbuf()->sputbackc(c); traits::eq_int_type(res, traits::eof())) {
                        err |= ios_base::badbit;
                    }
                } catch (...) {
                    err |= ios_base::badbit;
                    ios_base::rdstate |= err;
                    if (this->exceptions() & ios_base::badbit) {
                        throw;
                    }
                }

                this->setstate(err);
            }

            return *this;
        }

        basic_istream& unget() {
            ios_base::rdstate &= ~ios_base::eofbit;
            gcnt = 0;
            ios_base::iostate err = ios_base::goodbit;
            sentry sent{*this, true};

            if (sent) {
                try {
                    if (!this->good()) {
                        err |= ios_base::failbit;
                    } else if (!this->rdbuf()) {
                        err |= ios_base::badbit;
                    } else if (const typename traits::int_type c = this->rdbuf()->sungetc(); traits::eq_int_type(c, traits::eof())) {
                        err |= ios_base::badbit;
                    }
                } catch (...) {
                    err |= ios_base::badbit;
                    ios_base::rdstate |= err;
                    if (this->exceptions() & ios_base::badbit) {
                        throw;
                    }
                }

                this->setstate(err);
            }

            return *this;
        }

        int sync() {
            ios_base::iostate err = ios_base::goodbit;
            sentry sent{*this, true};

            bool ok = true;

            if (sent) {
                try {
                    if (!this->rdbuf()) {
                        ok = false;
                    } else if (this->rdbuf()->pubsync() == -1) {
                        err |= ios_base::badbit;
                        ok = false;
                    }
                } catch (...) {
                    err |= ios_base::badbit;
                    ios_base::rdstate |= err;
                    if (this->exceptions() & ios_base::badbit) {
                        throw;
                    }
                }

                this->setstate(err);
            }

            return ok ? 0 : -1;
        }

        pos_type tellg() {
            ios_base::iostate err = ios_base::goodbit;
            sentry sent{*this, true};

            if (sent) {
                try {
                    if (!this->fail()) {
                        return this->rdbuf()->pubseekoff(0, this->cur, this->in);
                    }
                } catch (...) {
                    err |= ios_base::badbit;
                    ios_base::rdstate |= err;
                    if (this->exceptions() & ios_base::badbit) {
                        throw;
                    }
                }
            }

            return pos_type(-1);
        }

        basic_istream& seekg(pos_type pos) {
            ios_base::rdstate &= ~ios_base::eofbit;
            ios_base::iostate err = ios_base::goodbit;
            sentry sent{*this, true};

            if (sent) {
                try {
                    if (this->fail()) {
                        err |= ios_base::failbit;
                    } else {
                        this->rdbuf()->pubseekpos(pos, ios_base::in);
                    }
                } catch (...) {
                    err |= ios_base::badbit;
                    ios_base::rdstate |= err;
                    if (this->exceptions() & ios_base::badbit) {
                        throw;
                    }
                }

                this->setstate(err);
            }

            return *this;
        }

        basic_istream& seekg(off_type off, ios_base::seekdir dir) {
            ios_base::rdstate &= ~ios_base::eofbit;
            ios_base::iostate err = ios_base::goodbit;
            sentry sent{*this, true};

            if (sent) {
                try {
                    if (this->fail()) {
                        err |= ios_base::failbit;
                    } else {
                        this->rdbuf()->pubseekoff(off, dir, ios_base::in);
                    }
                } catch (...) {
                    err |= ios_base::badbit;
                    ios_base::rdstate |= err;
                    if (this->exceptions() & ios_base::badbit) {
                        throw;
                    }
                }

                this->setstate(err);
            }

            return *this; 
        }

    protected:
        basic_istream(const basic_istream&) = delete;
        basic_istream(basic_istream&& rhs) : basic_ios<charT, traits>(), gcnt(rhs.gcnt) {
            basic_ios<charT, traits>::move(rhs);
            rhs.gcnt = 0;
        }

        basic_istream& operator=(const basic_istream&) = delete;

        basic_istream& operator=(basic_istream&& rhs) {
            swap(rhs);
            return *this;
        }

        void swap(basic_istream& rhs) {
            basic_ios<charT, traits>::swap(rhs);
            std::swap(gcnt, rhs.gcnt);
        }
    private:
        streamsize gcnt;

        template<class CT, class TT>
        friend basic_istream<CT, TT>& ws(basic_istream<CT, TT>& is);
    };

    template<class charT, class traits>
    basic_istream<charT, traits>& operator>>(basic_istream<charT, traits>& in, charT& c) {
        basic_istream<charT, traits>::formatted_input_function([&](ios_base::iostate& err) {
            const typename traits::int_type i = in.rdbuf()->sgetc();
            if (traits::eq_int_type(c, traits::eof())) {
                err |= ios_base::failbit;
            } else {
                c = traits::to_char_type(i);
            }
        });

        return in;
    }

    template<class traits>
    basic_istream<char, traits>& operator>>(basic_istream<char, traits>& in, unsigned char& c) {
        return in >> static_cast<char>(c);
    }

    template<class traits>
    basic_istream<char, traits>& operator>>(basic_istream<char, traits>& in, signed char& c) {
        return in >> static_cast<char>(c);
    }

    template<class charT, class traits, std::size_t N>
    basic_istream<charT, traits>& operator>>(basic_istream<charT, traits>& in, charT (&s)[N]) {
        basic_istream<charT, traits>::formatted_input_function([&](ios_base::iostate& err) {
            streamsize n = in->width() > 0 ? min(std::size_t(in->width()), N) : N;
            streamsize i = 0;
            const std::ctype<charT>& ct = use_facet<std::ctype<charT>>(in.getloc());

            while (i < n) {
                const typename traits::int_type c = in.rdbuf()->sgetc();
                if (traits::eq_int_type(c, traits::eof())) {
                    err |= ios_base::eofbit;
                    break;
                } else if (ct.is(ct.space, traits::to_char_type(c))) {
                    break;
                } else {
                    s[i] = traits::to_char_type(c);
                    i++;
                }
            }

            *s = in->widen('\0');
            in.width(0);

            if (i == 0) {
                err |= ios_base::failbit;
            }
        });

        return in;
    }
    
    template<class traits, std::size_t N>
    basic_istream<char, traits>& operator>>(basic_istream<char, traits>& in, unsigned char (&s)[N]) {
        return in >> static_cast<char(&)[N]>(s);
    }
    
    template<class traits, std::size_t N>
    basic_istream<char, traits>& operator>>(basic_istream<char, traits>& in, signed char (&s)[N]) {
        return in >> static_cast<char(&)[N]>(s);
    }

    template<class charT, class traits>
    class basic_iostream : public basic_istream<charT, traits>, public basic_ostream<charT, traits> {
    public:
        using char_type = charT;
        using int_type = typename traits::int_type;
        using pos_type = typename traits::pos_type;
        using off_type = typename traits::off_type;
        using traits_type = traits;

        explicit basic_iostream(basic_streambuf<charT, traits>* sb) : basic_istream<charT, traits>(sb), basic_ostream<charT, traits>(sb) {}

        virtual ~basic_iostream() = default;

    protected:
        basic_iostream(const basic_iostream&) = delete;
        basic_iostream(basic_iostream&& rhs) : basic_istream<charT, traits>(rhs.sb), basic_ostream<charT, traits>() {}

        basic_iostream& operator=(const basic_iostream&) = delete;
        basic_iostream& operator=(basic_iostream&& rhs) {
            swap(rhs);
            return *this;
        }

        void swap(basic_iostream& rhs) {
            basic_istream<charT, traits>::swap(rhs);
        }
    };

    template<class charT, class traits>
    basic_istream<charT, traits>& ws(basic_istream<charT, traits>& is) {
        ios_base::iostate err = ios_base::goodbit;
        const typename basic_istream<charT, traits>::sentry sent{is, true};

        if (sent) {
            try {
                const std::ctype<charT>& ctype = use_facet<std::ctype<charT>>(is.getloc());
                while (true) {
                    const typename traits::int_type c = is.rdbuf()->sgetc();
                    if (ctype.is(ctype.space, c) != 0) {
                        break;
                    } else if (traits::eq(traits::eof(), c)) {
                        err |= ios_base::eofbit;
                        break;
                    }
                    is.rdbuf()->sbumpc();
                }
            } catch (...) {
                err |= ios_base::badbit;
                is->iostate = is->rdstate() | err | (is->rdbuf() ? ios_base::goodbit : ios_base::badbit);
                if (is->exceptions() & ios_base::badbit) {
                    throw;
                }
            }

            is->setstate(err);
        }

        return is;
    }

    template<class Istream, class T>
    Istream&& operator>>(Istream&& is, T&& x)
    requires requires { is >> forward<T>(x); } && is_convertible_v<Istream&, ios_base&> {
        is >> forward<T>(x);
        return move(is);
    }
}