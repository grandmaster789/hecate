#include "debugger.h"
#include "string_util.h"
#include "codec/utf.h"
#include "../preprocessor.h"

#if HECATE_PLATFORM == HECATE_PLATFORM_WINDOWS

namespace hecate::util {
	bool debugger_present() noexcept {
		return IsDebuggerPresent();
	}

	void debugger_log(std::string_view text) noexcept {
		auto message   = std::string(text) + "\r\n";
		auto messageWS = to_wstring(message);

		OutputDebugStringW(messageWS.data());
	}

	void debugger_dialog_message(
		std::string_view caption,
		std::string_view message
	) noexcept {
		std::wstring cap_ws = to_wstring(caption);
		std::wstring msg_ws = to_wstring(message);

		// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messageboxw
		MessageBoxW(
			nullptr,       // HWND parent
			msg_ws.data(), // text
			cap_ws.data(), // caption
			MB_APPLMODAL | // type flags 
			MB_OK |
			MB_ICONERROR
		);
	}
}

#elif HECATE_PLATFORM == HECATE_PLATFORM_LINUX
	// not sure what a reasonable development workflow looks like here tbh
#else
	#error "not implemented"
#endif