#include "log_category.h"
#include <ostream>

namespace hecate::core::logger {
	std::ostream& operator << (std::ostream& os, e_LogCategory cat) {

		switch (cat) {
		case e_LogCategory::debug:   os << "[dbg] "; break;
		case e_LogCategory::info:    os << "      "; break; // should be the most common, and hence the least annotated
		case e_LogCategory::warning: os << "*wrn* "; break;

		case e_LogCategory::err:     os << "< ERROR >     "; break;
		case e_LogCategory::fatal:   os << "<## FATAL ##> "; break;

		default:
			os << "[???] ";
			break;
		}

		return os;
	}
}