#pragma once

#include "../preprocessor.h"
#include <string_view>

#if HECATE_OS == HECATE_OS_WINDOWS
	#define debug_break DebugBreak
#elif HECATE_OS == HECATE_OS_LINUX
	#define debug_break __builtin_trap // should work for both GCC and Clang
#else
	#error "not implemented"
#endif

// NOTE the actual debugger tested with is Visual Studio; at some point I should figure out VScode and CLion
// NOTE the naming is a bit awkward, mostly in order to dodge overload set issues
namespace hecate::util {
	bool debugger_present() noexcept;
	
	void debugger_log(std::string_view message) noexcept;
	void debugger_dialog_message(
		std::string_view caption, 
		std::string_view message
	) noexcept;

	template <typename...Ts>
	void debugger_msg(
		std::string_view fmt,
		Ts&&...          args
	) noexcept;

	template <typename...Ts>
	void debugger_dialog(
		std::string_view caption,
		std::string_view fmt,
		Ts&&...          args
	) noexcept;

	template <typename...Ts>
	[[noreturn]] no_inline void debugger_fatal(
		const char*      source_file,
		int              source_line,
		std::string_view fmt,
		Ts&&...          args
	) noexcept;
}

template <typename... Ts>
void debugger_abort(Ts... args) {
	::hecate::util::debugger_fatal(__FILE__, __LINE__, args...);
}

template <typename ...Ts>
void debugger_message(Ts...args) {
	::hecate::util::debugger_msg(args...);
}

#include "debugger.inl"