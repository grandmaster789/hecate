#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <utility>

#include "concepts.h"

namespace hecate::util {
	[[nodiscard]] constexpr bool is_upper        (char c) noexcept; // A-Z
	[[nodiscard]] constexpr bool is_lower        (char c) noexcept; // a-z
	[[nodiscard]] constexpr bool is_alpha        (char c) noexcept; // A-Za-z
	[[nodiscard]] constexpr bool is_digit        (char c) noexcept; // 0-9
	[[nodiscard]] constexpr bool is_alphanum     (char c) noexcept; // A-Za-z0-9
	[[nodiscard]] constexpr bool is_linefeed     (char c) noexcept; // \n\r\f\v
	[[nodiscard]] constexpr bool is_whitespace   (char c) noexcept; // ' '\t\n\r\f\v
	[[nodiscard]] constexpr bool is_number_begin (char c) noexcept; // 0-9+-
	[[nodiscard]] constexpr bool is_name_begin   (char c) noexcept; // A-Za-z_$
	[[nodiscard]] constexpr bool is_name_next    (char c) noexcept; // A-Za-z0-9_$
	[[nodiscard]] constexpr bool is_quote        (char c) noexcept; // '"`
	[[nodiscard]] constexpr bool is_open_bracket (char c) noexcept; // ([{
	[[nodiscard]] constexpr bool is_close_bracket(char c) noexcept; // )]}
	[[nodiscard]] constexpr bool is_operator     (char c) noexcept; // ~!@#%^&*<>?/,.:;|\+-=

	[[nodiscard]] std::string to_lower           (std::string_view sv) noexcept;
	[[nodiscard]] std::string to_upper           (std::string_view sv) noexcept;
	[[nodiscard]] std::string normalize_linefeeds(std::string_view sv) noexcept; // get rid of all line feeds except for \n

	[[nodiscard]] std::string concat(const std::vector<std::string>&      parts, std::string_view separator = "");
	[[nodiscard]] std::string concat(const std::vector<const char*>&      parts, std::string_view separator = "");
	[[nodiscard]] std::string concat(const std::vector<std::string_view>& parts, std::string_view separator = "");

	template <typename... tArgs>
	[[nodiscard]] std::string stringify(tArgs... args);

	// insert a separator character every 'column_width' characters
	std::string columnize(
		const std::string& source_string,
		const int          column_width = 80,
		const char         separator = '\n'
	);

	inline constexpr size_t string_size(c_Size auto str) noexcept;
	inline constexpr size_t string_size(      auto str) noexcept;

	// search the haystack for all needles, return the first matched range
	template <typename T, typename... Ts>
	[[nodiscard]] std::pair<size_t, size_t> find_first_string(
		std::string_view haystack,
		size_t           offset,
		const T&         needle_head,
		const Ts&...     needle_tail
	) noexcept;

	template <typename... Ts>
	[[nodiscard]] std::vector<std::string> split(
		std::string_view haystack, 
		const Ts&...     needles
	) noexcept;

	namespace traits {
		// zero-terminated string of bytes
		struct ByteChar {
			using char_type  = std::byte;
			using int_type   = unsigned int;
			using state_type = std::mbstate_t;        // https://en.cppreference.com/w/cpp/string/multibyte/mbstate_t
			using off_type   = std::streamoff;        // offset type
			using pos_type   = std::fpos<state_type>; // position type

			static inline constexpr void assign      (char_type& result, const char_type& value) noexcept;
			static inline constexpr bool eq          (char_type a, char_type b) noexcept;

			static constexpr bool      eq_int_type (int_type  a, int_type  b) noexcept;
			static constexpr char_type to_char_type(int_type  c) noexcept; // may truncate
			static constexpr int_type  to_int_type (char_type c) noexcept; // may truncate

			static constexpr int_type eof    ()           noexcept;
			static constexpr int_type not_eof(int_type i) noexcept;

			static char_type* assign(char_type* buffer, size_t count, char_type value);		// memset
			static char_type* move  (char_type* dst, const char_type* src, size_t count);	// memmove
			static char_type* copy  (char_type* dst, const char_type* src, size_t count);	// memcpy

			static size_t           length (const char_type* buffer);                                          // strlen
			static int              compare(const char_type* a, const char_type* b, size_t count);             // memcmp
			static const char_type* find   (const char_type* haystack, size_t count, const char_type& needle); // memchr
		};
	}

	using ByteString     = std::basic_string<std::byte,      traits::ByteChar>;
	using ByteStringView = std::basic_string_view<std::byte, traits::ByteChar>;

	ByteString toByteString(const std::string& src) noexcept;
}

#include "string_util.inl"