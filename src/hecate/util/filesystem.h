#pragma once

#include <string>
#include <filesystem>

namespace scimitar::util {
	std::string load_plaintext(const std::filesystem::path& p);
}
