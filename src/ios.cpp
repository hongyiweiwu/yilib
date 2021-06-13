#include "ios.hpp"
#include "system_error.hpp"
#include "string.hpp"
#include "cstring.hpp"
#include "locale.hpp"
#include "bit.hpp"
#include "cstddef.hpp"

namespace std {
    error_code make_error_code(io_errc e) noexcept {
        return error_code(static_cast<int>(e), iostream_category());
    }

    error_condition make_error_condition(io_errc e) noexcept {
        return error_condition(static_cast<int>(e), iostream_category());
    }

    namespace __internal {
        struct iostream_category : public error_category {
            const char* name() const noexcept override { return "iostream"; }
            string message(int ev) const override { return std::strerror(ev); }
        };
    }

    const error_category& iostream_category() noexcept {
        static constexpr __internal::iostream_category category{};
        return category;
    }

    ios_base::failure::failure(const string& msg, const error_code& ec) : system_error(ec, msg) {}
    ios_base::failure::failure(const char* msg, const error_code& ec) : system_error(ec, msg) {}

    ios_base::fmtflags ios_base::flags() const { return fmtfl; }
    ios_base::fmtflags ios_base::flags(ios_base::fmtflags fmtfl) { 
        const fmtflags prev = this->fmtfl;
        this->fmtfl = fmtfl;
        return prev;
    }

    ios_base::fmtflags ios_base::setf(ios_base::fmtflags fmtfl) { return flags(fmtfl); }
    ios_base::fmtflags ios_base::setf(ios_base::fmtflags fmtfl, ios_base::fmtflags mask) {
        const fmtflags prev = this->fmtfl;
        unsetf(mask);
        this->fmtfl = fmtfl & mask;
        return prev;
    }

    void ios_base::unsetf(fmtflags mask) { fmtfl &= ~mask; }

    streamsize ios_base::precision() const { return prec; }
    streamsize ios_base::precision(streamsize prec) { 
        streamsize old = this->prec;
        this->prec = prec;
        return old;
    }

    streamsize ios_base::width() const { return wide; }
    streamsize ios_base::width(streamsize wide) {
        streamsize old = this->wide;
        this->wide = wide;
        return old;
    }

    locale ios_base::imbue(const locale& loc) {
        locale old = getloc();
        *(this->loc) = loc;

        for (size_t i = callback_count - 1; i >= 0; i--) 
            (*callbacks[i])(imbue_event, *this, callback_indices[i]);
        return old;
    }

    locale ios_base::getloc() const { return *loc; }

    constinit size_t ios_base::index = 0;
    int ios_base::xalloc() { return __atomic_fetch_add(&index, 1, __ATOMIC_SEQ_CST); }

    long& ios_base::iword(int idx) {
        try {
            if (!iarray) {
                iarray = new long[1]{0};
                iarray_size = 1;
            }

            if (static_cast<size_t>(idx) >= iarray_size) {
                const size_t new_size = bit_ceil(static_cast<size_t>(idx) + 1);
                long* new_ptr = new long[new_size]{0};
                std::memcpy(new_ptr, iarray, sizeof(long) * iarray_size);
                delete[] iarray;
                iarray = new_ptr;
                iarray_size = new_size;
            }

            return iarray[idx];
        } catch (...) {
            static constinit long error_return = 0;
            setstate(badbit);
            return error_return;
        }
    }

    void*& ios_base::pword(int idx) {
        try {
            if (!parray) {
                parray = new void*[1]{0};
                parray_size = 1;
            }

            if (static_cast<size_t>(idx) >= iarray_size) {
                const size_t new_size = bit_ceil(static_cast<size_t>(idx) + 1);
                void** new_ptr = new void*[new_size]{nullptr};
                std::memcpy(new_ptr, parray, sizeof(void*) * parray_size);
                delete[] parray;
                parray = new_ptr;
                parray_size = new_size;
            }

            return parray[idx];
        } catch (...) {
            static constinit void* error_return = nullptr;
            setstate(badbit);
            return error_return;
        }
    }

    void ios_base::setstate(iostate) {}

    ios_base::~ios_base() {
        for (size_t i = callback_count - 1; i >= 0; i--)
            (*callbacks[i])(erase_event, *this, callback_indices[i]);

        delete loc;
        delete[] callbacks;
        delete[] callback_indices;
        delete[] iarray;
        delete[] parray;
    }

    void ios_base::register_callback(event_callback fn, int idx) {
        if (callback_count == 0 || has_single_bit(callback_count)) {
            const size_t new_size = 1 << bit_width(callback_count);
            event_callback* callbacks = new event_callback[new_size];
            int* callback_indices = new int[new_size];
            std::memcpy(callbacks, this->callbacks, sizeof(event_callback) * callback_count);
            std::memcpy(callback_indices, this->callback_indices, sizeof(int) * callback_count);
            delete[] this->callbacks;
            delete[] this->callback_indices;
            this->callbacks = callbacks;
            this->callback_indices = callback_indices;
        }

        callbacks[callback_count + 1] = fn;
        callback_indices[callback_count + 1] = idx;
        callback_count++;
    }

    bool ios_base::sync_with_stdio(bool sync) {
        static constinit bool prev_state = true;
        bool old = prev_state;
        prev_state = sync;
        return old;
    }

    ios_base& boolalpha(ios_base& str) { 
        str.setf(ios_base::boolalpha);
        return str;
    }

    ios_base& noboolalpha(ios_base& str) { 
        str.unsetf(ios_base::boolalpha);
        return str;
    }

    ios_base& showbase(ios_base& str) { 
        str.setf(ios_base::showbase);
        return str;
    }

    ios_base& noshowbase(ios_base& str) { 
        str.unsetf(ios_base::showbase);
        return str;
    }

    ios_base& showpoint(ios_base& str) {
        str.setf(ios_base::showpoint);
        return str;
    }

    ios_base& noshowpoint(ios_base& str) {
        str.unsetf(ios_base::showpoint);
        return str;
    }

    ios_base& showpos(ios_base& str) {
        str.setf(ios_base::showpos);
        return str;
    }

    ios_base& noshowpos(ios_base& str) {
        str.unsetf(ios_base::showpos);
        return str;
    }

    ios_base& skipws(ios_base& str) {
        str.setf(ios_base::skipws);
        return str;
    }

    ios_base& noskipws(ios_base& str) {
        str.unsetf(ios_base::skipws);
        return str;
    }

    ios_base& uppercase(ios_base& str) {
        str.setf(ios_base::uppercase);
        return str;
    }

    ios_base& nouppercase(ios_base& str) {
        str.unsetf(ios_base::uppercase);
        return str;
    }

    ios_base& unitbuf(ios_base& str) {
        str.setf(ios_base::unitbuf);
        return str;
    }

    ios_base& nounitbuf(ios_base& str) {
        str.unsetf(ios_base::unitbuf);
        return str;
    }

    ios_base& internal(ios_base& str) {
        str.setf(ios_base::internal, ios_base::adjustfield);
        return str;
    }

    ios_base& left(ios_base& str) {
        str.setf(ios_base::left, ios_base::adjustfield);
        return str;
    }

    ios_base& right(ios_base& str) {
        str.setf(ios_base::right, ios_base::adjustfield);
        return str;
    }

    ios_base& dec(ios_base& str) {
        str.setf(ios_base::dec, ios_base::basefield);
        return str;
    }

    ios_base& hex(ios_base& str) {
        str.setf(ios_base::hex, ios_base::basefield);
        return str;
    }

    ios_base& oct(ios_base& str) {
        str.setf(ios_base::oct, ios_base::basefield);
        return str;
    }

    ios_base& fixed(ios_base& str) {
        str.setf(ios_base::fixed, ios_base::floatfield);
        return str;
    }

    ios_base& scientific(ios_base& str) {
        str.setf(ios_base::scientific, ios_base::floatfield);
        return str;
    }

    ios_base& hexfloat(ios_base& str) {
        str.setf(ios_base::fixed | ios_base::scientific, ios_base::floatfield);
        return str;
    }

    ios_base& defaultfloat(ios_base& str) {
        str.unsetf(ios_base::floatfield);
        return str;
    }
}