#pragma once

#include <string>
#include <chrono>
#include <cstddef>

namespace hecate {
	// STL literal cheat sheet
	/*
	*	Integer suffix literals (decimal base):
	*		(no suffix) ~> int / long / long long
	*		u/U			~> unsigned int / unsigned long / unsigned long long
	*		l/L			~> long / long long
	*		ul/UL		~> unsigned long / unsigned long long
	*		ll/LL		~> long long
	*		ull/ULL		~> unsigned long long
	*		z/Z			~> (C++23) ssize_t -- this file provides a preliminary _z  variant of this
	*		uz/UZ		~> (C++23) size_t  -- this file provides a preliminary _uz variant of this
	* 
	*	Integer suffix literals (hex/binary/octal base):
	*		(no suffix) ~> int / unsigned int / long / long long / unsigned long long
	*		u/U			~> unsigned int / unsigned long / unsigned long long
	*		l/L			~> long / unsigned long / long long / unsigned long long
	*		ul/UL		~> unsigned long / unsigned long long
	*		ll/LL		~> long long / unsigned long long
	*		ull/ULL		~> unsigned long long
	*		z/Z			~> (C++23) ssize_t
	*		uz/UZ		~> (C++23) size_t
	*		
	*	Floating point suffix literals:
	*		(no suffix) ~> double
	*		f/F			~> float
	*		l/L			~> long double
	*	[NOTE] C++17 introduced hex-digit sequences i.e. 
	*			0x1.4p3 == (hex)1.4 scaled by 2^3 
	*                   == (dec)1.25 * 8 
	*					== (dec)10.0
	* 
	*	Character prefix literals:
	*		''	 ~> char
	*		u8'' ~> UTF-8 character  (char8_t)  [NOTE] C++20
	*		u''  ~> UTF-16 character (char16_t)	[NOTE] C++11
	*		U''  ~> UTF-32 character (char32_t) [NOTE] C++11
	*		L''  ~> wchar_t character
	* 
	*	Character sequence literals (e.g. 'AB'):
	*		''   ~> int     (may be implementation defined)
	*		L''  ~> wchar_t (may be implementation defined)
	*
	*   String prefix literal:
	*		""   ~> zero-terminated char* sequence
	*		u8"" ~> UTF-8    string literal
	*		u""  ~> char16_t string literal
	*		U""  ~> char32_t string literal
	*		L""  ~> wchar_t  string literal
	* 
	*	std::literals::chrono_literals suffixes:
	*		d   ~> day in month
	*		h   ~> hour
	*		min ~> minute
	*		ms  ~> millisecond  (10^-3 s)
	*		ns  ~> nanosecond	(10^-9 s)
	*		s   ~> second
	*		us  ~> microsecond  (10^-6 s)
	*		y   ~> year
	* 
	*	std::literals::complex_literals suffixes:
	*		i   ~> imaginary component <double>
	*		if  ~> imaginary component <float>
	*		il  ~> imaginary component <long double>
	* 
	*	std::literals::string_literals suffixes:
	*		s	~> (const char*)     ~> std::string
	*			   (const char8_t*)  ~> std::u8string
	*			   (const char16_t*) ~> std::u16string
	*			   (const char32_t*) ~> std::u32string
	*		       (const wchar_t*)  ~> std::wstring
	* 
	*	std::literals::string_view_literals suffixes:
	*		sv  ~> (const char*)     ~> std::string_view
	*			   (const char8_t*)  ~> std::u8string_view
	*			   (const char16_t*) ~> std::u16string_view
	*			   (const char32_t*) ~> std::u32string_view
	*              (const wchar_t*)  ~> std::wstring_view
	*/
	using namespace std::literals;

	using ssize_t = std::make_signed<std::size_t>::type; // signed size type

#define ssizeof(x) (static_cast<ssize_t>(sizeof(x)))

	constexpr ssize_t operator "" _z (unsigned long long value) noexcept; // 123_z  ~> (ssize_t)(123)
	constexpr size_t  operator "" _uz(unsigned long long value) noexcept; // 123_uz ~> (size_t) (123)
}

#include "literals.inl"