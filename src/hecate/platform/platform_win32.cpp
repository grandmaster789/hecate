#include "platform.h"
#include "../dependencies.h"
#include "../core/engine.h"

#include <thread>
#include <chrono>

#if HECATE_PLATFORM != HECATE_PLATFORM_WINDOWS
	#error This is platform-specific, please remove it from the CMakeList so that it doesn't get compiled for any platform other than windows
#endif

namespace {
	struct PlatformState {
		HINSTANCE     m_InstanceHandle; // 
		HWND          m_WindowHandle;   // Hecate will only support a single window for now
		double        m_ClockFrequency; // frequency in Hz
		LARGE_INTEGER m_StartTime;      // 
	} g_PlatformState;

	static constexpr const wchar_t* k_HecateWindowClassName = L"hecate_window_class";
	static constexpr const wchar_t* k_HecateApplicationName = L"Hecate";

	LRESULT CALLBACK win32_message_pump(
		HWND     window_handle,
		uint32_t message,
		WPARAM   wparam,
		LPARAM   lparam
	) {
		// TODO ~~ forward key/mouse events to Input subsystem
		// notifications: https://docs.microsoft.com/en-us/windows/win32/winmsg/window-notifications
		// messages:      https://docs.microsoft.com/en-us/windows/win32/winmsg/window-messages
		switch (message) {
			
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_CLOSE:
			hecate::core::Engine::instance().stop();
			return 0;

		}

		return DefWindowProc( // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-defwindowproca
			window_handle, 
			message, 
			wparam, 
			lparam
		);
	}
}

namespace hecate::platform {
	Platform::Platform():
		System("Platform")
	{
	}

	bool Platform::init() {
		System::init();

		uint64_t state_size = sizeof(PlatformState);

		g_PlatformState.m_InstanceHandle = GetModuleHandle(nullptr);

		// we'll just consider a single window	
		WNDCLASS wc      = {};                                                          // https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassa
		wc.style         = CS_DBLCLKS;                                                  // https://docs.microsoft.com/en-us/windows/win32/winmsg/window-class-styles
		wc.lpfnWndProc   = win32_message_pump;                                          // A pointer to the window procedure
		wc.cbClsExtra    = 0;                                                           // The number of extra bytes to allocate following the window-class structure
		wc.cbWndExtra    = 0;                                                           // The number of extra bytes to allocate following the window instance
		wc.hInstance     = g_PlatformState.m_InstanceHandle;                            // A handle to the instance that contains the window procedure for the class
		wc.hIcon         = LoadIcon(g_PlatformState.m_InstanceHandle, IDI_APPLICATION); // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadicona
		wc.hCursor       = LoadCursor(g_PlatformState.m_InstanceHandle, IDC_ARROW);     // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadcursora
		wc.hbrBackground = nullptr;                                                     // A handle to the class background brush
		wc.lpszMenuName  = nullptr;                                                     // The resource name of the class menu, as the name appears in the resource file
		wc.lpszClassName = k_HecateWindowClassName;                                     // A pointer to a null-terminated string or an atom

		if (!RegisterClass(&wc)) {
			MessageBox(nullptr, L"Window class registration failed", L"Error", MB_ICONEXCLAMATION | MB_OK);
			return false;
		}

		uint32_t window_style =    // https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
			WS_OVERLAPPED |        // The window is an overlapped window. An overlapped window has a title bar and a border
			WS_SYSMENU |           // The window has a window menu on its title bar
			WS_CAPTION;            // The window has a title bar (includes the WS_BORDER style)

		uint32_t window_style_ex = // https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
			WS_EX_APPWINDOW;       // Forces a top-level window onto the taskbar when the window is visible

		RECT border = {};
		AdjustWindowRectEx(
			&border,
			window_style,
			FALSE,                 // menu
			window_style_ex
		);

		int32_t window_x      = border.left;
		int32_t window_y      = border.top;
		int32_t window_width  = border.right  - border.left;
		int32_t window_height = border.bottom - border.top;

		g_PlatformState.m_WindowHandle = CreateWindowEx(
			window_style_ex,
			k_HecateWindowClassName,
			k_HecateApplicationName,
			window_style,
			window_x,
			window_y,
			window_width,
			window_height,
			nullptr,                          // parent window handle
			nullptr,                          // menu handle
			g_PlatformState.m_InstanceHandle, // program instance
			nullptr                           // lpParam
		);

		if (!g_PlatformState.m_WindowHandle) {
			MessageBox(nullptr, L"Window creation failed", L"Error", MB_ICONEXCLAMATION | MB_OK);
			return false;
		}

		ShowWindow(g_PlatformState.m_WindowHandle, SW_SHOWMAXIMIZED); // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow

		// setup timing
		{
			LARGE_INTEGER frequency;
			QueryPerformanceFrequency(&frequency);                                            // https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancefrequency
			g_PlatformState.m_ClockFrequency = 1.0 / static_cast<double>(frequency.QuadPart);
			QueryPerformanceCounter(&g_PlatformState.m_StartTime);                            // https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancecounter
		}

		return true;
	}

	void Platform::update() {
		message_pump();
	}

	void Platform::shutdown() {
		System::shutdown();

		if (g_PlatformState.m_WindowHandle) {
			DestroyWindow(g_PlatformState.m_WindowHandle); // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-destroywindow
			g_PlatformState.m_WindowHandle = nullptr;
		}
	}

	bool Platform::message_pump() {
		MSG message;

		while (PeekMessage(
			&message, 
			g_PlatformState.m_WindowHandle,
			0,        // wMsgFilterMin
			0,        // wMsgFilterMax
			PM_REMOVE
		)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		return true;
	}

	void* Platform::allocate(uint64_t num_bytes, bool aligned_storage) {
		return nullptr;
	}

	void  Platform::deallocate(void* block, bool aligned_storage) {
	
	}

	void* Platform::zero_memory(void* block, uint64_t num_bytes) {
		return nullptr;
	}

	void* Platform::copy_memory(void* dest, const void* source, uint64_t num_bytes) {
		return nullptr;
	}

	void* Platform::set_memory(void* dest, int32_t value, uint64_t num_bytes) {
		return nullptr;
	}

	double Platform::get_absolute_time() {
		return {};
	}

	void Platform::sleep_ms(uint64_t milliseconds) {
		std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
	}

	void Platform::console_write(const char* message, uint8_t color) {

	}

	void Platform::console_write_error(const char* message, uint8_t color) {

	}
}