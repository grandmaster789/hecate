#pragma once

#include "endian.h"
#include "../preprocessor.h"

#include <type_traits>
#include <cstdint>
#include <bit>

namespace hecate::util {
	template <c_Unsigned T>
	[[nodiscard]] T byte_swap(T value) noexcept {
		// when possible, use builtins to achieve the swap

#if HECATE_PLATFORM == HECATE_PLATFORM_WINDOWS
		// MSVC: https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/byteswap-uint64-byteswap-ulong-byteswap-ushort

		if constexpr (sizeof(T) == sizeof(uint64_t)) {
			return static_cast<T>(_byteswap_uint64(static_cast<uint64_t>(value)));
		}
		
		if constexpr (sizeof(T) == sizeof(unsigned long)) {
			return static_cast<T>(_byteswap_ulong(static_cast<unsigned long>(value)));
		}
		
		if constexpr (sizeof(T) == sizeof(unsigned short)) {
			return static_cast<T>(_byteswap_ushort(static_cast<unsigned short>(value)));
		}

#elif HECATE_PLATFORM == HECATE_PLATFORM_LINUX
		// Clang/GCC: https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html

		if constexpr (sizeof(T) == sizeof(uint64_t)) {
			return static_cast<T>(__builtin_bswap64(static_cast<uint64_t>(value)));
		}
		
		if constexpr (sizeof(T) == sizeof(uint32_t)) {
			return static_cast<T>(__builtin_bswap32(static_cast<uint32_t>(value)));
		}
		
		if constexpr (sizeof(T) == sizeof(uint16_t)) {
			return static_cast<T>(__builtin_bswap16(static_cast<uint16_t>(value)));
		}
#else
		// fall back to algorithmic approach
		T result;

		const std::byte* src = reinterpret_cast<const std::byte*>(&value);
		std::byte* dst = reinterpret_cast<std::byte*>(&result);

		for (size_t i = 0; i < sizeof(T); ++i)
			dst[i] = src[sizeof(T) - i - 1];

		return result;
#endif
	}

	template <c_Signed T>
	[[nodiscard]] T byte_swap(T value) noexcept {
		return static_cast<T>(
			byte_swap(
				static_cast<std::make_unsigned_t<T>>(value)
			)
		);
	}

	template <c_FloatingPoint T> 
	[[nodiscard]] T byte_swap(T value) noexcept {
		if constexpr (std::is_same_v<T, float>) {
			uint32_t x = std::bit_cast<uint32_t>(value);
			x = byte_swap(x);

			return std::bit_cast<float>(x);
		}
		
		if constexpr (std::is_same_v<T, double>) {
			uint64_t x = std::bit_cast<uint64_t>(value);
			x = byte_swap(x);

			return std::bit_cast<double>(x);
		}

		// do we even want to consider long double?

		return value;
	}
}