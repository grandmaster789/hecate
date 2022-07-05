#include "log_sink.h"
#include "../../util/debugger.h"

#include <rang.hpp>

#include <iostream>
#include <fstream>
#include <format>
#include <chrono>
#include <filesystem>

namespace {
	struct FileSink {
		FileSink(const std::filesystem::path& p):
			m_File(p.string())
		{
			if (!m_File.good())
				debugger_abort("Failed to create file sink");
		}

		void operator()(
			const hecate::core::logger::LogMessage::MetaInfo& info, 
			const std::string&                                  message
		) {
			auto now = std::chrono::system_clock::now();

			auto simplified_source_path = std::filesystem::path(info.m_SourceFile)
				.filename()
				.string();

			// in the MSVC STL v14.29.30133 we can't provide formatter specializations... fixes are
			// coming in Visual Studio 2022 but at the moment that's still a Preview IDE so I'm holding out
			// for a bit...
			// time formatting also seems broken, so I'll just use the old school C approach... which has platform issues...

			/*m_File << std::format(
				"[{%T}] {} {} ({}:{})\n",
				now, // https://en.cppreference.com/w/cpp/chrono/system_clock/formatter#Format_specification
				cat.str(),
				message,
				info.m_SourceFile,
				info.m_SourceLine
			);*/

			auto tc = std::chrono::system_clock::to_time_t(now);
			tm localtime;

#if HECATE_PLATFORM == HECATE_PLATFORM_WINDOWS
				localtime_s(&localtime, &tc);
#else
				localtime_r(&localtime, &tc);
#endif

			m_File 
				<< std::put_time(&localtime, "[%H:%M:%S] ")
				<< info.m_Category
				<< message
				<< " (" << simplified_source_path
				<< ":"  << info.m_SourceLine
				<< ")\n";
		}

		std::ofstream m_File;
	};
}

namespace hecate::core::logger {
	void LogSink::write(
		const LogMessage::MetaInfo& info,
		const std::string&          message
	) {
		(*m_Wrapper)(info, message);
	}

	LogSink makeStdOutSink() {
		return [](
			const LogMessage::MetaInfo& info, 
			const std::string&          message
		) {
			switch (info.m_Category) {
			case e_LogCategory::debug:   std::cout << rang::fgB::green;                     break;
			case e_LogCategory::warning: std::cout << rang::fgB::yellow;                    break;
			case e_LogCategory::err:     std::cout << rang::fgB::red;                       break;
			case e_LogCategory::fatal:   std::cout << rang::fgB::red << rang::style::blink; break;
			default:
				// 'info' gets default color+style
				break;
			}

			std::cout << info.m_Category << message << "\n";
			std::cout << rang::style::reset;
		};
	}

	LogSink makeStdErrSink() {
		return [](
			const LogMessage::MetaInfo& info,
			const std::string& message
		) {
			std::cerr << info.m_Category << message << "\n";
		};
	}

	LogSink makeFileSink(const std::filesystem::path& p) {
		return FileSink(p);
	}

#if HECATE_PLATFORM == HECATE_PLATFORM_WINDOWS
	LogSink makeDebuggerSink() {
		return[](
			const LogMessage::MetaInfo& info,
			const std::string&          message
		) {
			(void)info;
			debugger_message(message);
		};
	}
#endif
}