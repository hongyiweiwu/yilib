#pragma once

// NOTE: All implementations in this file are dummy ones for now.

namespace std {
    struct locale {};

    template<class charT> struct ctype {
        ctype() = default;

        charT widen(char c) const { return c; }
        charT narrow(charT c, char) { return c; }
    };

    template<class Facet>
    const Facet& use_facet(const locale&) {
        static const Facet f;
        return f;
    }
}