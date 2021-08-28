#pragma once

#include "cstddef.hpp"
#include "limits.hpp"
#include "type_traits.hpp"
#include "array.hpp"
#include "iterator.hpp"
#include "ranges.hpp"
#include "memory.hpp"

namespace std {
    inline constexpr std::size_t dynamic_extent = numeric_limits<std::size_t>::max();

    /* 22.7.3 Class template span */
    template<class ElementType, std::size_t Extent = dynamic_extent>
    requires __internal::is_complete<ElementType>::value && (!is_abstract_v<ElementType>)
    class span {
    private:
        /* Returns if a given type is a specialization of span or array. */
        template<class T, std::size_t E>
        static consteval bool is_specialization_of_span_or_array(span<T, E>) noexcept {
            return true;
        }

        template<class T, std::size_t N>
        static consteval bool is_specialization_of_span_or_array(array<T, N>) noexcept {
            return true;
        }

        template<class T>
        static consteval bool is_specialization_of_span_or_array(T) noexcept {
            return false;
        }
    public:
        using element_type = ElementType;
        using value_type = remove_cv_t<ElementType>;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = element_type*;
        using const_pointer = const element_type*;
        using reference = element_type&;
        using const_reference = const element_type&;
        using iterator = const_pointer;
        using reverse_iterator = std::reverse_iterator<iterator>;
        static constexpr size_type extent = Extent;

        constexpr span() noexcept
        requires (Extent == dynamic_extent) || (Extent == 0)
            : d(nullptr), s(0) {}

        template<contiguous_iterator It>
        requires is_convertible_v<remove_reference_t<iter_reference_t<It>>(*)[], element_type(*)[]>
        constexpr explicit(extent != dynamic_extent) span(It first, size_type count) noexcept
            : d(to_address(first)), s(count) {}

        template<contiguous_iterator It, sized_sentinel_for<It> End>
        requires is_convertible_v<remove_reference_t<iter_reference_t<It>>(*)[], element_type(*)[]> && (!is_convertible_v<End, std::size_t>)
        constexpr explicit(extent != dynamic_extent) span(It first, End last) noexcept(noexcept(last - first))
            : d(to_address(first)), s(last - first) {}

        template<std::size_t N>
        constexpr span(type_identity_t<element_type> (&arr)[N]) noexcept
        requires (extent == dynamic_extent || extent == N) && is_convertible_v<remove_pointer_t<decltype(data(arr))>(*)[], element_type(*)[]>
            : d(arr), s(N) {}

        template<class T, std::size_t N>
        constexpr span(array<T, N>& arr) noexcept
        requires (extent == dynamic_extent || extent == N) && is_convertible_v<remove_pointer_t<decltype(data(arr))>(*)[], element_type(*)[]>
            : d(arr.data()), s(N) {}

        template<class T, std::size_t N>
        constexpr span(const array<T, N>& arr) noexcept
        requires (extent == dynamic_extent || extent == N) && is_convertible_v<remove_pointer_t<decltype(data(arr))>(*)[], element_type(*)[]>
            : d(arr.data()), s(N) {}

        template<class R>
        requires ranges::contiguous_range<R> && ranges::sized_range<R> && (ranges::borrowed_range<R> || is_const_v<element_type>)
            && (!is_specialization_of_span_or_array<remove_cvref_t<R>>()) && (!is_array_v<remove_cvref_t<R>>)
            && is_convertible_v<remove_reference_t<ranges::range_reference_t<R>>(*)[], element_type(*)[]>
        constexpr explicit(extent != dynamic_extent) span(R&& r) noexcept(noexcept(ranges::data(r), ranges::size(r)))
            : d(ranges::data(r)), s(ranges::size(r)) {}

        constexpr span(const span& other) noexcept = default;

        template<class OtherElementType, std::size_t OtherExtent>
        requires (extent == dynamic_extent || OtherExtent == dynamic_extent || extent == OtherExtent)
            && is_convertible_v<OtherElementType(*)[], element_type(*)[]>
        constexpr explicit(extent != dynamic_extent && OtherExtent == dynamic_extent) span(const span<OtherElementType, OtherExtent>& s) noexcept
            : d(s.data()), s(s.size()) {}

        ~span() noexcept = default;

        constexpr span& operator=(const span& other) noexcept = default;

        template<std::size_t Count>
        requires (Count <= Extent)
        constexpr span<element_type, Count> first() const {
            return {data(), Count};
        }

        template<std::size_t Count>
        requires (Count <= Extent)
        constexpr span<element_type, Count> last() const {
            return {data() + (size() - Count), Count};
        }

        template<std::size_t Offset, std::size_t Count = dynamic_extent>
        requires (Offset <= Extent) && (Count == dynamic_extent || Count <= Extent - Offset)
        constexpr span<element_type, Count != dynamic_extent ? Count : (Extent != dynamic_extent ? Extent - Offset : dynamic_extent)>
        subspan() const {
            return { data() + Offset, Count != dynamic_extent ? Count : size() - Offset };
        }

        constexpr span<element_type, dynamic_extent> first(size_type count) const {
            return { data(), count };
        }

        constexpr span<element_type, dynamic_extent> last(size_type count) const {
            return { data() + (size() - count), count };
        }

        constexpr span<element_type, dynamic_extent> subspan(size_type offset, size_type count = dynamic_extent) const {
            return { data() + offset, count == dynamic_extent ? size() - offset : count };
        }

        constexpr size_type size() const noexcept {
            return s;
        }

        constexpr size_type size_bytes() const noexcept {
            return s * sizeof(element_type);
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return s == 0;
        }

        constexpr reference operator[](size_type idx) const {
            return data()[idx];
        }

        constexpr reference front() const {
            return *data();
        }

        constexpr reference back() const {
            return data()[size() - 1];
        }

        constexpr pointer data() const noexcept {
            return d;
        }

        constexpr iterator begin() const noexcept {
            return data();
        }

        constexpr iterator end() const noexcept {
            return data() + size();
        }

        constexpr reverse_iterator rbegin() const noexcept {
            return reverse_iterator(end());
        }

        constexpr reverse_iterator rend() const noexcept {
            return reverse_iterator(begin());
        }

    private:
        pointer d;
        size_type s;
    };

    template<contiguous_iterator It, class EndOrSize>
    span(It, EndOrSize) -> span<remove_reference_t<iter_reference_t<It>>>;

    template<class T, std::size_t N>
    span(T (&)[N]) -> span<T, N>;

    template<class T, std::size_t N>
    span(array<T, N>&) -> span<T, N>;

    template<class T, std::size_t N>
    span(const array<T, N>&) -> span<const T, N>;

    template<ranges::contiguous_range R>
    span(R&&) -> span<remove_reference_t<ranges::range_reference_t<R>>>;

    template<class ElementType, std::size_t Extent>
    inline constexpr bool ranges::enable_view<span<ElementType, Extent>> = Extent == 0 || Extent == dynamic_extent;

    template<class ElementType, std::size_t Extent>
    inline constexpr bool ranges::enable_borrowed_range<span<ElementType, Extent>> = true;

    /* 22.7.3.8 Views of object representation */
    template<class ElementType, std::size_t Extent>
    span<const byte, Extent == dynamic_extent ? dynamic_extent : sizeof(ElementType) * Extent>
    as_bytes(span<ElementType, Extent> s) noexcept {
        return { reinterpret_cast<const byte*>(s.data()), s.size_bytes() };
    }

    template<class ElementType, std::size_t Extent>
    requires (!is_const_v<ElementType>)
    span<byte, Extent == dynamic_extent ? dynamic_extent : sizeof(ElementType) * Extent>
    as_writable_bytes(span<ElementType, Extent> s) noexcept {
        return { reinterpret_cast<byte*>(s.data()), s.size_bytes() };
    }
}