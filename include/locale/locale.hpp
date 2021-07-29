#pragma once

#include "cstddef.hpp"
#include "cstring.hpp"
#include "typeinfo.hpp"
#include "stdexcept.hpp"
#include "memory.hpp"

namespace std {
    /* 28.3.1 locale */
    struct locale {
    public:
        class facet {
        private:
            unsigned long refs;

        protected:
            explicit facet(std::size_t refs = 0);
            virtual ~facet() = default;
            facet(const facet&) = delete;
            void operator=(const facet&) = delete;

        public:
            unsigned long increment_ref() noexcept;
            unsigned long decrement_ref() noexcept;
        };

        class id {
        public:
            std::size_t n;
            static std::size_t last_assigned_n;

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
            std::strcpy(n.get(), "*");
            if (!f) {
                return;
            } else if (Facet::id.n >= facets_arr_size) {
                locale::facet** new_facets_arr = new locale::facet*[Facet::id.n + 1]();
                std::memcpy(new_facets_arr, facets, sizeof(locale::facet*) * facets_arr_size);
                delete[] facets;
                facets = new_facets_arr;
                facets_arr_size = Facet::id.n + 1;
            }

            if (facets[Facet::id.n]) {
                facets[Facet::id.n]->decrement_ref();
            }
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
        locale(locale::facet** facets, std::size_t facets_arr_size, const char* name = "C");

        unique_ptr<char[]> n;
        locale::facet** facets;
        std::size_t facets_arr_size;

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
}