#include "string_util.h"

#include <bit>

namespace hecate::util {
	[[nodiscard]] constexpr bool is_upper(char c) noexcept {
		return
			(c >= 'A') &&
			(c <= 'Z');
	}

	[[nodiscard]] constexpr bool is_lower(char c) noexcept {
		return
			(c >= 'a') &&
			(c <= 'z');
	}

	[[nodiscard]] constexpr bool is_alpha(char c) noexcept {
		return
			is_upper(c) ||
			is_lower(c);
	}

	[[nodiscard]] constexpr bool is_digit(char c) noexcept {
		return
			(c >= '0') &&
			(c <= '9');
	}

	[[nodiscard]] constexpr bool is_alphanum(char c) noexcept {
		return
			is_alpha(c) ||
			is_digit(c);
	}

	// https://en.cppreference.com/w/cpp/language/escape
	[[nodiscard]] constexpr bool is_linefeed(char c) noexcept {
		return
			(c == '\n') ||	// new line        (should be the most common by far)
			(c == '\r') ||  // carriage return (fairly common in Windows or old Apple plaintext)
			(c == '\f') ||	// form feed       (exceedingly rare, I've never seen this in the wild)
			(c == '\v');	// vertical tab    (exceedingly rare, I've never seen this in the wild)
	}

	[[nodiscard]] constexpr bool is_whitespace(char c) noexcept {
		return
			(c == ' ')  || // space
			(c == '\t') || // tab
			is_linefeed(c);
	}

	[[nodiscard]] constexpr bool is_number_begin(char c) noexcept {
		return
			is_digit(c) ||
			(c == '+')  ||
			(c == '-');
	}

	[[nodiscard]] constexpr bool is_name_begin(char c) noexcept {
		return
			is_alpha(c) ||
			(c == '_')  ||
			(c == '$');
	}

	[[nodiscard]] constexpr bool is_name_next(char c) noexcept {
		return
			is_alphanum(c) ||
			(c == '_')     ||
			(c == '$');
	}

	[[nodiscard]] constexpr bool is_quote(char c) noexcept {
		return
			(c == '\'') ||
			(c == '"')  ||
			(c == '`');
	}

	[[nodiscard]] constexpr bool is_open_bracket(char c) noexcept {
		return
			(c == '(') ||
			(c == '[') ||
			(c == '{');
	}

	[[nodiscard]] constexpr bool is_close_bracket(char c) noexcept {
		return
			(c == ')') ||
			(c == ']') ||
			(c == '}');
	}

	[[nodiscard]] constexpr bool is_operator(char c) noexcept {
		return
			(c == '~')  ||
			(c == '!')  ||
			(c == '@')  ||
			(c == '#')  ||
			(c == '%')  ||
			(c == '^')  ||
			(c == '&')  ||
			(c == '*')  ||
			(c == '-')  ||
			(c == '+')  ||
			(c == '=')  ||
			(c == '|')  ||
			(c == '\\') ||
			(c == ';')  ||
			(c == ':')  ||
			(c == ',')  ||
			(c == '.')  ||
			(c == '/')  ||
			(c == '?');
	}

	[[nodiscard]] std::string to_lower(std::string_view sv) noexcept {
		std::string result;
		result.reserve(sv.size());

		for (const auto c : sv) {
			if (is_upper(c))
				result += (c - 'A' + 'a');
			else
				result += c;
		}

		return result;

	}

	[[nodiscard]] std::string to_upper(std::string_view sv) noexcept {
		std::string result;
		result.reserve(sv.size());

		for (const auto c : sv) {
			if (is_lower(c))
				result += (c - 'a' + 'A');
			else
				result += c;
		}

		return result;
	}

	[[nodiscard]] std::string normalize_linefeeds(std::string_view sv) noexcept {
		std::string result;
		result.reserve(sv.size());

		for (auto c : sv) {
			if (
				(c == '\r') ||
				(c == '\f') ||
				(c == '\v')
			)
				[[unlikely]]
				continue;  
			else
				[[likely]]
				result += c;
		}

		return result;
	}

	[[nodiscard]] std::string concat(
		const std::vector<std::string>& parts,
		std::string_view                separator
	) {
		size_t total = 0;

		for (const auto& part : parts) {
			total += part.size();
			total += separator.size();
		}

		std::string result;
		result.reserve(total);

		for (const auto& part : parts) {
			result.append(part);
			result.append(separator);
		}

		return result;
	}

	[[nodiscard]] std::string concat(
		const std::vector<const char*>& parts,
		std::string_view                separator
	) {
		size_t total = 0;

		for (const auto& part : parts) {
			total += strlen(part); // eww
			total += separator.size();
		}
		
		std::string result;
		result.reserve(total);

		for (const auto& part : parts) {
			result.append(part);
			result.append(separator);
		}

		return result;
	}

	[[nodiscard]] std::string concat(
		const std::vector<std::string_view>& parts,
		std::string_view separator
	) {
		size_t total = 0;

		for (const auto& part : parts) {
			total += part.size();
			total += separator.size();
		}

		std::string result;
		result.reserve(total);

		for (const auto& part : parts) {
			result.append(part);
			result.append(separator);
		}

		return result;
	}

	std::string columnize(
		const std::string& source_string,
		const int          column_width,
		const char         separator
	) {
		std::string result;

		size_t cursor = 0;

		for (const auto& character : source_string) {
			result.push_back(character);

			if (++cursor >= column_width) {
				cursor = 0;
				result.push_back(separator);
			}
		}

		return result;
	}

	namespace traits {
		constexpr bool ByteChar::eq_int_type(
			int_type  a, 
			int_type  b
		) noexcept {
			return (a == b);
		}

		constexpr ByteChar::char_type ByteChar::to_char_type(int_type  c) noexcept {
			return static_cast<char_type>(c);
		}

		constexpr ByteChar::int_type  ByteChar::to_int_type(char_type c) noexcept {
			return static_cast<int_type>(c);
		}

		constexpr ByteChar::int_type ByteChar::eof() noexcept {
			return 0x0100;
		}

		constexpr ByteChar::int_type ByteChar::not_eof(int_type i) noexcept {
			if (eq_int_type(i, eof()))
				return 0;
			else
				return i;
		}

		ByteChar::char_type* ByteChar::assign(
			char_type* buffer, 
			size_t     count, 
			char_type  value
		) {
			return std::bit_cast<char_type*>(std::memset(buffer, static_cast<int>(value), count));
		}

		ByteChar::char_type* ByteChar::move(
			char_type*       dst, 
			const char_type* src, 
			size_t           count
		) {
			return std::bit_cast<char_type*>(std::memmove(dst, src, count));
		}

		ByteChar::char_type* ByteChar::copy(
			char_type*       dst, 
			const char_type* src, 
			size_t           count
		) {
			return std::bit_cast<char_type*>(std::memcpy(dst, src, count));
		}

		size_t ByteChar::length(const char_type* buffer) {
			return std::strlen(std::bit_cast<const char*>(buffer));
		}

		int ByteChar::compare(
			const char_type* a, 
			const char_type* b, 
			size_t           count
		) {
			return std::memcmp(a, b, count);
		}

		const ByteChar::char_type* ByteChar::find(
			const char_type* haystack,
			size_t           count,
			const char_type& needle
		) {
			return std::bit_cast<const char_type*>(std::memchr(
				haystack,
				static_cast<uint8_t>(needle),
				count
			));
		}
	}

	ByteString toByteString(const std::string& src) noexcept {
		return ByteString(
			std::bit_cast<std::byte*>(src.data()), 
			src.size()
		);
	}
}