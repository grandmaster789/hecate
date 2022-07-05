#pragma once

#include "concepts.h"

#include <cstdint>
#include <array>
#include <string_view>
#include <compare>

namespace hecate::util {
	// 
	// 
	// Somewhat based around UUIDs (https://en.wikipedia.org/wiki/Universally_unique_identifier) 
	// (as in 128-bit unsinged integer composed out of 2x uint64_t == Biguint<uint64_t, 2>)
	// (this is also the reason it's in the util namespace)
	template <std::unsigned_integral T, int N>
	class Biguint {
	public:
		using Digit = T;

		Biguint() = default;

		template <int M>
		requires (M < N)
		Biguint(const Biguint<T, M>& x) noexcept;                      // implicit construction from smaller biguint

		template <util::c_Integral U>
		Biguint(U value) noexcept;                                     // implicit construction from native integer types
		
		explicit Biguint(std::string_view sv, int base = 10) noexcept; // explicit construction from string (supports base 2/8/10/16)

		std::string to_string() const noexcept;

		template <int M>
		requires (M < N)
		Biguint& operator = (const Biguint<T, M>& x) noexcept;         // assignment from a smaller biguint

		template <util::c_Integral U>
		Biguint& operator = (U value) noexcept;                        // assign from native integer types

		explicit operator bool() const noexcept;

		bool                 operator ==  (const Biguint& rhs) const noexcept;
		std::strong_ordering operator <=> (const Biguint& rhs) const noexcept;

		Biguint& operator += (const Biguint& rhs) noexcept;
		Biguint& operator -= (const Biguint& rhs) noexcept;
		Biguint& operator *= (const Biguint& rhs) noexcept;
		Biguint& operator /= (const Biguint& rhs) noexcept;

		Biguint operator + (const Biguint& rhs) const noexcept;
		Biguint operator - (const Biguint& rhs) const noexcept;
		Biguint operator * (const Biguint& rhs) const noexcept;
		Biguint operator / (const Biguint& rhs) const noexcept;

	private:
		static_assert(sizeof(int16_t) == 2); // ensure that we have 8 bits per byte

		struct DivResult {
			Biguint m_Quotient;
			Biguint m_Remainder;
		};

		Biguint   add(const Biguint& rhs) const noexcept;
		Biguint   sub(const Biguint& rhs) const noexcept;
		Biguint   mul(const Biguint& rhs) const noexcept;
		DivResult div(const Biguint& rhs) const noexcept;

		static constexpr int m_NumDigits    = N;
		static constexpr int m_BitsPerDigit = sizeof(Digit) * 8;
		static constexpr int m_NumBits      = m_NumDigits * m_BitsPerDigit;

		std::array<Digit, m_NumDigits> m_Digits;
	};

	using ubig128 = Biguint<uint64_t, 2>;
	using uuid    = Biguint<uint64_t, 2>;
}

#include "biguint.inl"