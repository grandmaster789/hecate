#pragma once

#include <string>
#include <string_view>

namespace hecate::util {

	// https://en.wikipedia.org/wiki/UTF-8
	// https://en.wikipedia.org/wiki/UTF-16
	// https://en.wikipedia.org/wiki/UTF-32

	[[nodiscard]] std::string to_string(std::u8string_view  sv) noexcept;
	[[nodiscard]] std::string to_string(std::u16string_view sv) noexcept;
	[[nodiscard]] std::string to_string(std::u32string_view sv) noexcept;
	[[nodiscard]] std::string to_string(std::wstring_view   sv) noexcept;

	[[nodiscard]] std::u8string to_u8string(std::string_view    sv) noexcept;
	[[nodiscard]] std::u8string to_u8string(std::u16string_view sv) noexcept;
	[[nodiscard]] std::u8string to_u8string(std::u32string_view sv) noexcept;
	[[nodiscard]] std::u8string to_u8string(std::wstring_view   sv) noexcept;

	[[nodiscard]] std::u16string to_u16string(std::string_view    sv) noexcept;
	[[nodiscard]] std::u16string to_u16string(std::u8string_view  sv) noexcept;
	[[nodiscard]] std::u16string to_u16string(std::u32string_view sv) noexcept;
	[[nodiscard]] std::u16string to_u16string(std::wstring_view   sv) noexcept;

	[[nodiscard]] std::u32string to_u32string(std::string_view    sv) noexcept;
	[[nodiscard]] std::u32string to_u32string(std::u8string_view  sv) noexcept;
	[[nodiscard]] std::u32string to_u32string(std::u16string_view sv) noexcept;
	[[nodiscard]] std::u32string to_u32string(std::wstring_view   sv) noexcept;

	[[nodiscard]] std::wstring to_wstring(std::string_view    sv) noexcept;
	[[nodiscard]] std::wstring to_wstring(std::u8string_view  sv) noexcept;
	[[nodiscard]] std::wstring to_wstring(std::u16string_view sv) noexcept;
	[[nodiscard]] std::wstring to_wstring(std::u32string_view sv) noexcept;

	[[nodiscard]] std::u32string sanitize_u32string(std::u32string&& str) noexcept;
	[[nodiscard]] std::u16string sanitize_u16string(std::u16string&& str) noexcept;
	[[nodiscard]] std::u8string  sanitize_u8string (std::u8string&&  str) noexcept;

	// single-character transformation methods (using iterators to traverse underlying data as needed)
	namespace detail {
		template <typename It> [[nodiscard]] constexpr char32_t utf8_to_utf32 (It& it) noexcept;
		template <typename It> [[nodiscard]] constexpr char32_t utf16_to_utf32(It& it) noexcept;
	
		template <typename It>               
		constexpr bool utf8_to_utf32 (It& it, It last, char32_t& codepoint) noexcept; // returns true if the sequence was valid

		template <typename It> 
		constexpr bool utf16_to_utf32(It& it, It last, char32_t& codepoint) noexcept; // returns true if the sequence was valid
	
		template <typename It> constexpr void utf32_to_utf16(char32_t codepoint, It& backinserter) noexcept;
		template <typename It> constexpr void utf32_to_utf8 (char32_t codepoint, It& backinserter) noexcept;
	}
}

#include "utf.inl"