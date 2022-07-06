#include "logger.h"
#include "logger/log_message.h"
#include "logger/log_sink.h"

namespace hecate::core {
	Logger::Logger(const std::string& filename) {
		add(logger::makeFileSink(filename));
	}

	// this class provides a singleton interface, but can also be used as a regular object
	Logger& Logger::instance() {
		static Logger global_logger("hecate.log");
		return global_logger;
	}

	Logger::LogMessage Logger::operator()(
		e_LogCategory      category,
		const std::string& source_file,
		unsigned int       source_line
	) {
		return LogMessage(
			this, 
			category, 
			source_file, 
			source_line
		);
	}

	void Logger::add(LogSink sink) {
		m_Sinks.push_back(std::move(sink));
	}

	void Logger::removeAll() {
		m_Sinks.clear();
	}

	size_t Logger::getNumSinks() const {
		return m_Sinks.size();
	}

	void Logger::flush(logger::LogMessage* message) {
		auto info = message->m_MetaInfo;
		auto str  = message->m_Buffer.str();

		for (auto& sink : m_Sinks)
			sink.write(info, str);
	}
}