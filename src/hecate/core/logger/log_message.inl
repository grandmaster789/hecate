#pragma once

namespace hecate::core::logger {
	template <typename T>
	LogMessage& LogMessage::operator << (const T& value) {
		m_Buffer << value;
		return *this;
	}
}