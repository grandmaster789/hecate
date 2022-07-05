#pragma once

#include "concepts.h"

namespace hecate::util {
	template <c_Unsigned T>      [[nodiscard]] T byte_swap(T value) noexcept;
	template <c_Signed T>        [[nodiscard]] T byte_swap(T value) noexcept;
	template <c_FloatingPoint T> [[nodiscard]] T byte_swap(T value) noexcept;
}

#include "endian.inl"