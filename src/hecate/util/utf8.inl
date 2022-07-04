#pragma once

#include "utf8.h"
#include <stdexcept>

namespace hecate::util::detail {
    constexpr const char* codepoint_to_utf8(
        char32_t codepoint, 
        char     seq[8]
    ) {
        int n = 0;

             if (codepoint <  0x80)       n = 1;
        else if (codepoint <  0x800)      n = 2;
        else if (codepoint <  0x10000)    n = 3;
        else if (codepoint <  0x200000)   n = 4;
        else if (codepoint <  0x4000000)  n = 5;
        else if (codepoint <= 0x7fffffff) n = 6;

        seq[n] = '\0';

        switch (n) {
        case 6: 
            seq[5] = 0x80 | (codepoint & 0x3f); 
            codepoint = codepoint >> 6; 
            codepoint |= 0x4000000; 
            [[fallthrough]];

        case 5: 
            seq[4] = 0x80 | (codepoint & 0x3f); 
            codepoint = codepoint >> 6; 
            codepoint |= 0x200000; 
            [[fallthrough]];

        case 4: 
            seq[3] = 0x80 | (codepoint & 0x3f); 
            codepoint = codepoint >> 6; 
            codepoint |= 0x10000; 
            [[fallthrough]];

        case 3: 
            seq[2] = 0x80 | (codepoint & 0x3f); 
            codepoint = codepoint >> 6; 
            codepoint |= 0x800; 
            [[fallthrough]];

        case 2: 
            seq[1] = 0x80 | (codepoint & 0x3f); 
            codepoint = codepoint >> 6; 
            codepoint |= 0xc0; 
            [[fallthrough]];

        case 1: 
            seq[0] = static_cast<char>(codepoint);
            break;

        default:
            break;
        }

        return seq;
    }

    namespace utf8_mask {
        static constexpr uint8_t const k_First  = 0b1000'0000; // 128
        static constexpr uint8_t const k_Second = 0b0100'0000; //  64
        static constexpr uint8_t const k_Third  = 0b0010'0000; //  32
        static constexpr uint8_t const k_Fourth = 0b00010000; //  16
    };

    namespace utf8 {
        enum {
            accept = 0,
            reject = 12
        };
    }
}

namespace hecate::util {
    constexpr bool is_space(char32_t codepoint) {
        switch (codepoint) {
        case 9:    // \t (tab)
        case 10:   // \n (newline)
        case 11:   // \v (vertical tab)
        case 12:   // \f (form feed)
        case 13:   // \r (carriage return)
        case 32:   // (space)
        case 0xA0: // nbsp; (non-breaking space)
            return true;

        default:
            return false;
        }
    }

    constexpr bool is_space(const char* utf8) {
        return is_space(codepoint(utf8));
    }

    constexpr bool is_newline(char32_t codepoint) {
        // [NOTE] there's a bit of platform-specific interpretations here
        //        on windows it's still very common to do both a newline and
        //        carriage return as a form of 'newline'
        switch (codepoint) {
        case 10: // \n (newline)
        case 13: // \r (carriage return)
            return true;

        default:
            return false;
        }
    }

    constexpr bool is_newline(const char* utf8) {
        return is_newline(codepoint(utf8));
    }

    constexpr bool is_punctuation(char32_t codepoint) {
        if (codepoint < 0x80)
            return std::ispunct(codepoint);

        // 0xA0 - 0xBF Latin-1 supplement
        // https://unicodeplus.com/block/0080
        if (codepoint >= 0xA0 && codepoint <= 0xBF)
            return true;

        // 0x2000 - 0x206F is General Punctuation Block
        // https://unicodeplus.com/block/2000
        if (codepoint >= 0x2000 && codepoint <= 0x206F)
            return true;

        return false;
    }

    constexpr bool is_punctuation(const char* utf8) {
        return is_punctuation(codepoint(utf8));
    }

    constexpr std::string codepoint_to_utf8(char32_t codepoint) {
        std::string result{ 8 };
        detail::codepoint_to_utf8(codepoint, result.data());
        return result;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Decoding utf8
    //
    // Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
    // See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
    ////////////////////////////////////////////////////////////////////////////

    constexpr char32_t decode_utf8(char32_t& state, char32_t& codepoint, char byte) {
        constexpr uint8_t utf8d[] = {
            // The first part of the table maps bytes to character classes that
            // to reduce the size of the transition table and create bitmasks.
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,       9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
            7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,       7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
            8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,       2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
            10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3,      11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

            // The second part is a transition table that maps a combination
            // of a state of the automaton and a character class to a state.
            0,12,24,36,60,96,84,12,12,12,48,72,    12,12,12,12,12,12,12,12,12,12,12,12,
            12, 0,12,12,12,12,12, 0,12, 0,12,12,   12,24,12,12,12,12,12,24,12,24,12,12,
            12,12,12,12,12,12,12,24,12,12,12,12,   12,24,12,12,12,12,12,12,12,24,12,12,
            12,12,12,12,12,12,12,36,12,36,12,12,   12,36,12,12,12,12,12,36,12,36,12,12,
            12,36,12,12,12,12,12,12,12,12,12,12,
        };

        char32_t type = utf8d[byte];

        codepoint = (state != detail::utf8::accept) ?
            (byte & 0x3fu) | (codepoint << 6) :
            (0xff >> type) & (byte)
            ;

        state = utf8d[256 + state + type];
        return state;
    }

    ////////////////////////////////////////////////////////////////////////////
    // utf8 Iteration. See A code point iterator adapter for C++ strings in
    // UTF-8 by Angel Jose Riesgo: http://www.nubaria.com/en/blog/?p=371
    ////////////////////////////////////////////////////////////////////////////

    constexpr const char* next_utf8(const char* utf8, const char* last) {
        char c = *utf8;
        std::size_t offset = 1;

        if (c & detail::utf8_mask::k_First)
            offset =
            (c & detail::utf8_mask::k_Third) ?
            ((c & detail::utf8_mask::k_Fourth) ? 4 : 3) : 2
            ;

        utf8 += offset;
        if (utf8 > last)
            utf8 = last;

        return utf8;
    }

    constexpr const char* prev_utf8(const char* utf8, const char* first) {
        if (*--utf8 & detail::utf8_mask::k_First) {
            if ((*--utf8 & detail::utf8_mask::k_Second) == 0) {
                if ((*--utf8 & detail::utf8_mask::k_Second) == 0)
                    --utf8;
            }
        }

        if (utf8 < first)
            utf8 = first;

        return utf8;
    }

    constexpr char32_t codepoint(const char*& utf8) {
        char32_t state = 0;
        char32_t cp;

        while (decode_utf8(state, cp, uint8_t(*utf8)))
            utf8++;

        ++utf8; // advance one past the end

        return cp;
    }
    
    constexpr std::string to_utf8(std::u32string_view utf32) {
        std::string utf8;

        for (auto codepoint : utf32) {
            char seq[8];
            detail::codepoint_to_utf8(codepoint, seq);
            utf8.append(seq);
        }

        return utf8;
    }

    constexpr std::u32string to_utf32(std::string_view sv) {
        std::u32string s32;
        char const* last = sv.data() + sv.size();
        char32_t state = 0;
        char32_t cp;

        for (char const* i = sv.data(); i != last; ++i) {
            while (decode_utf8(state, cp, uint8_t(*i)))
                i++;

            s32.push_back(cp);
        }

        if (state == detail::utf8::reject)
            throw std::runtime_error{ "Error: Invalid utf8." };

        return s32;
    }

    constexpr bool is_valid_utf8(std::string_view sv) {
        char const* last = sv.data() + sv.size();
        char32_t state = 0;
        char32_t cp;

        for (char const* i = sv.data(); i != last; ++i) {
            while (decode_utf8(state, cp, uint8_t(*i)))
                i++;
        }

        return state != detail::utf8::reject;
    }
}