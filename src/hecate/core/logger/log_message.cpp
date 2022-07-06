#include "log_message.h"
#include "../logger.h"
#include <ostream>

namespace hecate::core::logger {
	LogMessage::LogMessage(
		Logger*            owner,
		e_LogCategory      category,
		const std::string& source_file,
		unsigned int       source_line
	):
		m_Owner(owner),
		m_MetaInfo{
			category,
			source_file,
			source_line
		}
	{
	}

	LogMessage::~LogMessage() {
		if (m_Owner)
			m_Owner->flush(this);
	}


	// allow iostream manipulator functions to work on this as well (such as std::endl and std::boolalpha)
	LogMessage& LogMessage::operator << (std::ostream& (*fn)(std::ostream&)) {
		(*fn)(m_Buffer);
		return *this;
	}
}