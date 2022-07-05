#pragma once

#include "memory.h"

#include <cassert>
#include <memory>
#include <vector>
#include <array>

namespace hecate::util {
	template <typename T, typename U>
	void mem_swap(T& a, U& b) noexcept {
		static_assert(sizeof(T) == sizeof(U));
		
		std::array<std::byte, sizeof(T)> buffer;

		memcpy(buffer.data(), &a, sizeof(T));
		memcpy(&a, &b,            sizeof(T));
		memcpy(&b, buffer.data(), sizeof(T));
	}

	template <typename It, typename T>
	T* placement_copy(It src, T* dst) {
		assert(dst != nullptr);

		return new (dst) T(*src);
	}

	template <typename It, typename T>
	void placement_copy(It it, It last, T* dst) {
		assert(it != dst);
		assert(last >= it);

		while (it != last)
			placement_copy(it++, dst++);
	}
}