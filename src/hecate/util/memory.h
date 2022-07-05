#pragma once

namespace hecate::util {
	template <typename T, typename U>
	void mem_swap(T& a, U& b) noexcept;

	template <typename It, typename T>
	T* placement_copy(It src, T* dst);

	template <typename It, typename T>
	void placement_copy(It first, It last, T* dst);
}

#include "memory.inl"