#pragma once

#include <string>
#include <string_view>
#include <cctype>

namespace hecate::util {
	constexpr bool is_space(char32_t codepoint);
	constexpr bool is_space(const char* utf8);

	constexpr bool is_newline(char32_t codepoint);
	constexpr bool is_newline(const char* utf8);

	constexpr bool is_punctuation(char32_t codepoint);
	constexpr bool is_punctuation(const char* utf8);

	constexpr std::string    codepoint_to_utf8(char32_t codepoint);
	constexpr char32_t       decode_utf8(char32_t& state, char32_t& codepoint, char byte);
	constexpr const char*    next_utf8(const char* utf8, const char* last);
	constexpr const char*    prev_utf8(const char* utf8, const char* first);
	constexpr char32_t       codepoint(const char*& utf8);
	constexpr std::string    codepoint_to_utf8(char32_t codepoint);
	constexpr std::string    to_utf8(std::u32string_view utf32);
	constexpr std::u32string to_utf32(std::string_view sv);
	constexpr bool           is_valid_utf8(std::string_view sv);
}

#include "utf8.inl"