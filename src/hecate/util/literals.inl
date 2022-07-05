#pragma once

#include "literals.h"

namespace hecate {
	constexpr ssize_t operator "" _z(unsigned long long value) noexcept {
		return static_cast<ssize_t>(value);
	}

	constexpr size_t  operator "" _uz(unsigned long long value) noexcept {
		return static_cast<size_t>(value);
	}
}