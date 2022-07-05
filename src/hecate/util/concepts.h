#pragma once

#include <type_traits>
#include <concepts>
#include <string>
#include <string_view>

namespace hecate::util {
	template <typename T> concept c_Arithmetic    = std::is_arithmetic_v<T>;
	template <typename T> concept c_Integral      = std::is_integral_v<T>;
	template <typename T> concept c_Unsigned      = std::is_unsigned_v<T>;
	template <typename T> concept c_Signed        = std::is_signed_v<T>;
	template <typename T> concept c_FloatingPoint = std::is_floating_point_v<T>;
	template <typename T> concept c_Pointer       = std::is_pointer_v<T>;
	template <typename T> concept c_Reference     = std::is_reference_v<T>;
	template <typename T> concept c_LValueRef     = std::is_lvalue_reference_v<T>;
	template <typename T> concept c_RValueRef     = std::is_rvalue_reference_v<T>;

	template <typename T>
	concept c_ToString = requires(T value) {
		{ to_string(value) } -> std::convertible_to<std::string>;
	};

	/*
	template <typename T>
	concept c_FromString = requires(T value) {
		{ from_string(std::string_view{}) } -> std::convertible_to<T>;
	};
	*/

	template <typename T, typename U>
	concept c_StaticCast = requires(T value) {
		{ static_cast<U>(value) }->std::convertible_to<U>;
	};

	template <typename T>
	concept c_Size = requires(T value) {
		{ size(value) } -> std::convertible_to<size_t>;
	};
}