#pragma once

#include "cast.h"
#include <cassert>
#include <numeric>

namespace hecate {
	template <util::c_Arithmetic T, util::c_Arithmetic U>
	constexpr T narrow_cast(U value) noexcept {
		static constexpr auto minimum = std::numeric_limits<T>::lowest();
		static constexpr auto maximum = std::numeric_limits<T>::max();

		assert(value >= minimum);
		assert(value <= maximum);

		return static_cast<T>(value);
	}
}