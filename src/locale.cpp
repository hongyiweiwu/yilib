#include "locale.hpp"
#include "clocale.hpp"
#include "utility.hpp"
#include "string.hpp"
#include "cstddef.hpp"
#include "cctype.hpp"
#include "cwctype.hpp"
#include "cwchar.hpp"
#include "algorithm.hpp"
#include "cstdlib.hpp"
#include "climits.hpp"
#include "cstring.hpp"
#include "type_traits.hpp"
#include "util/utility_traits.hpp"
#include "stdexcept.hpp"
#include "bit.hpp"
#include "tuple.hpp"
#include "cstdint.hpp"
#include "optional.hpp"
#include "limits.hpp"
#include "memory.hpp"
#include "iterator.hpp"
#include "cstdio.hpp"
#include "ios.hpp"

namespace std {
    // locale::facet
    locale::facet::facet(std::size_t refs) : refs(refs) {}

    unsigned long locale::facet::increment_ref() noexcept {
        return __atomic_add_fetch(&refs, 1, __ATOMIC_SEQ_CST);
    }

    unsigned long locale::facet::decrement_ref() noexcept {
        const unsigned long result_ref = __atomic_sub_fetch(&refs, 1, __ATOMIC_SEQ_CST);
        if (!result_ref) delete this;
        return result_ref;
    }

    // locale::id
    constinit std::size_t locale::id::last_assigned_n = 0;

    // locale
    locale::locale() noexcept : locale(classic()) {}
    locale::locale(const locale& other) noexcept : n(), facets(other.facets), facets_arr_size(other.facets_arr_size) {
        n = std::make_unique<char[]>(std::strlen(n.get()) + 1);
        std::strcpy(n.get(), other.n.get());
        for (std::size_t i = 0; i < facets_arr_size; i++)
            if (facets[i]) facets[i]->increment_ref();
    }

    locale::locale(const char* std_name) : n() {
        if (!std_name) {
            throw runtime_error("Invalid std_name in locale constructor.");
        }

        n = std::make_unique<char[]>(std::strlen(std_name) + 1);
        std::strcpy(n.get(), std_name);

        static const std::size_t max_id = max({ std::ctype<char>::id.n, std::ctype<wchar_t>::id.n,
            codecvt<char, char, std::mbstate_t>::id.n, codecvt<char16_t, char8_t, std::mbstate_t>::id.n,
            codecvt<char32_t, char8_t, std::mbstate_t>::id.n, codecvt<wchar_t, char, std::mbstate_t>::id.n,
            numpunct<char>::id.n, numpunct<wchar_t>::id.n, num_get<char>::id.n, num_get<wchar_t>::id.n });

        facets_arr_size = max_id + 1;
        facets = new locale::facet*[max_id + 1]();

        facets[std::ctype<char>::id.n] = new ctype_byname<char>(std_name, 1);
        facets[std::ctype<wchar_t>::id.n] = new ctype_byname<wchar_t>(std_name, 1);
        facets[codecvt<char, char, std::mbstate_t>::id.n] = new codecvt_byname<char, char, std::mbstate_t>(std_name, 1);
        facets[codecvt<char16_t, char8_t, std::mbstate_t>::id.n] = new codecvt_byname<char16_t, char8_t, std::mbstate_t>(std_name, 1);
        facets[codecvt<char32_t, char8_t, std::mbstate_t>::id.n] = new codecvt_byname<char32_t, char8_t, std::mbstate_t>(std_name, 1);
        facets[codecvt<wchar_t, char, std::mbstate_t>::id.n] = new codecvt_byname<wchar_t, char, std::mbstate_t>(std_name, 1);
        facets[numpunct<char>::id.n] = new numpunct_byname<char>(std_name, 1);
        facets[numpunct<wchar_t>::id.n] = new numpunct_byname<wchar_t>(std_name, 1);
        facets[num_get<char>::id.n] = new num_get<char>(1);
        facets[num_get<wchar_t>::id.n] = new num_get<wchar_t>(1);
    }

    locale::locale(const string& std_name) : locale(std_name.c_str()) {}

    locale::locale(const locale& other, const char* std_name, category c) : locale(other) {
        if (!std_name) throw runtime_error("Invalid std_name in constructor of locale.");

        static constexpr auto replace_facet = [](locale::facet** facet_arr, std::size_t idx, locale::facet* replacement) {
            if (facet_arr[idx]) facet_arr[idx]->decrement_ref();
            facet_arr[idx] = replacement;
        };

        if (c & collate) {}

        if (c & ctype) {
            replace_facet(facets, std::ctype<char>::id.n, new ctype_byname<char>(std_name, 1));
            replace_facet(facets, std::ctype<wchar_t>::id.n, new ctype_byname<wchar_t>(std_name, 1));
            replace_facet(facets, codecvt<char, char, std::mbstate_t>::id.n, new codecvt_byname<char, char, std::mbstate_t>(std_name, 1));
            replace_facet(facets, codecvt<char16_t, char8_t, std::mbstate_t>::id.n, new codecvt_byname<char16_t, char8_t, std::mbstate_t>(std_name, 1));
            replace_facet(facets, codecvt<char32_t, char8_t, std::mbstate_t>::id.n, new codecvt_byname<char32_t, char8_t, std::mbstate_t>(std_name, 1));
            replace_facet(facets, codecvt<wchar_t, char, std::mbstate_t>::id.n, new codecvt_byname<wchar_t, char, std::mbstate_t>(std_name, 1));
        }

        if (c & monetary) {}
        if (c & numeric) {
            replace_facet(facets, numpunct<char>::id.n, new numpunct_byname<char>(std_name, 1));
            replace_facet(facets, numpunct<wchar_t>::id.n, new numpunct_byname<wchar_t>(std_name, 1));
            replace_facet(facets, num_get<char>::id.n, new num_get<char>(1));
            replace_facet(facets, num_get<wchar_t>::id.n, new num_get<wchar_t>(1));
        }
        if (c & time) {}
        if (c & messages) {}
    }

    locale::locale(const locale& other, const string& std_name, category c) : locale(other, std_name.c_str(), c) {}

    locale::locale(const locale& other, const locale& one, category c) : locale(other) {
        if (other.name() == "*" && one.name() == "*") {
            std::strcpy(n.get(), "*");
        }

        static constexpr auto replace_facet = [](locale::facet** facet_arr, std::size_t idx, locale::facet* replacement) {
            if (facet_arr[idx]) facet_arr[idx]->decrement_ref();
            facet_arr[idx] = replacement;
            if (replacement) replacement->increment_ref();
        };

        if (c & collate) {}

        if (c & ctype) {
            replace_facet(facets, std::ctype<char>::id.n, one.facets[std::ctype<char>::id.n]);
            replace_facet(facets, std::ctype<wchar_t>::id.n, one.facets[std::ctype<wchar_t>::id.n]);
            replace_facet(facets, codecvt<char, char, std::mbstate_t>::id.n, one.facets[codecvt<char, char, std::mbstate_t>::id.n]);
            replace_facet(facets, codecvt<char16_t, char8_t, std::mbstate_t>::id.n, one.facets[codecvt<char16_t, char8_t, std::mbstate_t>::id.n]);
            replace_facet(facets, codecvt<char32_t, char8_t, std::mbstate_t>::id.n, one.facets[codecvt<char32_t, char8_t, std::mbstate_t>::id.n]);
            replace_facet(facets, codecvt<wchar_t, char, std::mbstate_t>::id.n, one.facets[codecvt<wchar_t, char, std::mbstate_t>::id.n]);
        }

        if (c & monetary) {}
        if (c & numeric) {
            replace_facet(facets, numpunct<char>::id.n, one.facets[numpunct<char>::id.n]);
            replace_facet(facets, numpunct<wchar_t>::id.n, one.facets[numpunct<wchar_t>::id.n]);
            replace_facet(facets, num_get<char>::id.n, one.facets[num_get<char>::id.n]);
            replace_facet(facets, num_get<wchar_t>::id.n, one.facets[num_get<wchar_t>::id.n]);
        }
        if (c & time) {}
        if (c & messages) {}
    }

    locale::~locale() {
        for (std::size_t i = 0; i < facets_arr_size; i++)
            if (facets[i]) facets[i]->decrement_ref();

        delete[] facets;
    }

    const locale& locale::operator=(const locale& other) noexcept {
        n = std::make_unique<char[]>(std::strlen(other.n.get()) + 1);
        std::strcpy(n.get(), other.n.get());
        facets = other.facets;
        facets_arr_size = other.facets_arr_size;

        for (std::size_t i = 0; i < facets_arr_size; i++) {
            if (facets[i]) {
                facets[i]->increment_ref();
            }
        }

        return *this;
    }

    string locale::name() const { 
        return n.get(); 
    }

    bool locale::operator==(const locale& other) const {
        if (name() != "*" && other.name() != "*" && n == other.n) {
            return true;
        } else if (n == other.n && facets == other.facets && facets_arr_size == other.facets_arr_size) {
            return true;
        } else {
            return false;
        }
    }

    locale locale::global(const locale& other) {
        static locale global_loc = classic();
        if (other.name() != "*") std::setlocale(LC_ALL, other.name().c_str());
        locale old = other;
        swap(global_loc, old);
        return old;
    }

    const locale& locale::classic() {
        static const std::size_t max_facet_id = max({
            std::ctype<char>::id.n, std::ctype<wchar_t>::id.n,
            codecvt<char, char, std::mbstate_t>::id.n, codecvt<char16_t, char8_t, std::mbstate_t>::id.n,
            codecvt<char32_t, char8_t, std::mbstate_t>::id.n, codecvt<wchar_t, char, std::mbstate_t>::id.n,
            numpunct<char>::id.n, numpunct<wchar_t>::id.n, num_get<char>::id.n, num_get<wchar_t>::id.n
        });

        static facet** facets = new facet*[max_facet_id + 1]();

        facets[std::ctype<char>::id.n] = new std::ctype<char>(nullptr, false, 1);
        facets[std::ctype<wchar_t>::id.n] = new std::ctype<wchar_t>(1);
        facets[codecvt<char, char, std::mbstate_t>::id.n] = new codecvt<char, char, std::mbstate_t>(1);
        facets[codecvt<char16_t, char8_t, std::mbstate_t>::id.n] = new codecvt<char16_t, char8_t, std::mbstate_t>(1);
        facets[codecvt<char32_t, char8_t, std::mbstate_t>::id.n] = new codecvt<char32_t, char8_t, std::mbstate_t>(1);
        facets[codecvt<wchar_t, char, std::mbstate_t>::id.n] = new codecvt<wchar_t, char, std::mbstate_t>(1);

        facets[numpunct<char>::id.n] = new numpunct<char>(1);
        facets[numpunct<wchar_t>::id.n] = new numpunct<wchar_t>(1);
        facets[num_get<char>::id.n] = new num_get<char>(1);
        facets[num_get<wchar_t>::id.n] = new num_get<wchar_t>(1);

        static locale classic_locale = locale(facets, max_facet_id + 1);

        return classic_locale;
    }

    locale::locale(locale::facet** facets, std::size_t facets_arr_size, const char* name) 
        : n(std::make_unique<char[]>(std::strlen(name) + 1)), facets(facets), facets_arr_size(facets_arr_size) {
        std::strcpy(n.get(), name);
    }

    // ctype<char>
    ctype<char>::ctype(const mask* tab, bool del, std::size_t refs) : locale::facet(refs), __internal::__ctype_impl<char>(), tab(tab), del_tab(del) {}
    
    bool ctype<char>::is(mask m, char c) const { return tab[static_cast<unsigned char>(c)] & m; }
    const char* ctype<char>::is(const char* low, const char* high, mask* vec) const {
        for (std::size_t i = 0; low != high; low++, i++) {
            vec[i] = tab[static_cast<unsigned char>(*low)];
        }

        return high;
    }
    const char* ctype<char>::scan_is(mask m, const char* low, const char* high) const {
        while (low != high) {
            if (is(m, *low)) break;
            low++;
        }

        return low;
    }

    const char* ctype<char>::scan_not(mask m, const char* low, const char* high) const {
        while (low != high) {
            if (!is(m, *low)) break;
            low++;
        }

        return low;
    }

    locale::id ctype<char>::id;

    const ctype_base::mask* ctype<char>::table() const noexcept { return tab ? tab : classic_table(); }
    const ctype_base::mask* ctype<char>::classic_table() noexcept {
        static constexpr mask table[table_size] {
            cntrl, cntrl, cntrl, cntrl, cntrl, cntrl, cntrl, cntrl,
            cntrl, cntrl | space | blank, cntrl | space, cntrl | space, cntrl | space, cntrl | space, cntrl, cntrl,
            cntrl, cntrl, cntrl, cntrl, cntrl, cntrl, cntrl, cntrl,
            cntrl, cntrl, cntrl, cntrl, cntrl, cntrl, cntrl, cntrl,
            print | space | blank, print | punct, print | punct, print | punct, print | punct, print | punct, print | punct, print | punct,
            print | punct, print | punct, print | punct, print | punct, print | punct, print | punct, print | punct, print | punct,
            print | digit | xdigit, print | digit | xdigit, print | digit | xdigit, print | digit | xdigit, print | digit | xdigit, print | digit | xdigit, print | digit | xdigit, print | digit | xdigit,
            print | digit | xdigit, print | digit | xdigit, print | punct, print | punct, print | punct, print | punct, print | punct, print | punct,
            print | punct, print | alpha | upper | xdigit, print | alpha | upper | xdigit, print | alpha | upper | xdigit, print | alpha | upper | xdigit, print | alpha | upper | xdigit, print | alpha | upper | xdigit, print | alpha | upper,
            print | alpha | upper, print | alpha | upper, print | alpha | upper, print | alpha | upper, print | alpha | upper, print | alpha | upper, print | alpha | upper, print | alpha | upper,
            print | alpha | upper, print | alpha | upper, print | alpha | upper, print | alpha | upper, print | alpha | upper, print | alpha | upper, print | alpha | upper, print | alpha | upper,
            print | alpha | upper, print | alpha | upper, print | alpha | upper, print | punct, print | punct, print | punct, print | punct, print | punct,
            print | punct, print | alpha | lower | xdigit, print | alpha | lower | xdigit, print | alpha | lower | xdigit, print | alpha | lower | xdigit, print | alpha | lower | xdigit, print | alpha | lower | xdigit, print | alpha | lower,
            print | alpha | lower, print | alpha | lower, print | alpha | lower, print | alpha | lower, print | alpha | lower, print | alpha | lower, print | alpha | lower, print | alpha | lower,
            print | alpha | lower, print | alpha | lower, print | alpha | lower, print | alpha | lower, print | alpha | lower, print | alpha | lower, print | alpha | lower, print | alpha | lower,
            print | alpha | lower, print | alpha | lower, print | alpha | lower, print | punct, print | punct, print | punct, print | punct, cntrl
        };

        return table;
    }

    ctype<char>::~ctype() { if (tab && del_tab) delete[] tab; }

    char ctype<char>::do_toupper(char c) const { return std::toupper(c); }
    const char* ctype<char>::do_toupper(char* low, const char* high) const {
        while (low != high) {
            *low = do_toupper(*low);
            low++;
        }

        return high;
    }

    char ctype<char>::do_tolower(char c) const { return std::tolower(c); }
    const char* ctype<char>::do_tolower(char* low, const char* high) const {
        while (low != high) {
            *low = do_tolower(*low);
            low++;
        }

        return high;
    }

    char ctype<char>::do_widen(char c) const { return c; }
    const char* ctype<char>::do_widen(const char* low, const char* high, char* to) const {
        while (low != high) {
            *to = do_widen(*low);
            low++;
            to++;
        }

        return high;
    }
    char ctype<char>::do_narrow(char c, char) const { return c; }
    const char* ctype<char>::do_narrow(const char* low, const char* high, char dfault, char* to) const {
        while (low != high) {
            *to = do_narrow(*low, dfault);
            low++;
            to++;
        }
        return high;
    }

    // ctype<wchar_t>
    ctype<wchar_t>::ctype(std::size_t refs) : locale::facet(refs), __internal::__ctype_impl<wchar_t>() {}

    locale::id ctype<wchar_t>::id;

    bool ctype<wchar_t>::do_is(mask m, wchar_t c) const {
        mask temp;
        do_is(&c, &c + 1, &temp);
        return temp & m;
    }

    const wchar_t* ctype<wchar_t>::do_is(const wchar_t* low, const wchar_t* high, mask* vec) const {
        using test_fn_t = int (*)(std::wint_t);
        /* An array containing the character category tests we need to run through for each character. */
        static constexpr const test_fn_t tests[10] {
            std::iswspace, std::iswprint, std::iswcntrl,
            std::iswupper, std::iswlower, std::iswalpha,
            std::iswdigit, std::iswpunct, std::iswxdigit, std::iswblank
        };

        /* An array where the ith entry is the ith mask we need to add if tests[i] evaluates to true. */
        static constexpr const mask masks[10] {
            space, print, cntrl, upper, lower, alpha, digit, punct, xdigit, blank
        };

        for (; low != high; vec++, low++) {
            *vec = 0;
            for (std::size_t i = 0; i < 10; i++) {
                if (tests[i](*low)) *vec |= masks[i];
            }
        }

        return high;
    }

    const wchar_t* ctype<wchar_t>::do_scan_is(mask m, const wchar_t* low, const wchar_t* high) const {
        while (low != high) {
            if (do_is(m, *low)) break;
            low++;
        }

        return low;
    }

    const wchar_t* ctype<wchar_t>::do_scan_not(mask m, const wchar_t* low, const wchar_t* high) const {
        while (low != high) {
            if (!do_is(m, *low)) break;
            low++;
        }

        return low;
    }

    wchar_t ctype<wchar_t>::do_toupper(wchar_t c) const { return std::towupper(c); }
    const wchar_t* ctype<wchar_t>::do_toupper(wchar_t* low, const wchar_t* high) const {
        while (low != high) {
            *low = do_toupper(*low);
            low++;
        }

        return high;
    }
    wchar_t ctype<wchar_t>::do_tolower(wchar_t c) const { return std::towlower(c); }
    const wchar_t* ctype<wchar_t>::do_tolower(wchar_t* low, const wchar_t* high) const {
        while (low != high) {
            *low = do_tolower(*low);
            low++;
        }

        return high;
    }

    wchar_t ctype<wchar_t>::do_widen(char c) const { return std::btowc(c); }
    const char* ctype<wchar_t>::do_widen(const char* low, const char* high, wchar_t* to) const {
        for (; low != high; low++, to++) {
            *to = do_widen(*low);
        }
        return high;
    }
    
    char ctype<wchar_t>::do_narrow(wchar_t c, char dfault) const { 
        if (const char a = std::wctob(c); c != EOF) {
            return a;
        } else {
            return dfault;
        }
    }

    const wchar_t* ctype<wchar_t>::do_narrow(const wchar_t* low, const wchar_t* high, char dfault, char* to) const {
        for (; low != high; low++, to++) {
            *to = do_narrow(*low, dfault);
        }

        return high;
    }

    // __internal::locale_container
#if YILIB_HAS_XLOCALE
    __internal::__locale_container<true>::__locale_container(const char* loc, int category)
        : loc(::newlocale(category, loc, static_cast<locale_t>(0))) {}

    __internal::__locale_container<true>::~__locale_container<true>() {
        ::freelocale(loc);
    }

    locale_t __internal::__locale_container<true>::get_locale() const noexcept { 
        return loc; 
    }

    std::lconv* __internal::__locale_container<true>::get_lconv() const noexcept {
        return ::localeconv_l(loc);
    }

#else
    __internal::__locale_container<false>::__locale_container(const char* loc, int category) 
        : loc(loc), category(category) {}

    __internal::__locale_container<false>::global_locale_switch::global_locale_switch(const char* loc, int category)
        : category(category), old_loc(std::setlocale(category, NULL)) {
            std::setlocale(category, loc);
        }

    __internal::__locale_container<false>::global_locale_switch::~global_locale_switch() {
        std::setlocale(category, old_loc);
    }

    __internal::__locale_container<false>::global_locale_switch __internal::__locale_container<false>::get_locale() const {
        return {loc, category};
    }

    std::lconv* __internal::__locale_container<false>::get_lconv() const noexcept {
        const global_locale_switch switch = get_locale();
        return std::localeconv();
    }
#endif

    // ctype_byname<wchar_t>
    ctype_byname<wchar_t>::ctype_byname(const char* loc, std::size_t refs)
        : ctype<wchar_t>(refs), __internal::__locale_container<>(loc, LC_CTYPE) {}
        
    ctype_byname<wchar_t>::ctype_byname(const string& loc, std::size_t refs) : ctype_byname(loc.c_str(), refs) {}

    const wchar_t* ctype_byname<wchar_t>::do_is(const wchar_t* low, const wchar_t* high, mask* vec) const {
        auto loc = get_locale();
#if YILIB_HAS_XLOCALE
        using test_fn_t = int (*)(std::wint_t, locale_t);
        /* An array containing the character category tests we need to run through for each character. */
        static constexpr const test_fn_t tests[10] {
            ::iswspace_l, ::iswprint_l, ::iswcntrl_l,
            ::iswupper_l, ::iswlower_l, ::iswalpha_l,
            ::iswdigit_l, ::iswpunct_l, ::iswxdigit_l, ::iswblank_l
        };

        /* An array where the ith entry is the ith mask we need to add if tests[i] evaluates to true. */
        static constexpr const mask masks[10] {
            space, print, cntrl, upper, lower, alpha, digit, punct, xdigit, blank
        };

        for (; low != high; vec++, low++) {
            *vec = 0;
            for (std::size_t i = 0; i < 10; i++) {
                if (tests[i](*low, loc)) *vec |= masks[i];
            }
        }

        return high;
#else
        return ctype<wchar_t>::do_is(low, high, vec);
#endif
    }

    wchar_t ctype_byname<wchar_t>::do_toupper(wchar_t c) const {
        auto loc = get_locale();

#if YILIB_HAS_XLOCALE
        return ::towupper_l(c, loc);
#else
        return std::towupper(c);
#endif
    }

    wchar_t ctype_byname<wchar_t>::do_tolower(wchar_t c) const {
        auto loc = get_locale();

#if YILIB_HAS_XLOCALE
        return ::towlower_l(c, loc);
#else
        return std::towlower(c);
#endif
    }

    wchar_t ctype_byname<wchar_t>::do_widen(char c) const {
        auto loc = get_locale();

#if YILIB_HAS_XLOCALE
        return ::btowc_l(c, loc);
#else
        return std::btowc(c);
#endif
    }

    char ctype_byname<wchar_t>::do_narrow(wchar_t c, char dfault) const {
        auto loc = get_locale();

#if YILIB_HAS_XLOCALE
        const int result = ::wctob_l(c, loc);
#else
        const int result = std::wctob(c);
#endif

        return result == EOF ? dfault : result;
    }

    // ctype_byname<char>
    ctype_byname<char>::ctype_byname(const char* loc, std::size_t refs)
        : ctype<char>(nullptr, false, refs), __internal::__locale_container<>(loc, LC_CTYPE) {}
        
    ctype_byname<char>::ctype_byname(const string& loc, std::size_t refs) : ctype_byname(loc.c_str(), refs) {}

    char ctype_byname<char>::do_toupper(char c) const {
        auto loc = get_locale();

#if YILIB_HAS_XLOCALE
        return ::toupper_l(c, loc);
#else
        return std::toupper(c);
#endif
    }

    char ctype_byname<char>::do_tolower(char c) const {
        auto loc = get_locale();

#if YILIB_HAS_XLOCALE
        return ::tolower_l(c, loc);
#else
        return std::tolower(c);
#endif
    }

    // codecvt<char, char, std::mbstate_t>
    codecvt<char, char, std::mbstate_t>::codecvt(std::size_t refs) : locale::facet(refs) {}

    locale::id codecvt<char, char, std::mbstate_t>::id;

    codecvt_base::result codecvt<char, char, std::mbstate_t>::do_out(std::mbstate_t&, const char* from, const char*, const char*& from_next, char* to, char*, char*& to_next) const { 
        from_next = from;
        to_next = to;
        return noconv; 
    }

    codecvt_base::result codecvt<char, char, std::mbstate_t>::do_unshift(std::mbstate_t&, char* to, char*, char*& to_next) const {
        to_next = to;
        return noconv;
    }

    codecvt_base::result codecvt<char, char, std::mbstate_t>::do_in(std::mbstate_t&, const char* from, const char*, const char*& from_next, char* to, char*, char*& to_next) const { 
        from_next = from;
        to_next = to;
        return noconv; 
    }

    int codecvt<char, char, std::mbstate_t>::do_encoding() const noexcept { return 1; }
    bool codecvt<char, char, std::mbstate_t>::do_always_noconv() const noexcept { return true; }
    int codecvt<char, char, std::mbstate_t>::do_length(std::mbstate_t&, const char* from, const char* end, std::size_t max) const { 
        return min(max, static_cast<size_t>(end - from)); 
    }
    int codecvt<char, char, std::mbstate_t>::do_max_length() const noexcept { return 1; }
    
    // codecvt<char16_t, char8_t, std::mbstate_t>
    codecvt<char16_t, char8_t, std::mbstate_t>::codecvt(std::size_t refs) : locale::facet(refs) {}

    locale::id codecvt<char16_t, char8_t, std::mbstate_t>::id;

    constexpr tuple<codecvt_base::result, char32_t, std::size_t> codecvt<char16_t, char8_t, std::mbstate_t>::utf16_to_utf32(const char16_t* buf, std::size_t max) noexcept {
        if (max < 1) [[unlikely]] {
            return {partial, 0, 0};
        }

        if (*buf <= 0xD7FF || (*buf >= 0xE000 && *buf <= 0xFFFF)) {
            // BMP code points.
            return {ok, *buf, 1};
        }

        if (max < 2) [[unlikely]] {
            return {partial, 0, 0};
        }

        const char16_t high_surrogate = buf[0];
        const char16_t low_surrogate = buf[1];

        if ((high_surrogate >> 10) != 0b110110 || (low_surrogate >> 10) != 0b110111) [[unlikely]] {
            return {error, 0, 0};
        }

        const char32_t code_point = ((high_surrogate & 0b1111111111) << 10) + (low_surrogate & 0b1111111111);

        return {ok, code_point, 2};
    }

    constexpr tuple<codecvt_base::result, std::size_t> codecvt<char16_t, char8_t, std::mbstate_t>::utf32_to_utf16(char32_t c, char16_t* buf) noexcept {
        if (c <= 0xD7FF || (c >= 0xE000 && c <= 0xFFFF)) {
            // BMP code points.
            *buf = static_cast<char16_t>(c);
            return {ok, 1};
        } else if (c >= 0x10000 && c <= 0x10FFFF) {
            // Supplementary plane code points.
            buf[0] = static_cast<char16_t>(((c - 0x10000) >> 10) + 0xD800);
            buf[1] = static_cast<char16_t>(((c - 0x10000) & 0b1111111111) + 0xDC00);
            return {ok, 2};
        } else [[unlikely]] {
            return {error, 0};
        }
    }

    constexpr tuple<codecvt_base::result, std::size_t, std::size_t> codecvt<char16_t, char8_t, std::mbstate_t>::utf8_to_utf16(const char8_t* from_buf, std::size_t from_max, char16_t* buf) noexcept {
        const auto [utf8_to_utf32_result, utf32_char, utf8_consumed] = codecvt<char32_t, char8_t, std::mbstate_t>::utf8_to_utf32(from_buf, from_max);
        if (utf8_to_utf32_result != ok) [[unlikely]] {
            return {utf8_to_utf32_result, 0, 0};
        }

        const auto [utf32_to_utf16_result, utf16_written] = utf32_to_utf16(utf32_char, buf);
        if (utf32_to_utf16_result != ok) [[unlikely]] {
            return {utf32_to_utf16_result, 0, 0};
        } else {
            return {ok, utf8_consumed, utf16_written};
        }
    }

    constexpr tuple<codecvt_base::result, std::size_t, std::size_t> codecvt<char16_t, char8_t, std::mbstate_t>::utf16_to_utf8(const char16_t* from_buf, std::size_t from_max, char8_t* buf) noexcept {
        const auto [utf16_to_utf32_result, utf32_char, utf16_consumed] = utf16_to_utf32(from_buf, from_max);
        if (utf16_to_utf32_result != ok) [[unlikely]] {
            return {utf16_to_utf32_result, 0, 0};
        }

        const auto [utf32_to_utf8_result, utf8_written] = codecvt<char32_t, char8_t, std::mbstate_t>::utf32_to_utf8(utf32_char, buf);
        if (utf32_to_utf8_result != ok) [[unlikely]] {
            return {utf32_to_utf8_result, 0, 0};
        } else {
            return {ok, utf16_consumed, utf8_written};
        }
    }

    codecvt_base::result codecvt<char16_t, char8_t, std::mbstate_t>::do_out(std::mbstate_t&, const char16_t* from, const char16_t* from_end, const char16_t*& from_next,
               char8_t* to, char8_t* to_end, char8_t*& to_next) const {
        char8_t buf[4];
        for (from_next = from, to_next = to; from_next != from_end && to_next != to_end;) {
            const auto [result, utf16_consumed, utf8_produced] = utf16_to_utf8(from_next, from_end - from_next, buf);
            if (result != ok) {
                return result;
            } else if (utf8_produced > static_cast<std::size_t>(to_end - to_next)) {
                return partial;
            } else {
                from_next += utf16_consumed;
                std::memcpy(to_next, buf, utf8_produced);
                to_next += utf8_produced;
            }
        }

        return ok;
    }

    codecvt_base::result codecvt<char16_t, char8_t, std::mbstate_t>::do_unshift(std::mbstate_t&, char8_t* to, char8_t*, char8_t*& to_next) const {
        to_next = to;
        return noconv;
    }

    codecvt_base::result codecvt<char16_t, char8_t, std::mbstate_t>::do_in(std::mbstate_t&, const char8_t* from, const char8_t* from_end, const char8_t*& from_next,
               char16_t* to, char16_t* to_end, char16_t*& to_next) const {
        char16_t buf[2];
        for (from_next = from, to_next = to; from_next != from_end && to_next != to_end;) {
            const auto [result, utf8_consumed, utf16_produced] = utf8_to_utf16(from_next, from_end - from_next, buf);
            if (result != ok) {
                return result;
            } else if (utf16_produced > static_cast<std::size_t>(to_end - to_next)) {
                return partial;
            } else {
                from_next += utf8_consumed;
                std::memcpy(to_next, buf, utf16_produced);
                to_next += utf16_produced;
            }
        }

        return ok;
    }

    int codecvt<char16_t, char8_t, std::mbstate_t>::do_encoding() const noexcept { return 0; }
    bool codecvt<char16_t, char8_t, std::mbstate_t>::do_always_noconv() const noexcept { return false; }
    int codecvt<char16_t, char8_t, std::mbstate_t>::do_length(std::mbstate_t&, const char8_t* from, const char8_t* end, std::size_t max) const {
        // This array will have many race conditions, but we don't really care the content inside. It's simply used to be passed into utf8_to_utf16.
        static char16_t buf[2];
        std::size_t from_converted = 0, to_converted = 0;
        while (to_converted < max) {
            const auto [result, utf8_consumed, utf16_written] = utf8_to_utf16(from, static_cast<std::size_t>(end - from), buf);
            if (result != ok) {
                return from_converted;
            }

            from_converted += utf8_consumed;
            from += utf8_consumed;
            if (to_converted + utf16_written > max) {
                return from_converted;
            } else {
                to_converted += utf16_written;
            }
        }

        return from_converted;
    }
    
    int codecvt<char16_t, char8_t, std::mbstate_t>::do_max_length() const noexcept { return 4; }

    // codecvt<char32_t, char8_t, std::mbstate_t>
    codecvt<char32_t, char8_t, std::mbstate_t>::codecvt(std::size_t refs) : locale::facet(refs) {}

    locale::id codecvt<char32_t, char8_t, std::mbstate_t>::id;

    constexpr tuple<codecvt_base::result, std::size_t> codecvt<char32_t, char8_t, std::mbstate_t>::utf32_to_utf8(char32_t c, char8_t* buf) noexcept {
        if (c < 0x80) {
            buf[0] = 0x80;
            return {ok, 1};
        } else if (c < 0x800) {
            buf[0] = 0b11000000ull | (c >> 6);
            buf[1] = 0b10000000ull | (c & 0b111111ull);
            return {ok, 2};
        } else if (c <= 0xFFFF) {
            buf[0] = 0b11100000ull | (c >> 12);
            buf[1] = 0b10000000ull | ((c >> 6) & 0b111111ull);
            buf[2] = 0b10000000ull | (c & 0b111111ull);
            return {ok, 3};
        } else if (c >= 0x10000 && c <= 0x10FFFF) {
            buf[0] = 0b11110000ull | (c >> 18);
            buf[1] = 0b10000000ull | ((c >> 12) & 0b111111ull);
            buf[2] = 0b10000000ull | ((c >> 6) & 0b111111ull);
            buf[3] = 0b10000000ull | (c & 0b111111ull);
            return {ok, 4};
        } else {
            return {error, 0};
        }
    }

    constexpr tuple<codecvt_base::result, char32_t, std::size_t> codecvt<char32_t, char8_t, std::mbstate_t>::utf8_to_utf32(const char8_t* buf, std::size_t max) noexcept {
        constexpr tuple<codecvt_base::result, char32_t, std::size_t> partial_return = { partial, 0, 0 };
        constexpr tuple<codecvt_base::result, char32_t, std::size_t> error_return = { error, 0, 0 };
        
        if (max < 1) {
            return partial_return;
        }

        const std::size_t leading_ones = countl_one(static_cast<std::uint8_t>(*buf));
        if (leading_ones > 4 || leading_ones == 1) [[unlikely]] {
            return error_return;
        }
        const std::size_t expected_bytes = leading_ones == 0 ? 1 : leading_ones;

        if (expected_bytes > max) {
            return partial_return;
        }

        for (std::size_t i = 2; i <= expected_bytes; i++) {
            if ((buf[i - 1] & 0b11000000) != 0b10000000) [[unlikely]] {
                return error_return;
            }
        }

        switch (expected_bytes) {
            case 1:
                return { ok, *buf, 1 };

            case 2:
                if (const char32_t c = ((*buf & 0b11111) << 6) + (buf[1] & 0b111111); c >= 0x80) {
                    return { ok, c, 2 };
                } else [[unlikely]] {
                    return error_return;
                }

            case 3:
                if (const char32_t c = ((*buf & 0b11111) << 12) + ((buf[1] & 0b111111) << 6) + (buf[2] & 0b111111); c >= 0x800) {
                    return { ok, c, 3 };
                } else [[unlikely]] {
                    return error_return;
                }

            case 4:
                if (const char32_t c = ((*buf & 0b11111) << 18) + ((buf[1] & 0b111111) << 12) + ((buf[2] & 0b111111) << 6) + (buf[3] & 0b111111); c >= 0x10000 && c <= 0x10FFFF) {
                    return { ok, c, 4 };
                } else [[unlikely]] {
                    return error_return;
                }

            default: [[unlikely]]
                return error_return;
        }
    }

    codecvt_base::result codecvt<char32_t, char8_t, std::mbstate_t>::do_out(std::mbstate_t&, const char32_t* from, const char32_t* from_end, const char32_t*& from_next,
                                                                            char8_t* to, char8_t* to_end, char8_t*& to_next) const {
        char8_t buf[4] = {0};
        from_next = from;
        to_next = to;

        for (; from_next != from_end; from++) {
            const auto [result, utf8_bytes] = utf32_to_utf8(*from_next, buf);
            if (result != ok) {
                return result;
            } else if (static_cast<std::size_t>(to_end - to_next) < utf8_bytes) {
                return partial;
            } else {
                std::memcpy(to_next, buf, sizeof(char8_t) * utf8_bytes);   
                to_next += utf8_bytes;
            }
        }

        return ok;
    }

    codecvt_base::result codecvt<char32_t, char8_t, std::mbstate_t>::do_unshift(std::mbstate_t&, char8_t* to, char8_t*, char8_t*& to_next) const {
        to_next = to;
        return noconv;
    }

    codecvt_base::result codecvt<char32_t, char8_t, std::mbstate_t>::do_in(std::mbstate_t&, const char8_t* from, const char8_t* from_end, const char8_t*& from_next,
                                                                           char32_t* to, char32_t* to_end, char32_t*& to_next) const {
        from_next = from;
        to_next = to;
        for (from_next = from, to_next = to; from_next != from_end && to_next != to_end; ) {
            const auto [status, converted_char, converted_utf8] = utf8_to_utf32(from_next, from_end - from_next);
            if (status != ok) {
                return status;
            }

            *to_next = converted_char;
            to_next++;
            from_next += converted_utf8;
        }

        return ok;
    }

    int codecvt<char32_t, char8_t, std::mbstate_t>::do_encoding() const noexcept { return 0; }
    bool codecvt<char32_t, char8_t, std::mbstate_t>::do_always_noconv() const noexcept { return false; }
    int codecvt<char32_t, char8_t, std::mbstate_t>::do_length(std::mbstate_t&, const char8_t* from, const char8_t* end, std::size_t max) const {
        int utf8_count = 0;
        int utf32_count = 0;
        while (from != end && static_cast<std::size_t>(utf32_count) < max) {
            const auto [status, converted_char, converted_utf8] = utf8_to_utf32(from, static_cast<std::size_t>(end - from));
            if (status == partial || status == error) {
                return utf8_count;
            } else {
                utf8_count += converted_utf8;
                utf32_count++;
            }
        }

        return utf8_count;
    }
    int codecvt<char32_t, char8_t, std::mbstate_t>::do_max_length() const noexcept { return 4; }

    // codecvt<wchar_t, char, std::mbstate_t>
    codecvt<wchar_t, char, std::mbstate_t>::codecvt(std::size_t refs) : locale::facet(refs) {}

    locale::id codecvt<wchar_t, char, std::mbstate_t>::id;

    codecvt_base::result codecvt<wchar_t, char, std::mbstate_t>::do_out(std::mbstate_t& state, const wchar_t* from, const wchar_t* from_end, 
                                                                        const wchar_t*& from_next, char* to, char* to_end, char*& to_next) const {
        /* Passed into wcrtomb to store the result of conversion. */
        char buf[MB_LEN_MAX];
        for (from_next = from, to_next = to; from_next != from_end && to_next != to_end;) {
            const std::size_t to_bytes = std::wcrtomb(buf, *to_next, &state);
            if (to_bytes == size_t(-1)) {
                // Unrecognized wide character.
                return error;
            }

            const std::size_t copyable_bytes = std::min(std::size_t(to_end - to_next), to_bytes);
            std::memcpy(to_next, buf, copyable_bytes);
            from_next++;
            to_next += copyable_bytes;
            if (copyable_bytes < to_bytes) {
                return partial;
            }
        }

        return from_next == from_end ? ok : partial;
    }

    codecvt_base::result codecvt<wchar_t, char, std::mbstate_t>::do_unshift(std::mbstate_t& state, char* to, char* to_end, char*& to_next) const {
        static constexpr wchar_t wchar_str = L'\0';
        const wchar_t* dummy = nullptr; // Used as the "from_next" parameter to do_out. Doesn't really do anything.
        return do_out(state, &wchar_str, (&wchar_str) + 1, dummy, to, to_end, to_next);
    }

    codecvt_base::result codecvt<wchar_t, char, std::mbstate_t>::do_in(std::mbstate_t& state, const char* from, const char* from_end, const char*& from_next, wchar_t* to, wchar_t* to_end, wchar_t*& to_next) const { 
        for (from_next = from, to_next = to; from_next != from_end && to_next != to_end; ) {
            const std::size_t from_bytes = std::mbrtowc(to_next, from_next, from_end - from_next, &state);
            switch (from_bytes) {
                case std::size_t(-1):
                    return error;

                case std::size_t(-2):
                    return partial;

                case 0:
                    from_next++;
                    to_next++;
                    break;

                default:
                    from_next += from_bytes;
                    to_next++;
                    break;
            }
        }

        return from_next != from_end ? partial : ok;
    }

    int codecvt<wchar_t, char, std::mbstate_t>::do_encoding() const noexcept {
        if (std::mbtowc(nullptr, nullptr, MB_CUR_MAX)) {
            return -1;
        } else if (MB_CUR_MAX == 1) {
            return 1;
        } else {
            return 0;
        }
    }

    bool codecvt<wchar_t, char, std::mbstate_t>::do_always_noconv() const noexcept { return false; }

    int codecvt<wchar_t, char, std::mbstate_t>::do_length(std::mbstate_t& state, const char* from, const char* end, std::size_t max) const { 
        std::size_t n_intern = 0;
        std::size_t n_extern = 0;
        for (; from < end && n_intern < max; n_intern++) {
            const std::size_t n_converted = std::mbrlen(from, static_cast<std::size_t>(end - from), &state);
            switch (n_converted) {
                case std::size_t(-1):
                case std::size_t(-2):
                    return n_extern;

                case 0:
                    from++;

                default:
                    from += n_converted;
                    n_extern += n_converted;
            }
        }

        return n_extern;
    }

    int codecvt<wchar_t, char, std::mbstate_t>::do_max_length() const noexcept { return MB_CUR_MAX; }

    codecvt_byname<char, char, std::mbstate_t>::codecvt_byname(const char* loc, std::size_t refs)
        : codecvt(refs), __internal::__locale_container<>(loc, LC_CTYPE) {}
    codecvt_byname<char, char, std::mbstate_t>::codecvt_byname(const string& loc, std::size_t refs) : codecvt_byname(loc.c_str(), refs) {}

    codecvt_byname<char16_t, char8_t, std::mbstate_t>::codecvt_byname(const char*, std::size_t refs) : codecvt(refs) {}
    codecvt_byname<char16_t, char8_t, std::mbstate_t>::codecvt_byname(const string& loc, std::size_t refs) : codecvt_byname(loc.c_str(), refs) {}

    codecvt_byname<char32_t, char8_t, std::mbstate_t>::codecvt_byname(const char*, std::size_t refs) : codecvt(refs) {}
    codecvt_byname<char32_t, char8_t, std::mbstate_t>::codecvt_byname(const string& loc, std::size_t refs) : codecvt_byname(loc.c_str(), refs) {}
    
    codecvt_byname<wchar_t, char, std::mbstate_t>::codecvt_byname(const char* loc, std::size_t refs)
        : codecvt<wchar_t, char, std::mbstate_t>(refs), __internal::__locale_container<>(loc, LC_CTYPE) {}
    codecvt_byname<wchar_t, char, std::mbstate_t>::codecvt_byname(const string& loc, std::size_t refs) : codecvt_byname(loc.c_str(), refs) {}

    codecvt_base::result codecvt_byname<wchar_t, char, std::mbstate_t>::do_out(std::mbstate_t& state, const wchar_t* from, const wchar_t* from_end, 
                                                                        const wchar_t*& from_next, char* to, char* to_end, char*& to_next) const {
        /* Passed into wcrtomb to store the result of conversion. */
        char buf[MB_LEN_MAX];
        auto loc = get_locale();
        for (from_next = from, to_next = to; from_next != from_end && to_next != to_end;) {
#if YILIB_HAS_XLOCALE
            const std::size_t to_bytes = ::wcrtomb_l(buf, *to_next, &state, loc);
#else
            const std::size_t to_bytes = std::wcrtomb(buf, *to_next, &state);
#endif
            if (to_bytes == std::size_t(-1)) {
                // Unrecognized wide character.
                return error;
            }

            const std::size_t copyable_bytes = std::min(std::size_t(to_end - to_next), to_bytes);
            std::memcpy(to_next, buf, copyable_bytes);
            from_next++;
            to_next += copyable_bytes;
            if (copyable_bytes < to_bytes) {
                return partial;
            }
        }

        return from_next == from_end ? ok : partial;
    }

    codecvt_base::result codecvt_byname<wchar_t, char, std::mbstate_t>::do_in(std::mbstate_t& state, const char* from, const char* from_end, const char*& from_next, wchar_t* to, wchar_t* to_end, wchar_t*& to_next) const { 
        auto loc = get_locale();
        for (from_next = from, to_next = to; from_next != from_end && to_next != to_end; ) {
#if YILIB_HAS_XLOCALE
            const std::size_t from_bytes = ::mbrtowc_l(to_next, from_next, from_end - from_next, &state, loc);
#else
            const std::size_t from_bytes = std::mbrtowc(to_next, from_next, from_end - from_next, &state);
#endif
            switch (from_bytes) {
                case std::size_t(-1):
                    return error;

                case std::size_t(-2):
                    return partial;

                case 0:
                    from_next++;
                    to_next++;
                    break;

                default:
                    from_next += from_bytes;
                    to_next++;
                    break;
            }
        }

        return from_next != from_end ? partial : ok;
    }

    int codecvt_byname<wchar_t, char, std::mbstate_t>::do_encoding() const noexcept {
        auto loc = get_locale();
#if YILIB_HAS_XLOCALE
        const int is_state_dependent = ::mbtowc_l(nullptr, nullptr, MB_CUR_MAX_L(loc), loc);
#else
        const int is_state_dependent = std::mbtowc(nullptr, nullptr, MB_CUR_MAX);
#endif
        if (is_state_dependent) {
            return -1;
        } else if (MB_CUR_MAX == 1) {
            return 1;
        } else {
            return 0;
        }
    }

    int codecvt_byname<wchar_t, char, std::mbstate_t>::do_length(std::mbstate_t& state, const char* from, const char* end, std::size_t max) const { 
        std::size_t n_intern = 0;
        std::size_t n_extern = 0;
        auto loc = get_locale();
        for (; from < end && n_intern < max; n_intern++) {
#if YILIB_HAS_XLOCALE
            const std::size_t n_converted = ::mbrlen_l(from, static_cast<std::size_t>(end - from), &state, loc);
#else
            const std::size_t n_converted = std::mbrlen(from, static_cast<std::size_t>(end - from), &state);
#endif
            switch (n_converted) {
                case std::size_t(-1):
                case std::size_t(-2):
                    return n_extern;

                case 0:
                    from++;

                default:
                    from += n_converted;
                    n_extern += n_converted;
            }
        }

        return n_extern;
    }

    int codecvt_byname<wchar_t, char, std::mbstate_t>::do_max_length() const noexcept {
        auto loc = get_locale();

#if YILIB_HAS_XLOCALE
        return MB_CUR_MAX_L(loc);
#else
        return MB_CUR_MAX;
#endif
    }

    // num_get<char>
    template class num_get<char>;

    // num_get<wchar_t>
    template class num_get<wchar_t>;

    // numpunct<char>
    numpunct<char>::numpunct(std::size_t refs) : locale::facet(refs), __internal::__numpunct_impl<char>() {}

    locale::id numpunct<char>::id;

    numpunct<char>::char_type numpunct<char>::do_decimal_point() const {
        return '.';
    }

    numpunct<char>::char_type numpunct<char>::do_thousands_sep() const {
        return ',';
    }

    string numpunct<char>::do_grouping() const {
        return "";
    }

    numpunct<char>::string_type numpunct<char>::do_truename() const {
        return "true";
    }

    numpunct<char>::string_type numpunct<char>::do_falsename() const {
        return "false";
    }

    // numpunct<wchar_t>
    numpunct<wchar_t>::numpunct(std::size_t refs) : locale::facet(refs), __internal::__numpunct_impl<wchar_t>() {}

    locale::id numpunct<wchar_t>::id;

    numpunct<wchar_t>::char_type numpunct<wchar_t>::do_decimal_point() const {
        return L'.';
    }

    numpunct<wchar_t>::char_type numpunct<wchar_t>::do_thousands_sep() const {
        return L',';
    }

    string numpunct<wchar_t>::do_grouping() const {
        return "";
    }

    numpunct<wchar_t>::string_type numpunct<wchar_t>::do_truename() const {
        return L"true";
    }

    numpunct<wchar_t>::string_type numpunct<wchar_t>::do_falsename() const {
        return L"false";
    }

    // numpunct_byname<char>
    numpunct_byname<char>::numpunct_byname(const char* loc, std::size_t refs) : numpunct(refs), __internal::__locale_container<>(loc, LC_NUMERIC) {}
    numpunct_byname<char>::numpunct_byname(const string& loc, std::size_t refs) : numpunct_byname(loc.c_str(), refs) {}

    numpunct_byname<char>::char_type numpunct_byname<char>::do_decimal_point() const {
        return *get_lconv()->decimal_point;
    }

    numpunct_byname<char>::char_type numpunct_byname<char>::do_thousands_sep() const {
        return *get_lconv()->thousands_sep;
    }

    string numpunct_byname<char>::do_grouping() const {
        return get_lconv()->grouping;
    }

    numpunct_byname<char>::string_type numpunct_byname<char>::do_truename() const {
        return "true";
    }

    numpunct_byname<char>::string_type numpunct_byname<char>::do_falsename() const {
        return "false";
    }

    // numpunct_byname<wchar_t>
    numpunct_byname<wchar_t>::numpunct_byname(const char* loc, std::size_t refs) : numpunct(refs), __internal::__locale_container<>(loc, LC_NUMERIC) {}
    numpunct_byname<wchar_t>::numpunct_byname(const string& loc, std::size_t refs) : numpunct_byname(loc.c_str(), refs) {}

    numpunct_byname<wchar_t>::char_type numpunct_byname<wchar_t>::do_decimal_point() const {
        const char narrow_result = *get_lconv()->decimal_point;
        auto locale = get_locale();
#if YILIB_HAS_XLOCALE
        return ::btowc_l(narrow_result, locale);
#else
        return std::btowc(narrow_result);
#endif
    }

    numpunct_byname<wchar_t>::char_type numpunct_byname<wchar_t>::do_thousands_sep() const {
        const char narrow_result = *get_lconv()->thousands_sep;
        auto locale = get_locale();
#if YILIB_HAS_XLOCALE
        return ::btowc_l(narrow_result, locale);
#else
        return std::btowc(narrow_result);
#endif
    }

    string numpunct_byname<wchar_t>::do_grouping() const {
        return get_lconv()->grouping;
    }

    numpunct_byname<wchar_t>::string_type numpunct_byname<wchar_t>::do_truename() const {
        return L"true";
    }

    numpunct_byname<wchar_t>::string_type numpunct_byname<wchar_t>::do_falsename() const {
        return L"false";
    }
}
