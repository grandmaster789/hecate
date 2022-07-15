#pragma once

#include <string>

namespace hecate::platform {
	// NOTE the standard library implementation got deprecated in c++17 without a replacement (!), 
	//      so we'll have to depend on OS-level facilities instead...
	std::wstring to_wstring(const std::string&  string);
	std::string  to_string (const std::wstring& wide_string);
}