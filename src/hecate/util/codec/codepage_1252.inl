#include "codepage_1252.h"

#include <cstdint>

namespace hecate::util {
	inline constexpr char32_t codepage1252_to_utf32(char c) noexcept {
		uint8_t x = static_cast<uint8_t>(c);

		if (x <= 0x7F)
			return static_cast<char32_t>(x);

		if (x >= 0xA0)
			return static_cast<char32_t>(x);

		// https://en.wikipedia.org/wiki/Windows-1252 (also lists corresponding unicode code points)		
		switch (x) {
		case 128: return 0x20AC; // euro sign            https://unicode-table.com/en/20AC/
		
		case 130: return 0x201A; // low single quote     https://unicode-table.com/en/201A/
		case 131: return 0x0192; // florin symbol        https://unicode-table.com/en/0192/
		case 132: return 0x201E; // low double qoute     https://unicode-table.com/en/201E/
		case 133: return 0x2026; // triple dot leader    https://unicode-table.com/en/2026/
		case 134: return 0x2020; // dagger               https://unicode-table.com/en/2020/
		case 135: return 0x2021; // double dagger        https://unicode-table.com/en/2021/
		case 136: return 0x02C6; // circumflex accent    https://unicode-table.com/en/02C6/
		case 137: return 0x2030; // permille sign        https://unicode-table.com/en/2030/
		case 138: return 0x0160; // capital S with caron https://unicode-table.com/en/0160/
		case 139: return 0x2039; // left guillemet       https://unicode-table.com/en/2039/
		case 140: return 0x0152; // capital OE           https://unicode-table.com/en/0152/		
		
		case 142: return 0x017D; // capital Z with caron https://unicode-table.com/en/017D/		

		case 145: return 0x2018; // left single quote    https://unicode-table.com/en/2018/
		case 146: return 0x2019; // single comma quote   https://unicode-table.com/en/2019/
		case 147: return 0x201C; // left double quote    https://unicode-table.com/en/201C/
		case 148: return 0x201D; // right double quote   https://unicode-table.com/en/201D/
		case 149: return 0x2022; // black small circle   https://unicode-table.com/en/2022/
		case 150: return 0x2013; // large dash			 https://unicode-table.com/en/2013/
		case 151: return 0x2014; // dash                 https://unicode-table.com/en/2014/
		case 152: return 0x02DC; // small tilde          https://unicode-table.com/en/02DC/
		case 153: return 0x2122; // trademark symbol     https://unicode-table.com/en/2122/
		case 154: return 0x0161; // small s with caron   https://unicode-table.com/en/0161/
		case 155: return 0x203A; // right guillemet      https://unicode-table.com/en/203A/
		case 156: return 0x0153; // small OE             https://unicode-table.com/en/0153/
		
		case 158: return 0x017E; // small z with caron   https://unicode-table.com/en/017E/
		case 159: return 0x0178; // capital Y umlaubt    https://unicode-table.com/en/0178/

		// these 5 are unspecified, but MultiByteToWideChar maps these to C1 control codes?
		// doesn't really make much sense to me, so Imma leave it unspecified
		// https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
		// https://en.wikipedia.org/wiki/C0_and_C1_control_codes
		case 129: // unspecified in CP1252 (C1 code: High Octet Preset)
		case 141: // unspecified in CP1252 (C1 code: Reverse Line Feed)
		case 143: // unspecified in CP1252 (C1 code: Single-shift 3)
		case 144: // unspecified in CP1252 (C1 code: Device Control String)
		case 157: // unspecified in CP1252 (C1 code: End of Protected Area)
		default:
			return 0xFFFD; // replacement question mark  https://unicode-table.com/en/FFFD/
		}
	}
}