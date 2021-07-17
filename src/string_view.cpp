#include "string_view.hpp"

namespace std {
    std::size_t hash<string_view>::operator()(const string_view& key) const noexcept {
        return hash_bytes(key.data(), key.size() + 1);
    }

    std::size_t hash<u8string_view>::operator()(const u8string_view& key) const noexcept {
        return hash_bytes(key.data(), key.size() + 1);
    }

    std::size_t hash<u16string_view>::operator()(const u16string_view& key) const noexcept {
        return hash_bytes(key.data(), 2 * (key.size() + 1));
    }

    std::size_t hash<u32string_view>::operator()(const u32string_view& key) const noexcept {
        return hash_bytes(key.data(), 4 * (key.size() + 1));
    }

    std::size_t hash<wstring_view>::operator()(const wstring_view& key) const noexcept {
        return hash_bytes(key.data(), sizeof(wchar_t) * (key.size() + 1));
    }   
}