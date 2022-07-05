#pragma once

#include "biguint.h"
#include "codec/base_n.h"
#include <cassert>

namespace hecate::util {
	template <std::unsigned_integral T, int N>
	template <int M>
	requires (M < N)
		Biguint<T, N>::Biguint(const Biguint<T, M>& x) noexcept {
		int i = 0;

		for (; i < M; ++i)
			m_Digits[i] = x.m_Digits[i];

		for (; i < N; ++i)
			m_Digits[i] = 0;
	}

	template <std::unsigned_integral T, int N>
	template <util::c_Integral U>
	Biguint<T, N>::Biguint(U value)
		noexcept
	{
		if constexpr (sizeof(U) > sizeof(T)) {
			// ie Biguint<uint8_t, 8>(uint64_t some_value)			
			for (int i = 0; i < m_NumDigits; ++i)
				m_Digits[i] = static_cast<T>(value >> i * m_BitsPerDigit);

			// do we want to warn about truncation?
		}

		if constexpr (sizeof(U) <= sizeof(T)) {
			// ie Biguint<uint64_t, 2>(int32_t some_value)

			m_Digits[0] = static_cast<T>(value);

			for (int i = 1; i < m_NumDigits; ++i)
				m_Digits[i] = 0;
		}
	}

	template <std::unsigned_integral T, int N>
	Biguint<T, N>::Biguint(std::string_view sv, int base) noexcept :
		Biguint(0)
	{
		// first couple of common alphabets (base 2/8/10/16) are reasonably compatible
		// so we can try and use the base16 interpreter per single character
		assert(
			(base == 2) ||
			(base == 8) ||
			(base == 10) ||
			(base == 16)
		);

		for (auto i = 0; i < sv.size(); ++i) {
			auto piece = codec::Base16::int_from_char(sv[i]);
			(*this) *= base;no
			(*this) += piece;
		}
	}

	template <std::unsigned_integral T, int N>
	std::string Biguint<T, N>::to_string() const noexcept {
		std::string result;

		if (*this == 0) 
			result = "0";
		else {


			std::reverse(
				std::begin(result), 
				std::end(result)
			);
		}

		return result;
	}

	template <std::unsigned_integral T, int N>
	template <int M>
	requires (M < N)
		Biguint<T, N>& Biguint<T, N>::operator = (const Biguint<T, M>& x) noexcept {
		int i = 0;

		for (; i < M; ++i)
			m_Digits[i] = x.m_Digits[i];

		for (; i < N; ++i)
			m_Digits[i] = 0;

		return *this;
	}

	template <std::unsigned_integral T, int N>
	template <util::c_Integral U>
	Biguint<T, N>& Biguint<T, N>::operator = (U value)
		noexcept
	{
		if constexpr (sizeof(U) > sizeof(T)) {
			// ie Biguint<uint32_t, 2>(uint64_t some_value)			
			for (int i = 0; i < m_NumDigits; ++i)
				m_Digits[i] = static_cast<T>(value >> i * m_BitsPerDigit);

			// do we want to warn about truncation?
		}

		if constexpr (sizeof(U) <= sizeof(T)) {
			// ie Biguint<uint64_t, 2>(int32_t some_value)

			m_Digits[0] = static_cast<T>(value);

			for (int i = 1; i < m_NumDigits; ++i)
				m_Digits[i] = 0;
		}

		return *this;
	}

	template <std::unsigned_integral T, int N>
	Biguint<T, N>::operator bool() const noexcept {
		for (auto digit : m_Digits)
			if (digit)
				return true;

		return false;
	}

	template <std::unsigned_integral T, int N>
	bool Biguint<T, N>::operator == (const Biguint& rhs) const noexcept {
		return ((*this) <=> rhs) == std::strong_ordering::equal; // mweh
	}

	template <std::unsigned_integral T, int N>
	std::strong_ordering Biguint<T, N>::operator <=> (const Biguint& rhs) const noexcept {
		for (int i = N - 1; i >= 0; --i) {
			auto digit_a = m_Digits[i];
			auto digit_b = rhs.m_Digits[i];

			if (digit_a < digit_b)
				return std::strong_ordering::less;

			if (digit_a > digit_b)
				return std::strong_ordering::greater;
		}

		return std::strong_ordering::equal;
	}

	template <std::unsigned_integral T, int N>
	Biguint<T, N>& Biguint<T, N>::operator += (const Biguint& rhs) noexcept {
		Biguint result = 0;



		return result;
	}

	template <std::unsigned_integral T, int N>
	Biguint<T, N>& Biguint<T, N>::operator -= (const Biguint& rhs) noexcept {
		return {};
	}

	template <std::unsigned_integral T, int N>
	Biguint<T, N>& Biguint<T, N>::operator *= (const Biguint& rhs) noexcept {
		return {};
	}

	template <std::unsigned_integral T, int N>
	Biguint<T, N>& Biguint<T, N>::operator /= (const Biguint& rhs) noexcept {
		return {};
	}

	template <std::unsigned_integral T, int N>
	Biguint<T, N> Biguint<T, N>::operator + (const Biguint& rhs) const noexcept {
		return {};
	}

	template <std::unsigned_integral T, int N>
	Biguint<T, N> Biguint<T, N>::operator - (const Biguint& rhs) const noexcept {
		return {};
	}

	template <std::unsigned_integral T, int N>
	Biguint<T, N> Biguint<T, N>::operator * (const Biguint& rhs) const noexcept {
		return {};
	}

	template <std::unsigned_integral T, int N>
	Biguint<T, N> Biguint<T, N>::operator / (const Biguint& rhs) const noexcept {
		return {};
	}

	template <std::unsigned_integral T, int N>
	Biguint<T, N> Biguint<T, N>::add(const Biguint& rhs) const noexcept {
		return {};
	}

	template <std::unsigned_integral T, int N>
	Biguint<T, N> Biguint<T, N>::sub(const Biguint& rhs) const noexcept {
		return {};
	}

	template <std::unsigned_integral T, int N>
	Biguint<T, N> Biguint<T, N>::mul(const Biguint& rhs) const noexcept {
		return {};
	}

	template <std::unsigned_integral T, int N>
	Biguint<T, N>::DivResult Biguint<T, N>::div(const Biguint& rhs) const noexcept {
		DivResult result = {};



		return result;
	}
}