#pragma once

#include <iosfwd>

namespace hecate::core::logger {
	enum class e_LogCategory {
		debug,
		info,
		err,
		warning,
		fatal
	};

	std::ostream& operator << (std::ostream& os, e_LogCategory cat);
}
