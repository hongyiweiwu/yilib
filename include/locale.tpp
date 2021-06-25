#pragma once

#include "string.hpp"
#include "locale.hpp"

namespace std {
    template<class charT, class traits, class Allocator>
    bool locale::operator()(const basic_string<charT, traits, Allocator>& s1,
                    const basic_string<charT, traits, Allocator>& s2) const {
        return use_facet<std::collate<charT>>(*this).compare(s1.data(), s1.data() + s1.size(), s2.data(), s2.data() + s2.size()) < 0;
    }

    template<class charT> bool isspace(charT c, const locale& loc) { return use_facet<ctype<charT>>(loc).is(ctype_base::space, c); }
    template<class charT> bool isprint(charT c, const locale& loc) { return use_facet<ctype<charT>>(loc).is(ctype_base::print, c); }
    template<class charT> bool iscntrl(charT c, const locale& loc) { return use_facet<ctype<charT>>(loc).is(ctype_base::cntrl, c); }
    template<class charT> bool isupper(charT c, const locale& loc) { return use_facet<ctype<charT>>(loc).is(ctype_base::upper, c); }
    template<class charT> bool islower(charT c, const locale& loc) { return use_facet<ctype<charT>>(loc).is(ctype_base::lower, c); }
    template<class charT> bool isalpha(charT c, const locale& loc) { return use_facet<ctype<charT>>(loc).is(ctype_base::alpha, c); }
    template<class charT> bool isdigit(charT c, const locale& loc) { return use_facet<ctype<charT>>(loc).is(ctype_base::digit, c); }
    template<class charT> bool ispunct(charT c, const locale& loc) { return use_facet<ctype<charT>>(loc).is(ctype_base::punct, c); }
    template<class charT> bool isxdigit(charT c, const locale& loc) { return use_facet<ctype<charT>>(loc).is(ctype_base::xdigit, c); }
    template<class charT> bool isalnum(charT c, const locale& loc) { return use_facet<ctype<charT>>(loc).is(ctype_base::alnum, c); }
    template<class charT> bool isgraph(charT c, const locale& loc) { return use_facet<ctype<charT>>(loc).is(ctype_base::graph, c); }
    template<class charT> bool isblank(charT c, const locale& loc) { return use_facet<ctype<charT>>(loc).is(ctype_base::blank, c); }
    template<class charT> charT toupper(charT c, const locale& loc) { return use_facet<ctype<charT>>(loc).toupper(c); }
    template<class charT> charT tolower(charT c, const locale& loc) { return use_facet<ctype<charT>>(loc).tolower(c); }
}