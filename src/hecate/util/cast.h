#pragma once

#include "concepts.h"

// narrow_cast in debug checks for truncation and is equivalent to static_cast
// NOTE casting occurs often enough to put it in the project namespace
//
namespace hecate {
	template <util::c_Arithmetic T, util::c_Arithmetic U>	
	constexpr T narrow_cast(U value) noexcept; // narrow_cast<int>(long)
}

#include "cast.inl"