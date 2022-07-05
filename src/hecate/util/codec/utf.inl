#pragma once

#include "utf.h"
#include "codepage_1252.h"

#include <cassert>
#include <iterator>
#include <type_traits>

namespace hecate::util::detail {
	template <typename It>
	[[nodiscard]] constexpr char32_t utf8_to_utf32(It& it) noexcept {
		using V = typename std::iterator_traits<It>::value_type;
		static_assert(std::is_same_v<V, char8_t>);

		const char8_t unit = *(it++);

		if (unit <= 0x7F)
			return static_cast<char32_t>(unit);

		char32_t codepoint = 0;

		if (unit <= 0xDF) {
			// expect 1 extra unit
			codepoint = static_cast<char32_t>(unit & 0x1F); // take the relevant 5 bits
			codepoint <<= 6;
			codepoint |= static_cast<char32_t>(*(it++) & 0x3F); // take another 6

			assert(codepoint >= 0x0080);
			assert(codepoint <= 0x07FF);

			return codepoint;
		}

		if (unit <= 0xEF) {
			// expect 2 extra units
			codepoint = static_cast<char32_t>(unit & 0x0F); // take 4 bits
			codepoint <<= 6;
			codepoint |= static_cast<char32_t>(*(it++) & 0x3F); // take another 6
			codepoint <<= 6;
			codepoint |= static_cast<char32_t>(*(it++) & 0x3F); // and another 6

			assert(codepoint >= 0x0800);
			assert(codepoint <= 0xFFFF);

			return codepoint;
		}

		assert(unit <= 0xF7);
		// expect 3 extra units

		codepoint = static_cast<char32_t>(unit & 0x07); // take 3 bits
		codepoint <<= 6;
		codepoint |= static_cast<char32_t>(*(it++) & 0x3F); // take another 6
		codepoint <<= 6;
		codepoint |= static_cast<char32_t>(*(it++) & 0x3F); // and another 6
		codepoint <<= 6;
		codepoint |= static_cast<char32_t>(*(it++) & 0x3F); // and another 6

		assert(codepoint >= 0x10'0000);
		assert(codepoint <= 0x10'FFFF);

		return codepoint;
	}

	template <typename It> 
	[[nodiscard]] constexpr char32_t utf16_to_utf32(It& it) noexcept {
		using V = typename std::iterator_traits<It>::value_type;
		static_assert(std::is_same_v<V, char16_t>);

		const char16_t first = *(it++);

		if (first <= 0xD7FF)
			return first;
		if (first >= 0XE000)
			return first;

		assert(first <= 0xDBFF); // should be high surrogate
		
		const char16_t second = *(it++);
		
		assert(second >= 0xDC00); // should be low surrogate
		assert(second <= 0xDFFF);

		// shift everything into place
		return (
			(static_cast<char32_t>(first  - 0xD800) << 10) | 
			(static_cast<char32_t>(second - 0xDC00))
		) + 0x01'0000;
	}

	template <typename It> 
	constexpr bool utf8_to_utf32(It& it, It last, char32_t& codepoint) noexcept {
		using V = typename std::iterator_traits<It>::value_type;
		static_assert(std::is_same_v<V, char8_t>);

		char8_t unit = *(it++);
		
		int extra_units = 0;

		if (unit <= 0x7F) {
			codepoint = unit;
			return true;
		}

		if (unit <= 0xBF) {
			// invalid continuation character, assume Windows-1252 codepage
			codepoint = codepage1252_to_utf32(static_cast<char>(unit));
			return false;
		}

		if (unit <= 0xDF) {
			codepoint = static_cast<char32_t>(unit & 0x01F);
			extra_units = 1;
		}

		if (unit <= 0xEF) {
			codepoint = static_cast<char32_t>(unit & 0x0F);
			extra_units = 2;
		}

		if (unit <= 0xF7) {
			codepoint = static_cast<char32_t>(unit & 0x07);
			extra_units = 3;
		}

		// at this point, if we haven't returned and still don't know how many 
		// extra units to expect, it's an invalid utf8 sequence
		if (extra_units == 0) {
			codepoint = codepage1252_to_utf32(static_cast<char>(unit));
			return false;
		}

		auto old = it;

		for (int i = 0; i < extra_units; ++i) {
			if (
				(it == last) ||
				((*it & 0xC0) != 0x80)
			) {
				[[unlikely]]
				codepoint = codepage1252_to_utf32(static_cast<char>(unit));
				return false;
			}

			// grab another 6 bits
			codepoint <<= 6;
			codepoint = *(it++) & 0x3F;
		}

		// check and verify that the result makes sense
		if (
			((codepoint >= 0xD800) && (codepoint <= 0xDFFF))   ||
			((extra_units == 1)    && (codepoint < 0x00'0080)) ||
			((extra_units == 2)    && (codepoint < 0x00'0800)) ||
			((extra_units == 3)    && (codepoint < 0x10'0000))
		) {
			codepoint = codepage1252_to_utf32(static_cast<char>(unit));
			it = old;
			return false;
		}

		// passed all checks, should be fine
		return true;
	}

	template <typename It> 
	constexpr bool utf16_to_utf32(It& it, It last, char32_t& codepoint) noexcept {
		
	}

	template <typename It> 
	constexpr void utf32_to_utf16(char32_t codepoint, It& backinserter) noexcept {
		using V = typename It::container_type::value_type;
		static_assert(sizeof(V) == 2);

		if (codepoint <= 0xFFFF)
			*(backinserter++) = static_cast<V>(codepoint);
		else {
			assert(codepoint <= 0x10'FFFF);

			codepoint -= 0x10'0000;

			*(backinserter++) = static_cast<V>(codepoint >> 10)    | 0xD800;
			*(backinserter++) = static_cast<V>(codepoint) & 0x03FF | 0xDC00;
		}
	}

	template <typename It> 
	constexpr void utf32_to_utf8(char32_t codepoint, It& backinserter) noexcept {
		using V = typename It::container_type::value_type;
		static_assert(sizeof(V) == 1);

		if (codepoint <= 0x7F)
			*(backinserter++) = static_cast<V>(codepoint);
		else if (codepoint <= 0x07FF) {
			*(backinserter++) = static_cast<V>(codepoint >> 6)   | 0xC0;
			*(backinserter++) = static_cast<V>(codepoint) & 0x3F | 0x80;
		}
		else if (codepoint <= 0xFFFF) {
			*(backinserter++) = static_cast<V>(codepoint >> 12)        | 0xE0;
			*(backinserter++) = static_cast<V>(codepoint >> 6)  & 0x3F | 0x80;
			*(backinserter++) = static_cast<V>(codepoint)       & 0x3F | 0x80;
		}
		else {
			assert(codepoint <= 0x10'FFFF);

			*(backinserter++) = static_cast<V>(codepoint >> 18)        | 0xF0;
			*(backinserter++) = static_cast<V>(codepoint >> 12) & 0x3F | 0x80;
			*(backinserter++) = static_cast<V>(codepoint >> 6)  & 0x3F | 0x80;
			*(backinserter++) = static_cast<V>(codepoint)       & 0x3F | 0x80;
		}
	}
}