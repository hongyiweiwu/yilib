#pragma once

#include "compare.hpp"
#include "iosfwd.hpp"
#include "cstddef.hpp"
#include "iterator.hpp"
#include "memory.hpp"
#include "limits.hpp"
#include "stdexcept.hpp"
#include "functional.hpp"
#include "ranges.hpp"

namespace std {
    /* 21.4.3 Class template basic_string_view */
    template<class charT, class traits = char_traits<charT>>
    class basic_string_view {
    public:
        using traits_type = traits;
        using value_type = charT;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using const_iterator = const_pointer;
        using iterator = const_iterator;
        using const_reverse_iterator = reverse_iterator<const_iterator>;
        using reverse_iterator = const_reverse_iterator;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        
        static constexpr size_type npos = size_type(-1);

        constexpr basic_string_view() noexcept : d(nullptr), s(0) {}

        constexpr basic_string_view(const basic_string_view&) noexcept = default;

        constexpr basic_string_view& operator=(const basic_string_view&) noexcept = default;

        constexpr basic_string_view(const charT* str) : d(str), s(traits::length(str)) {}

        constexpr basic_string_view(const charT* str, size_type len) : d(str), s(len) {}

        template<contiguous_iterator It, sized_sentinel_for<It> End> requires is_same_v<iter_value_t<It>, charT> && is_convertible_v<End, size_type>
        constexpr basic_string_view(It begin, End end) : d(to_address(begin)), s(end - begin) {}

        constexpr const_iterator begin() const noexcept {
            return empty() ? nullptr : d;
        }

        constexpr const_iterator end() const noexcept {
            return begin() + size();
        }

        constexpr const_iterator cbegin() const noexcept {
            return empty() ? nullptr : d;
        }

        constexpr const_iterator cend() const noexcept {
            return begin() + size();
        }

        constexpr const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator(end());
        }

        constexpr const_reverse_iterator rend() const noexcept {
            return const_reverse_iterator(begin());
        }

        constexpr const_reverse_iterator crbegin() const noexcept {
            return const_reverse_iterator(end());
        }

        constexpr const_reverse_iterator crend() const noexcept {
            return const_reverse_iterator(begin());
        }

        constexpr size_type size() const noexcept {
            return s;
        }

        constexpr size_type length() const noexcept {
            return s;
        }

        constexpr size_type max_size() const noexcept {
            return numeric_limits<size_type>::max();
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return size() == 0;
        }

        constexpr const_reference operator[](size_type pos) const {
            return d[pos];
        }

        constexpr const_reference at(size_type pos) const {
            return pos >= size() ? throw out_of_range("Calling std::string_view::at with invalid argument.") : d[pos];
        }

        constexpr const_reference front() const {
            return d[0];
        }

        constexpr const_reference back() const {
            return d[s - 1];
        }

        constexpr const_pointer data() const noexcept {
            return d;
        }

        constexpr void remove_prefix(size_type n) {
            d += n;
            s -= n;
        }

        constexpr void remove_suffix(size_type n) {
            s -= n;
        }

        constexpr void swap(basic_string_view& s) noexcept {
            std::swap(d, s.d);
            std::swap(s, s.s);
        }

        constexpr size_type copy(charT* s, size_type n, size_type pos = 0) const {
            if (pos > size()) {
                throw out_of_range("Calling std::string_view::copy with invalid argument.");
            }

            const size_type rlen = min(n, size() - pos);
            traits::copy(s, data() + pos, rlen);
            return rlen;
        }

        constexpr basic_string_view substr(size_type pos = 0, size_type n = npos) const {
            if (pos > size()) {
                throw out_of_range("Calling std::string_view::substr with invalid argument.");
            }

            const size_type rlen = min(n, size() - pos);
            return basic_string_view(data() + pos, rlen);
        }

        constexpr int compare(basic_string_view s) const noexcept {
            const size_type rlen = min(size(), s.size());
            if (const int result = traits::compare(data(), s.data(), rlen); result != 0) {
                return result;
            } else {
                return size() < s.size() ? -1
                    : size() > s.size() ? 1
                    : 0;
            }
        }

        constexpr int compare(size_type pos1, size_type n1, basic_string_view s) const {
            return substr(pos1, n1).compare(s);
        }

        constexpr int compare(size_type pos1, size_type n1, basic_string_view str, size_type pos2, size_type n2) const {
            return substr(pos1, n1).compare(str.substr(pos2, n2));
        }

        constexpr int compare(const charT* s) const {
            return compare(basic_string_view(s));
        }

        constexpr int compare(size_type pos1, size_type n1, const charT* s) const {
            return substr(pos1, n1).compare(basic_string_view(s));
        }

        constexpr int compare(size_type pos1, size_type n1, const charT* s, size_type n2) const {
            return substr(pos1, n1).compare(basic_string_view(s, n2));
        }

        constexpr bool starts_with(basic_string_view x) const noexcept {
            return substr(0, x.size()) == x;
        }

        constexpr bool starts_with(charT x) const noexcept {
            return !empty() && traits::eq(front(), x);
        }

        constexpr bool starts_with(const charT* x) const {
            return starts_with(basic_string_view(x));
        }

        constexpr bool ends_with(basic_string_view x) const noexcept {
            return size() >= x.size() && compare(size() - x.size(), npos, x) == 0;
        }

        constexpr bool ends_with(charT x) const noexcept {
            return !empty() && traits::eq(back(), x);
        }

        constexpr bool ends_with(const charT* x) const {
            return ends_with(basic_string_view(x));
        }

        constexpr size_type find(basic_string_view s, size_type pos = 0) const noexcept {
            for (std::size_t i = pos; i <= size() - s.size(); i++) {
                if (traits::compare(s.data(), data()[i], s.size()) == 0) {
                    return i;
                }
            }

            return npos;
        }

        constexpr size_type find(charT c, size_type pos = 0) const noexcept {
            return find(basic_string_view(addressof(c), 1), pos);
        }

        constexpr size_type find(const charT* s, size_type pos, size_type n) const {
            return find(basic_string_view(s, n), pos);
        }

        constexpr size_type find(const charT* s, size_type pos = 0) const {
            return find(basic_string_view(s), pos);
        }

        constexpr size_type rfind(basic_string_view s, size_type pos = npos) const noexcept {
            for (std::size_t i = size() - s.size(); i >= pos; i--) {
                if (traits::compare(s.data(), data()[i], s.size()) == 0) {
                    return i;
                }
            }

            return npos;
        }

        constexpr size_type rfind(charT c, size_type pos = npos) const noexcept {
            return rfind(basic_string_view(addressof(c), 1), pos);
        }

        constexpr size_type rfind(const charT* s, size_type pos, size_type n) const {
            return rfind(basic_string_view(s, n), pos);
        }

        constexpr size_type rfind(const charT* s, size_type pos = npos) const {
            return rfind(basic_string_view(s), pos);
        }

        constexpr size_type find_first_of(basic_string_view s, size_type pos = 0) const noexcept {
            for (; pos < length(); pos++) {
                if (traits::find(s.data(), s.size(), data()[pos]) != nullptr) {
                    return pos;
                }
            }
            return npos;
        }

        constexpr size_type find_first_of(charT c, size_type pos = 0) const noexcept {
            return find_first_of(basic_string_view(addressof(c), 1), pos);
        }

        constexpr size_type find_first_of(const charT* s, size_type pos, size_type n) const {
            return find_first_of(basic_string_view(s, n), pos);
        }

        constexpr size_type find_first_of(const charT* s, size_type pos = 0) const {
            return find_first_of(basic_string_view(s), pos);
        }

        constexpr size_type find_last_of(basic_string_view s, size_type pos = npos) const noexcept {
            for (size_type i = min(pos, size() - 1); i >= 0; i--) {
                if (traits::find(s.data(), s.size(), data()[pos]) != nullptr) {
                    return i;
                }
            }
            return npos;
        }

        constexpr size_type find_last_of(charT c, size_type pos = npos) const noexcept {
            return find_last_of(basic_string_view(addressof(c), 1), pos);
        }

        constexpr size_type find_last_of(const charT* s, size_type pos, size_type n) const {
            return find_last_of(basic_string_view(s, n), pos);
        }

        constexpr size_type find_last_of(const charT* s, size_type pos = npos) const {
            return find_last_of(basic_string_view(s), pos);
        }

        constexpr size_type find_first_not_of(basic_string_view s, size_type pos = 0) const noexcept {
            for (; pos < length(); pos++) {
                if (traits::find(s.data(), s.size(), data()[pos]) == nullptr) {
                    return pos;
                }
            }
            return npos;
        }

        constexpr size_type find_first_not_of(charT c, size_type pos = 0) const noexcept {
            return find_first_not_of(basic_string_view(addressof(c), 1), pos);
        }

        constexpr size_type find_first_not_of(const charT* s, size_type pos, size_type n) const {
            return find_first_not_of(basic_string_view(s, n), pos);
        }

        constexpr size_type find_first_not_of(const charT* s, size_type pos = 0) const {
            return find_first_not_of(basic_string_view(s), pos);
        }

        constexpr size_type find_last_not_of(basic_string_view s, size_type pos = npos) const noexcept {
            for (size_type i = min(pos, size() - 1); i >= 0; i--) {
                if (traits::find(s.data(), s.size(), data()[pos]) == nullptr) {
                    return i;
                }
            }
            return npos;
        }

        constexpr size_type find_last_not_of(charT c, size_type pos = npos) const noexcept {
            return find_last_not_of(basic_string_view(addressof(c), 1), pos);
        }

        constexpr size_type find_last_not_of(const charT* s, size_type pos, size_type n) const {
            return find_last_not_of(basic_string_view(s, n), pos);
        }

        constexpr size_type find_last_not_of(const charT* s, size_type pos = npos) const {
            return find_last_not_of(basic_string_view(s), pos);
        }

    private:
        const_pointer d;
        size_type s;
    };

    template<class It, class End>
    basic_string_view(It, End) -> basic_string_view<iter_value_t<It>>;

    template<class charT, class traits>
    inline constexpr bool ranges::enable_view<basic_string_view<charT, traits>> = true;
    template<class charT, class traits>
    inline constexpr bool ranges::enable_borrowed_range<basic_string_view<charT, traits>> = true;

    /* 21.4.5 Non-member comparison functions */
    template<class charT, class traits>
    constexpr bool operator==(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) noexcept {
        return x.compare(y) == 0;
    }

    template<class charT, class traits>
    constexpr bool operator==(type_identity_t<basic_string_view<charT, traits>> x, basic_string_view<charT, traits> y) noexcept {
        return x.compare(y) == 0;
    }

    template<class charT, class traits>
    constexpr bool operator==(basic_string_view<charT, traits> x, type_identity_t<basic_string_view<charT, traits>> y) noexcept {
        return x.compare(y) == 0;
    }

    template<class charT, class traits>
    constexpr bool operator!=(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) noexcept {
        return x.compare(y) != 0;
    }

    template<class charT, class traits>
    constexpr bool operator!=(type_identity_t<basic_string_view<charT, traits>> x, basic_string_view<charT, traits> y) noexcept {
        return x.compare(y) != 0;
    }

    template<class charT, class traits>
    constexpr bool operator!=(basic_string_view<charT, traits> x, type_identity_t<basic_string_view<charT, traits>> y) noexcept {
        return x.compare(y) != 0;
    }

    template<class charT, class traits>
    constexpr bool operator<(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) noexcept {
        return x.compare(y) < 0;
    }

    template<class charT, class traits>
    constexpr bool operator<(type_identity_t<basic_string_view<charT, traits>> x, basic_string_view<charT, traits> y) noexcept {
        return x.compare(y) < 0;
    }

    template<class charT, class traits>
    constexpr bool operator<(basic_string_view<charT, traits> x, type_identity_t<basic_string_view<charT, traits>> y) noexcept {
        return x.compare(y) < 0;
    }

    template<class charT, class traits>
    constexpr bool operator<=(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) noexcept {
        return x.compare(y) <= 0;
    }

    template<class charT, class traits>
    constexpr bool operator<=(type_identity_t<basic_string_view<charT, traits>> x, basic_string_view<charT, traits> y) noexcept {
        return x.compare(y) <= 0;
    }

    template<class charT, class traits>
    constexpr bool operator<=(basic_string_view<charT, traits> x, type_identity_t<basic_string_view<charT, traits>> y) noexcept {
        return x.compare(y) <= 0;
    }

    template<class charT, class traits>
    constexpr bool operator>(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) noexcept {
        return x.compare(y) > 0;
    }

    template<class charT, class traits>
    constexpr bool operator>(type_identity_t<basic_string_view<charT, traits>> x, basic_string_view<charT, traits> y) noexcept {
        return x.compare(y) > 0;
    }

    template<class charT, class traits>
    constexpr bool operator>(basic_string_view<charT, traits> x, type_identity_t<basic_string_view<charT, traits>> y) noexcept {
        return x.compare(y) > 0;
    }

    template<class charT, class traits>
    constexpr bool operator>=(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) noexcept {
        return x.compare(y) >= 0;
    }

    template<class charT, class traits>
    constexpr bool operator>=(type_identity_t<basic_string_view<charT, traits>> x, basic_string_view<charT, traits> y) noexcept {
        return x.compare(y) >= 0;
    }

    template<class charT, class traits>
    constexpr bool operator>=(basic_string_view<charT, traits> x, type_identity_t<basic_string_view<charT, traits>> y) noexcept {
        return x.compare(y) >= 0;
    }

    template<class charT, class traits>
    constexpr auto operator<=>(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) noexcept {
        if constexpr (requires { typename traits::comparison_category; }) {
            return static_cast<typename traits::comparison_category>(x.compare(y) <=> 0);
        } else {
            return static_cast<weak_ordering>(x.compare(y) <=> 0);
        }
    }

    template<class charT, class traits>
    constexpr auto operator<=>(type_identity_t<basic_string_view<charT, traits>> x, basic_string_view<charT, traits> y) noexcept {
        if constexpr (requires { typename traits::comparison_category; }) {
            return static_cast<typename traits::comparison_category>(x.compare(y) <=> 0);
        } else {
            return static_cast<weak_ordering>(x.compare(y) <=> 0);
        }
    }

    template<class charT, class traits>
    constexpr auto operator<=>(basic_string_view<charT, traits> x, type_identity_t<basic_string_view<charT, traits>> y) noexcept {
        if constexpr (requires { typename traits::comparison_category; }) {
            return static_cast<typename traits::comparison_category>(x.compare(y) <=> 0);
        } else {
            return static_cast<weak_ordering>(x.compare(y) <=> 0);
        }
    }

    /* 21.4.6 Inserters and extractors */
    // TODO: Uncomment and implement once iostream is done.
    /*
    template<class charT, class traits>
    basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, basic_string_view<charT, traits> str); */

    using string_view = basic_string_view<char>;
    using u8string_view = basic_string_view<char8_t>;
    using u16string_view = basic_string_view<char16_t>;
    using u32string_view = basic_string_view<char32_t>;
    using wstring_view = basic_string_view<wchar_t>;

    /* 21.4.7 Hash support */
    template<>
    struct hash<string_view> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const string_view& key) const noexcept;
    };

    template<>
    struct hash<u8string_view> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const u8string_view& key) const noexcept;
    };

    template<>
    struct hash<u16string_view> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const u16string_view& key) const noexcept;
    };

    template<>
    struct hash<u32string_view> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const u32string_view& key) const noexcept;
    };

    template<>
    struct hash<wstring_view> : public hash<__internal::__enabled_hash_t> {
        std::size_t operator()(const wstring_view& key) const noexcept;
    };

    inline namespace literals {
        inline namespace string_view_literals {
            /* 21.4.8 Suffix for basic_string_view */
            constexpr string_view operator""sv(const char* str, std::size_t len) noexcept {
                return string_view(str, len);
            }

            constexpr u8string_view operator""sv(const char8_t* str, std::size_t len) noexcept {
                return u8string_view(str, len);
            }

            constexpr u16string_view operator""sv(const char16_t* str, std::size_t len) noexcept {
                return u16string_view(str, len);
            }

            constexpr u32string_view operator""sv(const char32_t* str, std::size_t len) noexcept {
                return u32string_view(str, len);
            }

            constexpr wstring_view operator""sv(const wchar_t* str, std::size_t len) noexcept {
                return wstring_view(str, len);
            }
        }
    }
}