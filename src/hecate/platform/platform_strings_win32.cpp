#include "platform_strings.h"
#include "../dependencies.h"

namespace hecate::platform {
	std::wstring to_wstring(const std::string& text) {
		if (text.empty())
			return L"";

		// https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
		const auto size_needed = MultiByteToWideChar(
			CP_UTF8,
			0,
			&text.at(0),
			static_cast<int>(text.size()),
			nullptr,
			0
		);

		if (size_needed <= 0)
			throw std::runtime_error("MultiByteToWideChar() failed: " + std::to_string(size_needed));

		std::wstring result(size_needed, 0);
		MultiByteToWideChar(
			CP_UTF8,
			0,
			&text.at(0),
			static_cast<int>(text.size()),
			&result.at(0),
			size_needed
		);

		return result;
	}

	std::string to_string(const std::wstring& wide_string) {
		if (wide_string.empty())
			return "";

		// https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
		const auto size_needed = WideCharToMultiByte(
			CP_UTF8,                 // Code page to use in performing the conversion
			0,                       // Flags indicating the conversion type
			&wide_string.at(0),      // Pointer to the Unicode string to convert
			(int)wide_string.size(), // Size, in characters, of the unicode string
			nullptr,                 // (optional) Pointer to a buffer that receives the converted string
			0,                       // (optional) Size, in bytes, of the buffer indicated by previous arg
			nullptr,                 // (optional) Pointer to the character to use if a character cannot be represented in the specified code page
			nullptr                  // (optional) Pointer to a flag that indicates if the function has used a default character in the conversion
		);

		if (size_needed <= 0)
			throw std::runtime_error("WideCharToMultiByte() failed: " + std::to_string(size_needed));

		std::string result(size_needed, 0);
		WideCharToMultiByte(
			CP_UTF8,
			0,
			&wide_string.at(0),
			(int)wide_string.size(),
			&result.at(0),
			size_needed,
			nullptr,
			nullptr
		);

		return result;
	}
}