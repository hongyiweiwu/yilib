#include "functional.hpp"
#include "cstddef.hpp"
#include "cstdint.hpp"

namespace std {
    std::size_t hash<__internal::__enabled_hash_t>::hash_bytes(const void* ptr, std::size_t len, std::size_t seed) const noexcept {
        if constexpr (sizeof(std::size_t) == 4) { // MurmurHash3, 32-bit.
            std::uint32_t hash = seed;

            const char* const begin = static_cast<const char*>(ptr);
            const char* it = begin;
            const std::size_t n_fourbytes = len / 4;
            for (std::size_t i = 0; i < n_fourbytes; i++, it += 4) {
                std::uint32_t k = *reinterpret_cast<const std::uint32_t*>(it);
                
                k *= 0xcc9e2d51;
                k = (k << 15) | (k >> 17);
                k *= 0x1b873593;

                hash ^= k;
                hash = (hash << 13) | (hash >> 19);
                hash = (hash * 5) + 0xe6546b64;
            }

            if (it != begin + len) {
                unsigned char tmp[4] = {0};
                for (std::size_t i = 0; it + i < begin + len; i++) {
                    tmp[i] = it[i];
                }

                std::uint32_t remaining_bytes = *reinterpret_cast<std::uint32_t*>(tmp);
                remaining_bytes *= 0xcc9e2d51;
                remaining_bytes = (remaining_bytes << 15) | (remaining_bytes >> 17);
                remaining_bytes *= 0x1b873593;

                hash ^= remaining_bytes;
            }

            hash ^= len;

            hash ^= (hash >> 16);
            hash *= 0x85ebca6b;
            hash ^= (hash >> 13);
            hash *= 0xc2b2ae35;
            hash ^= (hash >> 16);
            return hash;
        } else { // MurmurHash2, 64A
            std::uint64_t hash = seed ^ (len * 0xc6a4a7935bd1e995);

            const char* const begin = static_cast<const char*>(ptr);
            const char* it = begin;
            const std::size_t n_eightbytes = len / 8;
            for (std::size_t i = 0; i < n_eightbytes; i++, it += 8) {
                std::uint64_t k = *reinterpret_cast<const std::uint64_t*>(it);

                k *= 0xc6a4a7935bd1e995;
                k ^= (k >> 47);
                k *= 0xc6a4a7935bd1e995;

                hash ^= k;
                hash *= 0xc6a4a7935bd1e995;
            }

            if (it != begin + len) {
                unsigned char tmp[8] = {0};
                for (std::size_t i = 0; it + i < begin + len; i++) {
                    tmp[i] = it[i];
                }

                std::uint64_t remaining_bytes = *reinterpret_cast<std::uint64_t*>(tmp);
                hash ^= remaining_bytes;
                hash *= 0xc6a4a7935bd1e995;
            }

            hash ^= hash >> 47;
            hash *= 0xc6a4a7935bd1e995;
            hash ^= hash >> 47;

            return hash;
        }
    }

    std::size_t hash<bool>::operator()(const bool& key) const noexcept {
        return static_cast<std::size_t>(key);
    }

    std::size_t hash<char>::operator()(const char& key) const noexcept {
        return static_cast<std::size_t>(key);
    }

    std::size_t hash<signed char>::operator()(const signed char& key) const noexcept {
        return static_cast<std::size_t>(key);
    }

    std::size_t hash<unsigned char>::operator()(const unsigned char& key) const noexcept {
        return static_cast<std::size_t>(key);
    }

    std::size_t hash<char8_t>::operator()(const char8_t& key) const noexcept {
        return static_cast<std::size_t>(key);
    }

    std::size_t hash<char16_t>::operator()(const char16_t& key) const noexcept {
        return static_cast<std::size_t>(key);
    }

    std::size_t hash<char32_t>::operator()(const char32_t& key) const noexcept {
        return static_cast<std::size_t>(key);
    }

    std::size_t hash<wchar_t>::operator()(const wchar_t& key) const noexcept {
        return static_cast<std::size_t>(key);
    }

    std::size_t hash<short>::operator()(const short& key) const noexcept {
        return static_cast<std::size_t>(key);
    }

    std::size_t hash<unsigned short>::operator()(const unsigned short& key) const noexcept {
        return static_cast<std::size_t>(key);
    }

    std::size_t hash<int>::operator()(const int& key) const noexcept {
        return static_cast<std::size_t>(key);
    }

    std::size_t hash<unsigned int>::operator()(const unsigned int& key) const noexcept {
        return static_cast<std::size_t>(key);
    }

    std::size_t hash<long>::operator()(const long& key) const noexcept {
        return static_cast<std::size_t>(key);
    }

    std::size_t hash<long long>::operator()(const long long& key) const noexcept {
        return static_cast<std::size_t>(key);
    }

    std::size_t hash<unsigned long>::operator()(const unsigned long& key) const noexcept {
        return static_cast<std::size_t>(key);
    }

    std::size_t hash<unsigned long long>::operator()(const unsigned long long& key) const noexcept {
        return static_cast<std::size_t>(key);
    }

    std::size_t hash<float>::operator()(const float& key) const noexcept {
        return hash_bytes(&key, sizeof(float));
    }

    std::size_t hash<double>::operator()(const double& key) const noexcept {
        return hash_bytes(&key, sizeof(double));
    }

    std::size_t hash<long double>::operator()(const long double& key) const noexcept {
        return hash_bytes(&key, sizeof(long double));
    }

    std::size_t hash<std::nullptr_t>::operator()(const std::nullptr_t&) const noexcept {
        return 0;
    }
}