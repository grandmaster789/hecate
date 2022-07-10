#pragma once

#include <string>
#include <vector>
#include "logger/log_category.h"
#include "logger/log_sink.h"

namespace hecate::core::logger {
	class LogMessage;
}

namespace hecate {
	class Logger {
	public:
		using LogSink       = core::logger::LogSink;
		using LogMessage    = core::logger::LogMessage;
		using e_LogCategory = core::logger::e_LogCategory;

		explicit Logger() = default;
		Logger(const std::string& filename); // log both to a file and to std::cout

		// this class provides a singleton interface, but can also be used as a regular object
		static Logger& instance() noexcept;

		void   add(LogSink sink) noexcept;
		void   removeAll() noexcept;
		size_t getNumSinks() const noexcept;

		void flush(LogMessage* message) noexcept;

		// create an associated LogMessage with the appropriate message metadata
		LogMessage operator()(
			e_LogCategory      category,
			const std::string& source_file,
			unsigned int       source_line
		) noexcept;

	private:
		std::vector<LogSink> m_Sinks;
	};
}

// macros to make it as painless as possible to log something
#define g_LogCategory(category) \
	(::hecate::Logger::instance()(::hecate::core::logger::e_LogCategory::category, __FILE__, __LINE__))

#define g_Log (g_LogCategory(info))

#define g_LogDebug   (g_LogCategory(debug))
#define g_LogInfo    (g_LogCategory(info))
#define g_LogError   (g_LogCategory(err))
#define g_LogWarning (g_LogCategory(warning))
#define g_LogFatal   (g_LogCategory(fatal))
