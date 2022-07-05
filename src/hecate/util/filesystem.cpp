#include "filesystem.h"

#include <fstream>
#include <sstream>

namespace hecate::util {
	std::string load_plaintext(const std::filesystem::path& p) {
		using namespace std::filesystem;

		if (!exists(p))
			throw std::runtime_error("Path does not exist");

		if (!is_regular_file(p))
			throw std::runtime_error("Path is not a regular file");

		std::ifstream in(p.string());

		if (!in.good())
			throw std::runtime_error("Failed to open file");

		std::stringstream buffer;
		buffer << in.rdbuf();

		return buffer.str();
	}
}