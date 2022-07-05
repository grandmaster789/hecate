#pragma once

#include "debugger.h"
#include <format>
#include <cstdlib>

namespace hecate::util {
	template <typename...Ts>
	void debugger_msg(
		std::string_view fmt,
		Ts&&...          args
	) noexcept {
		debugger_log(
			std::vformat(fmt, std::make_format_args(std::forward<Ts>(args)...))
		);
	}

	template <typename...Ts>
	void debugger_dialog(
		std::string_view caption,
		std::string_view fmt,
		Ts&&...          args
	) noexcept {
		debugger_dialog_message(
			caption,
			std::vformat(fmt, std::make_format_args(std::forward<Ts>(args)...))
		);
	}

	template <typename...Ts>
	[[noreturn]] no_inline void debugger_fatal(
		const char*      source_file,
		int              source_line,
		std::string_view fmt,
		Ts&&...          args
	) noexcept {
		std::string message;

		// do the formatting here to preserve source location 

		if constexpr (sizeof...(Ts) == 0) {
			message = fmt;
		}
		else {
			message = std::vformat(fmt, std::make_format_args(std::forward<Ts>(args)...));
		}

		if (debugger_present()) {
			debugger_msg("{}:{} {}", source_file, source_line, message);
			debug_break();
		}
		else
			debugger_dialog("Fatal", "{}:{} {}", source_file, source_line, message);

		std::abort(); // https://en.cppreference.com/w/c/program/abort
	}
}