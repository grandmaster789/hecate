#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <string_view>
#include <span>

namespace hecate::util::codec::detail {
	//
	// https://tools.ietf.org/html/rfc4648
	// https://tools.ietf.org/html/rfc1924
	// https://en.wikipedia.org/wiki/Base64
	// https://en.wikipedia.org/wiki/Ascii85
	// https://github.com/git/git/blob/master/base85.c
	// (common implementations are Base64, Base32, Base36, Base58, Base62 and Base85)
	// (Base64 is popular on the web, Base85 rfc1924 is used in git/json)
	// Theoretically this should support encoding from/to up to base127, but concrete
	// alphabets are provided for common usage
	struct BaseN_Alphabet {
		bool     m_CaseSensitive = false;
		uint64_t m_Radix;
		char     m_PaddingCharacter;

		static_assert(sizeof(int8_t) == sizeof(char));

		std::array<int8_t, 256> m_CharacterToInt = {}; // -2 on terminal/invalid characters, -1 on whitespace
		std::array<char, 127>   m_IntToCharacter = {}; 

		template <size_t N>
		constexpr BaseN_Alphabet(
			const char (&text)[N],				   // null terminated string with le alphabet
			bool       is_case_sensitive = false,  // base16, base32, base32hex should treat upper and lower case letters the same
			char       padding_character = '\0'	   // explicit padding characters are really only required for base64
		) noexcept;

		inline constexpr char   from_int (int8_t x) const noexcept;
		inline constexpr int8_t from_char(char c)   const noexcept;
	};
}

namespace hecate::util::codec {
	template <
		detail::BaseN_Alphabet tAlphabet,
		int                    tCharsPerBlock,
		int                    tBytesPerBlock
	>
	class BaseN {
	public:
		static constexpr detail::BaseN_Alphabet c_Alphabet         = tAlphabet;
		static constexpr char                   c_PaddingCharacter = tAlphabet.m_PaddingCharacter;
		static constexpr uint64_t               c_Radix            = tAlphabet.m_Radix;
		static constexpr uint64_t               c_CharsPerBlock    = tCharsPerBlock;
		static constexpr uint64_t               c_BytesPerBlock    = tBytesPerBlock;

		static constexpr int8_t int_from_char(char   c) noexcept;
		static constexpr char   char_from_int(int8_t x) noexcept;

		template <typename tInputIt, typename tOutputIt>
		static constexpr void encode(
			tInputIt  begin, 
			tInputIt  end, 
			tOutputIt out
		) noexcept;

		template <typename tInputIt>
		static std::string encode(
			tInputIt begin, 
			tInputIt end
		) noexcept; // std::string doesn't have constexpr construction yet...

		static std::string encode(
			std::span<const std::byte> raw_bytes
		) noexcept;

		template <typename tInputIt, typename tOutputIt>
		static constexpr tInputIt decode(
			tInputIt  begin,
			tInputIt  end,
			tOutputIt out
		) noexcept;

	private:
		template <typename tOutputIt>
		static void encode_block(
			uint64_t  block,
			uint64_t  num_bytes,
			tOutputIt out
		) noexcept;

		template <typename tOutputIt>
		static void decode_block(
			uint64_t  block,
			uint64_t  num_bytes,
			tOutputIt out
		) noexcept;
	};
}

#include "base_n.inl"

namespace hecate::util::codec {
	constexpr auto c_Base2_alphabet             = detail::BaseN_Alphabet{"01"};
	constexpr auto c_Base8_alphabet             = detail::BaseN_Alphabet{"01234567"};
	constexpr auto c_Base10_alphabet            = detail::BaseN_Alphabet{"0123456789"};
	constexpr auto c_Base16_alphabet            = detail::BaseN_Alphabet{"0123456789ABCDEF"};
	constexpr auto c_Base32_rfc4648_alphabet    = detail::BaseN_Alphabet{"ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"}; // https://tools.ietf.org/html/rfc4648#page-8
	constexpr auto c_Base32hex_rfc4648_alphabet = detail::BaseN_Alphabet{"0123456789ABCDEFGHIJKLMNOPQRSTUV"}; // https://tools.ietf.org/html/rfc4648#page-10
	constexpr auto c_Base64_rfc4648_alphabet    = detail::BaseN_Alphabet{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/", true, '='}; // https://tools.ietf.org/html/rfc4648#page-5
	constexpr auto c_Base64url_rfc4648_alphabet = detail::BaseN_Alphabet{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_", true, '='}; // https://tools.ietf.org/html/rfc4648#page-7
	constexpr auto c_Base85_rfc1924_alphabet    = detail::BaseN_Alphabet{"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~", true}; // https://tools.ietf.org/html/rfc1924

	using Base2     = BaseN<c_Base2_alphabet,             8, 1>;
	using Base8     = BaseN<c_Base8_alphabet,             8, 3>;
	using Base10    = BaseN<c_Base10_alphabet,            2, 1>;
	using Base16    = BaseN<c_Base16_alphabet,            2, 1>;
	using Base32    = BaseN<c_Base32_rfc4648_alphabet,    8, 5>;
	using Base32hex = BaseN<c_Base32hex_rfc4648_alphabet, 8, 5>;
	using Base64    = BaseN<c_Base64_rfc4648_alphabet,    4, 3>;
	using Base64url = BaseN<c_Base64url_rfc4648_alphabet, 4, 3>;
	using Base85    = BaseN<c_Base85_rfc1924_alphabet,    5, 4>;
}