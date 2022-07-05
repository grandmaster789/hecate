#pragma once

#include "../algorithm.h"
#include "../cast.h"

#include <cassert>
#include <bit>

namespace hecate::util::codec::detail {
	template <size_t N>
	constexpr BaseN_Alphabet::BaseN_Alphabet(
		const char(&text)[N],				      // null terminated string with le alphabet
		bool       is_case_sensitive,
		char       padding_character
	) noexcept:
		m_Radix           (N - 1), // constructed from zero-terminated string...
		m_CaseSensitive   (is_case_sensitive),
		m_PaddingCharacter(padding_character)
	{
		// only specific radixes are supported
		static_assert(
			(N-1 == 2)  ||
			(N-1 == 8)  ||
			(N-1 == 10) ||
			(N-1 == 16) ||
			(N-1 == 32) ||
			(N-1 == 64) ||
			(N-1 == 85)
		);

		// start everything at -2
		fill(m_CharacterToInt, static_cast<int8_t>(-2));

		// whitespace should be marked -1
		m_CharacterToInt[' ']  = -1;
		m_CharacterToInt['\t'] = -1;
		m_CharacterToInt['\r'] = -1;
		m_CharacterToInt['\n'] = -1;
		m_CharacterToInt['\f'] = -1;

		if (padding_character)
			m_CharacterToInt[padding_character] = -1;

		// actually set up the table
		for (size_t i = 0; i < m_Radix; ++i) {
			char c = text[i];

			m_IntToCharacter[i] = c;

#if HECATE_COMPILER == HECATE_COMPILER_MSVC
	// looks like code analysis has some false positives...
	#pragma warning(push)
	#pragma warning(disable: 28020) // expression 0 <= _Param_(1) && _Param_(1) <= 256-1 is not true
#endif

			m_CharacterToInt[c] = static_cast<int8_t>(i);

#if HECATE_COMPILER == HECATE_COMPILER_MSVC
	#pragma warning(pop)
#endif

			// consider case insensitivity
			if (!is_case_sensitive) {
				if (c >= 'a' && c <= 'z') {
					// code analysis was concerned that arithmetic on 32-bit integers would overflow...
					size_t wide_c = static_cast<size_t>(c);
					size_t wide_a = static_cast<size_t>('a');
					size_t wide_A = static_cast<size_t>('A');

					// m_CharacterToInt[(c - 'a') + 'A'] = static_cast<int8_t>(i);
					m_CharacterToInt[(wide_c - wide_a) + wide_A] = static_cast<int8_t>(i);
				}
				else if (c >= 'A' && c <= 'Z') {
					// code analysis was concerned that arithmetic on 32-bit integers would overflow...
					size_t wide_c = static_cast<size_t>(c);
					size_t wide_a = static_cast<size_t>('a');
					size_t wide_A = static_cast<size_t>('A');

					//m_CharacterToInt[(c - 'A') + 'a'] = static_cast<int8_t>(i);
					m_CharacterToInt[(wide_c - wide_A) + wide_a] = static_cast<int8_t>(i);
				}
			}
		}
	}

	inline constexpr char BaseN_Alphabet::from_int(int8_t x) const noexcept {
		assert(x < m_Radix);

		return m_IntToCharacter[x];
	}

	inline constexpr int8_t BaseN_Alphabet::from_char(char c) const noexcept {
		// was getting weird warnings C28020 here?
		//return m_CharacterToInt[c];
		return m_CharacterToInt[std::bit_cast<uint8_t>(c)];
	}
}

namespace hecate::util::codec {
	template <detail::BaseN_Alphabet A, int C, int B>
	constexpr int8_t BaseN<A, C, B>::int_from_char(char c) noexcept {
		return c_Alphabet.from_char(c);
	}

	template <detail::BaseN_Alphabet A, int C, int B>
	constexpr char BaseN<A, C, B>::char_from_int(int8_t x) noexcept {
		return c_Alphabet.from_int(x);
	}

	template <detail::BaseN_Alphabet A, int C, int B>
	template <typename tInputIt, typename tOutputIt>
	constexpr void BaseN<A, C, B>::encode(
		tInputIt  it,
		tInputIt  end,
		tOutputIt out
	) noexcept {
		uint64_t byte_idx = 0;
		uint64_t block    = 0;

		for (; it != end; ++it) {
			const uint64_t shift = 8 * ((c_BytesPerBlock - 1) - byte_idx); // determine the next slot in the block
			
			block |= static_cast<uint64_t>(*it) << shift; // stick the character into the block, shift big-endian style
			++byte_idx;

			if (byte_idx == c_BytesPerBlock) {
				encode_block(block, c_BytesPerBlock, out);

				byte_idx = 0;
				block    = 0;
			}
		}

		if (byte_idx != 0)
			encode_block(block, byte_idx, out);
	}

	template <detail::BaseN_Alphabet A, int C, int B>
	template <typename tInputIt>
	std::string BaseN<A, C, B>::encode(
		tInputIt begin,
		tInputIt end
	) noexcept {
		std::string result;

		encode(
			begin, 
			end, 
			std::back_inserter(result)
		);

		return result;
	}

	template <detail::BaseN_Alphabet A, int C, int B>
	std::string BaseN<A, C, B>::encode(
		std::span<const std::byte> raw_bytes
	) noexcept {
		return encode(
			std::begin(raw_bytes), 
			std::end(raw_bytes)
		);
	}

	template <detail::BaseN_Alphabet A, int C, int B>
	template <typename tInputIt, typename tOutputIt>
	constexpr tInputIt BaseN<A, C, B>::decode(
		tInputIt  it,
		tInputIt  end,
		tOutputIt out
	) noexcept {
		uint64_t char_idx = 0;
		uint64_t block    = 0;

		for (; it != end; ++it) {
			int8_t digit = int_from_char(*it);

			if (digit == -1)
				continue; // skip over whitespace

			if (digit == -2)
				return it; // terminal or unrecognised character

			block *= c_Radix;
			block += digit;

			++char_idx;

			if (char_idx == c_CharsPerBlock) {
				decode_block(block, c_CharsPerBlock, out);

				block    = 0;
				char_idx = 0;
			}
		}

		if (char_idx) {
			// pad the block with zeroes (could also shift)
			for (auto i = char_idx; i < c_CharsPerBlock; ++i)
				block *= c_Radix;

			decode_block(block, c_CharsPerBlock, out); 
		}

		return it;
	}

	template <detail::BaseN_Alphabet A, int C, int B>
	template <typename tOutputIt>
	void BaseN<A, C, B>::encode_block(
		uint64_t  block,
		uint64_t  num_bytes,
		tOutputIt out
	) noexcept {
		const uint64_t padding = c_BytesPerBlock - num_bytes;

		std::string str;

		for (uint64_t i = 0; i < c_CharsPerBlock; ++i) {
			const auto value = block % c_Radix;
			block /= c_Radix;

			if (i < padding) {
				if constexpr (c_PaddingCharacter != '\0')
					str += c_PaddingCharacter;
			}
			else
				str += char_from_int(static_cast<int8_t>(value));
		}

		std::copy(
			std::rbegin(str),
			std::rend(str),
			out
		);
	}

	template <detail::BaseN_Alphabet A, int C, int B>
	template <typename tOutputIt>
	void BaseN<A, C, B>::decode_block(
		uint64_t  block,
		uint64_t  num_chars,
		tOutputIt out
	) noexcept {
		auto padding = c_CharsPerBlock - num_chars;

		for (uint64_t i = 0; i < (c_BytesPerBlock - padding); ++i) {
			uint64_t shift = 8 * ((c_BytesPerBlock - 1) - i);

			*(out++) = static_cast<std::byte>((block >> shift) & 0xFF);
		}
	}
}