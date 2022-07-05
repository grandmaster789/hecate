#include "utf.h"
#include "../endian.h"

namespace hecate::util {
    namespace detail {
        template <typename t_String>
        [[nodiscard]] t_String to_string_helper(std::u16string_view sv) noexcept {
            t_String result;
            result.reserve(sv.size());

            auto inserter = std::back_inserter(result);
            auto it       = std::begin(sv);

            while (it != std::end(sv)) {
                char32_t x = utf16_to_utf32(it);
                utf32_to_utf8(x, inserter);
            }

            return result;
        }

        template <typename t_String>
        [[nodiscard]] t_String to_string_helper(std::u32string_view sv) noexcept {
            t_String result;
            result.reserve(sv.size());

            auto inserter = std::back_inserter(result);

            for (auto x : sv)
                utf32_to_utf8(x, inserter);

            return result;
        }

        template <typename t_String>
        [[nodiscard]] t_String to_string_helper(std::wstring_view sv) noexcept {
            if constexpr (sizeof(std::wstring::value_type) == sizeof(char16_t)) {
                auto sanitized = sanitize_u16string(
                    std::u16string(
                        reinterpret_cast<const char16_t*>(sv.data()),
                        sv.size()
                    )
                );

                return to_string_helper<t_String>(sanitized);
            }
            else {
                // NOTE cannot use static_assert here because it gets evaluated 
                //      even when this branch should be removed at compile-time...
                assert(sizeof(std::wstring::value_type) == sizeof(char32_t));

                auto sanitized = sanitize_u32string(
                    std::u32string(
                        reinterpret_cast<const char32_t*>(sv.data()),
                        sv.size()
                    )
                );

                return to_string_helper<t_String>(sanitized);
            }
        }
    }

    [[nodiscard]] std::string to_string(std::u8string_view sv) noexcept {
        return std::string(
            reinterpret_cast<const char*>(sv.data()),
            sv.size()
        );
    }

    [[nodiscard]] std::string to_string(std::u16string_view sv) noexcept {
        return detail::to_string_helper<std::string>(sv);
    }

    [[nodiscard]] std::string to_string(std::u32string_view sv) noexcept {
        return detail::to_string_helper<std::string>(sv);
    }

    [[nodiscard]] std::string to_string(std::wstring_view sv) noexcept {
        return detail::to_string_helper<std::string>(sv);
    }

    [[nodiscard]] std::u8string to_u8string(std::string_view sv) noexcept {
        return sanitize_u8string(
            std::u8string(
                reinterpret_cast<const char8_t*>(sv.data()),
                sv.size()
            )
        );
    }

    [[nodiscard]] std::u8string to_u8string(std::u16string_view sv) noexcept {
        return detail::to_string_helper<std::u8string>(sv);
    }

    [[nodiscard]] std::u8string to_u8string(std::u32string_view sv) noexcept {
        return detail::to_string_helper<std::u8string>(sv);
    }

    [[nodiscard]] std::u8string to_u8string(std::wstring_view sv) noexcept {
        return detail::to_string_helper<std::u8string>(sv);
    }

    [[nodiscard]] std::u16string to_u16string(std::string_view sv) noexcept {
        return to_u16string(
            sanitize_u8string(
                std::u8string(
                    reinterpret_cast<const char8_t*>(sv.data()),
                    sv.size()
                )
            )
        );
    }

    [[nodiscard]] std::u16string to_u16string(std::u8string_view sv) noexcept {
        std::u16string result;
        result.reserve(sv.size());

        auto inserter = std::back_inserter(result);
        auto it       = std::begin(sv);

        while (it != std::end(sv)) {
            char32_t x = detail::utf8_to_utf32(it);
            detail::utf32_to_utf16(x, inserter);
        }

        return result;
    }

    [[nodiscard]] std::u16string to_u16string(std::u32string_view sv) noexcept {
        std::u16string result;
        result.reserve(sv.size());

        auto inserter = std::back_inserter(result);

        for (auto x : sv)
            detail::utf32_to_utf16(x, inserter);

        return result;
    }

    [[nodiscard]] std::u16string to_u16string(std::wstring_view sv) noexcept {
        if constexpr (sizeof(std::wstring::value_type) == 2) {
            return sanitize_u16string(
                std::u16string(
                    reinterpret_cast<const char16_t*>(sv.data()), 
                    sv.size() 
                )
            );
        }
        else {
            auto sanitized = sanitize_u32string(
                std::u32string(
                    reinterpret_cast<const char32_t*>(sv.data()), 
                    sv.size()
                )
            );

            return to_u16string(std::move(sanitized));
        }
    }

    [[nodiscard]] std::u32string to_u32string(std::string_view sv) noexcept {
        return to_u32string(
            sanitize_u8string(
                std::u8string(
                    reinterpret_cast<const char8_t*>(sv.data()),
                    sv.size()
                )
            )
        );
    }

    [[nodiscard]] std::u32string to_u32string(std::u8string_view sv) noexcept {
        std::u32string result;
        result.reserve(sv.size());

        for (auto it = std::begin(sv); it != std::end(sv);)
            result += detail::utf8_to_utf32(it);

        return result;
    }

    [[nodiscard]] std::u32string to_u32string(std::u16string_view sv) noexcept {
        std::u32string result;
        result.reserve(sv.size());

        auto it = std::begin(sv);
        
        while (it != std::end(sv))
            result += detail::utf16_to_utf32(it);

        return result;
    }

    [[nodiscard]] std::u32string to_u32string(std::wstring_view sv) noexcept {
        if constexpr (sizeof(std::wstring::value_type) == 2) {
            return to_u32string(
                sanitize_u16string(
                    std::u16string(
                        reinterpret_cast<const char16_t*>(sv.data()),
                        sv.size()
                    )
                )
            );
        }
        else {
            return sanitize_u32string(
                std::u32string(
                    reinterpret_cast<const char32_t*>(sv.data()),
                    sv.size()
                )
            );
        }
    }

	[[nodiscard]] std::wstring to_wstring(std::string_view sv) noexcept {
		auto sanitized = sanitize_u8string(
			std::u8string(
                reinterpret_cast<const char8_t*>(sv.data()),
			    sv.size()
            )
		);

		return to_wstring(sanitized);
	}

	[[nodiscard]] std::wstring to_wstring(std::u8string_view sv) noexcept {
        // wchar_t should be either 2 or 4 bytes
        if constexpr (sizeof(std::wstring::value_type) == 2) {
            auto str = to_u16string(sv);
            return std::wstring(
                reinterpret_cast<const wchar_t*>(str.data()),
                str.size()
            );
        }
        else {
            auto str = to_u32string(sv);
            return std::wstring(
                reinterpret_cast<const wchar_t*>(str.data()),
                str.size()
            );
        }
	}

	[[nodiscard]] std::wstring to_wstring(std::u16string_view sv) noexcept {
        // wchar_t should be either 2 or 4 bytes
        if constexpr (sizeof(std::wstring::value_type) == 2) {
            return std::wstring(
                reinterpret_cast<const wchar_t*>(sv.data()),
                sv.size()
            );
        }
        else {
            auto str = to_u32string(sv);
            return std::wstring(
                reinterpret_cast<const wchar_t*>(sv.data()),
                sv.size()
            );
        }
	}

	[[nodiscard]] std::wstring to_wstring(std::u32string_view sv) noexcept {
        // wchar_t should be either 2 or 4 bytes
        if constexpr (sizeof(std::wstring::value_type) == 2) {
            auto str = to_u16string(sv);
            return std::wstring(
                reinterpret_cast<const wchar_t*>(sv.data()),
                sv.size()
            );
        }
        else {
            return std::wstring(
                reinterpret_cast<const wchar_t*>(sv.data()),
                sv.size()
            );
        }
	}

    [[nodiscard]] std::u32string sanitize_u32string(std::u32string&& str) noexcept {
        std::u32string result = std::move(str);

        for (char32_t& c : result)
            if (
                (c > 0x10'FFFF) ||
                (
                    (c >= 0xD800) &&
                    (c <= 0xDFFF)
                )
            )
                c = 0xFFFD; // replacement question mark  https://unicode-table.com/en/FFFD/


        return result;
    }

    [[nodiscard]] std::u16string sanitize_u16string(std::u16string&& str) noexcept {
        std::u16string result = std::move(str);

        size_t num_codepoints = str.size();

        bool perform_byteswap = false; // swap those bytes when required

        for (size_t i = 0; i < num_codepoints; ) {
            char16_t unit = result[i];

            if (perform_byteswap) {
                unit = byte_swap(unit);
                result[i] = unit;
            }
            
            if (unit == 0xFFFE)
                perform_byteswap = !perform_byteswap; // byteswap BOM (don't increment counter here)
            else if (unit >= 0xD800 && unit <= 0xDBFF) {
                size_t j = i++;

                if (i < num_codepoints) {
                    char16_t next_unit = result[i];

                    if (next_unit >= 0xDC00 && next_unit <= 0xDFFF)
                        ++i; // valid surrogate pair
                    else
                        result[j] = 0xFFFD; // replacement question mark
                }
                else
                    // high surrogate at end of string...
                    result[j] = 0xFFFD; // replacement question mark
            }
            else if (unit >= 0xDC00 && unit <= 0xDFFF)
                // invalid low surrogate
                result[i++] = 0xFFFD; // replacement question mark
            else
                ++i; // unrecognized, skip over this code unit
        }

        return result;
    }

	[[nodiscard]] std::u8string sanitize_u8string(std::u8string&& str) noexcept {
        std::u8string result = std::move(str);

        const auto first = std::begin(result);
        const auto last  = std::end(result);

        char32_t codepoint = 0;
        bool     is_valid  = true;

        auto old = first;

        for (auto it = first; is_valid && it != last;) {
            old = it;

            is_valid &= detail::utf8_to_utf32(it, last, codepoint);
        }

        if (is_valid)
            return result;
        
        // if we reached here some portion wasn't valid 
        // and needs to be re-encoded
        
        // - create an u8string with the valid part
        std::u8string valid(std::begin(result), old);
        valid.reserve(result.size());

        // - add the last decoded code point
        auto inserter = std::back_inserter(valid);
        detail::utf32_to_utf8(codepoint, inserter);

        // re-encode the rest
        for (auto it = old + 1; it != last;) {
            detail::utf8_to_utf32(it, last, codepoint);
            detail::utf32_to_utf8(codepoint, inserter);
        }
        
        return valid;
	}
}