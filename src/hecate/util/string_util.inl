#pragma once

#include "string_util.h"
#include <sstream>

namespace hecate::util {
	template <typename... tArgs>
	[[nodiscard]] std::string stringify(tArgs... args) {
		std::stringstream result;

		(result << ... << std::forward<tArgs>(args));

		return result.str();
	}

	inline constexpr size_t string_size(c_Size auto str) noexcept {
		return size(str);
	}

	inline constexpr size_t string_size(auto) noexcept {
		return 1;
	}

	template <typename T, typename... Ts>
	[[nodiscard]] std::pair<size_t, size_t> find_first_string(
		std::string_view haystack,
		size_t           offset,
		const T&         needle_head,
		const Ts&...     needle_tail
	) noexcept {
		size_t first = haystack.find(needle_head, offset); // https://en.cppreference.com/w/cpp/string/basic_string_view/find
		size_t last  = first + string_size(needle_head);

		if (first == std::string_view::npos) {
			first = haystack.size();
			last  = haystack.size();
		}

		if constexpr (sizeof...(Ts) != 0) {
			const auto [x, y] = find_first_string(
				haystack, 
				offset, 
				needle_tail...
			); // tail recursion whoo

			if (x < first) {
				first = x;
				last  = y;
			}
		}

		return { first, last };
	}

	template <typename... Ts>
	[[nodiscard]] std::vector<std::string> split(
		std::string_view haystack,
		const Ts&... needles
	) noexcept {
		std::vector<std::string> result;

		size_t cursor = 0;

		while (cursor < haystack.size()) {
			const auto [x, y] = find_first_string(haystack, cursor, needles...);

			std::string token{ haystack.substr(cursor, x - cursor) };
			cursor = y;

			if (!token.empty())
				result.push_back(std::move(token));
		}

		return result;
	}

	namespace traits {
		inline constexpr void ByteChar::assign(
			char_type& result,
			const char_type& value
		) noexcept {
			result = value;
		}

		inline constexpr bool ByteChar::eq(
			char_type a,
			char_type b
		) noexcept {
			return (static_cast<uint8_t>(a) == static_cast<uint8_t>(b));
		}
	}
}