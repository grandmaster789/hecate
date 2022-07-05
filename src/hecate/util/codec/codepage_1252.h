#pragma once

#include <cstddef>

namespace hecate::util {
	// https://en.wikipedia.org/wiki/Windows-1252
	//
	inline constexpr char32_t codepage1252_to_utf32(char c) noexcept;
}

#include "codepage_1252.inl"