#include "platform.h"
#include "../dependencies.h"
#include "../core/engine.h"
#include "../input/mouse.h"
#include "../input/keyboard.h"
#include "../input/input.h"

#include <thread>
#include <chrono>

#if HECATE_PLATFORM != HECATE_PLATFORM_WINDOWS
	#error This is platform-specific, please remove it from the CMakeList so that it doesn't get compiled for any platform other than windows
#endif

namespace {
	HINSTANCE     g_InstanceHandle = nullptr; // Application instance handle
	HWND          g_WindowHandle   = nullptr; // Hecate will only support a single window for now
	double        g_ClockFrequency = 1;       // frequency in Hz
	LARGE_INTEGER g_StartTime      = {};      // 

	static constexpr const wchar_t* k_HecateWindowClassName = L"hecate_window_class";
	static constexpr const wchar_t* k_HecateApplicationName = L"Hecate";

	LRESULT CALLBACK win32_message_pump(
		HWND     window_handle,
		uint32_t message,
		WPARAM   wparam,
		LPARAM   lparam
	) {
		auto userdata = GetWindowLongPtr(window_handle, GWLP_USERDATA);

		if (!userdata)
			return DefWindowProc(window_handle, message, wparam, lparam);

		//auto* platform_object = (hecate::Platform*)userdata;

		// TODO ~~ forward key/mouse events to Input subsystem
		// notifications: https://docs.microsoft.com/en-us/windows/win32/winmsg/window-notifications
		// messages:      https://docs.microsoft.com/en-us/windows/win32/winmsg/window-messages
		switch (message) {

		case WM_DESTROY:
			PostQuitMessage(0); // well, this is fine if we're only using a single window
			return 0;

		case WM_CLOSE:
			hecate::Engine::instance().stop();
			return 0;
		}

		return DefWindowProc(
			window_handle,
			message,
			wparam,
			lparam
		);
	}
	
	std::vector<DISPLAY_DEVICE> enumerate_display_devices() {
		std::vector<DISPLAY_DEVICE> result; 

		/*
		https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-enumdisplaydevicesa

		To query all display devices in the current session, call this function (EnumDisplayDevices) in a loop, 
		starting with iDevNum set to 0, and incrementing iDevNum until the function fails. 
		To select all display devices in the desktop, use only the display devices that have 
		the DISPLAY_DEVICE_ATTACHED_TO_DESKTOP flag in the DISPLAY_DEVICE structure
		*/

		for (int i = 0;; ++i) {
			DISPLAY_DEVICE dd = {}; // https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-display_devicea
			dd.cb = sizeof(DISPLAY_DEVICE);
			
			if (!EnumDisplayDevices(NULL, i, &dd, 0))
				break; // stop if we coundn't find any more devices

			if (dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)
				continue; // skip over mirroring (pseudo) devices

			// NOTE the DISPLAY_DEVICE documentation doesn't mention this, but the value of DISPLAY_DEVICE_ATTACHED_TO_DESKTOP is equal to DISPLAY_DEVICE_ACTIVE
			if (!(dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)) 
				continue; // skip non-attached displays

			result.push_back(dd);
		}

		return result;
	}

	void list_display_devices() {
		auto display_devices = enumerate_display_devices();

		std::stringstream sstr;
		sstr << "Listing currently active display devices:\n";
		for (size_t i = 0; i < display_devices.size(); ++i) {
			const auto& x = display_devices[i];

			sstr << std::vformat(
				"\t[#{}]: {} {}\n",
				std::make_format_args(
					i,
					hecate::to_string(x.DeviceString),
					hecate::to_string(x.DeviceID)
				)
			);
		}

		g_LogDebug << sstr.str();
	}

	DEVMODE get_current_displaymode(DISPLAY_DEVICE dd) {
		DEVMODE mode = {};

		mode.dmSize = sizeof(mode);
		mode.dmDriverExtra = 0;

		// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enumdisplaysettingsw
		if (!EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &mode))
			throw std::runtime_error("Failed to query current display mode");

		return mode;
	}
}

namespace hecate {
	Platform::Platform():
		System("Platform")
	{
		add_dependency("Input"); // we need this system to be available so that we can do keybinding etc

		register_setting("main_window_width",  &m_MainWindowWidth);
		register_setting("main_window_height", &m_MainWindowHeight);
		register_setting("display_device",     &m_DisplayDeviceIdx);
	}

	bool Platform::init() {
		System::init();
		
		// not sure if the VS_DPI_AWARE property does anything, so I'm also doing it in code
		SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2); // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setthreaddpiawarenesscontext

		g_InstanceHandle = GetModuleHandle(nullptr);

		// we'll just consider a single window
		// NOTE Unregistering the WindowClass 
		WNDCLASSEX wc    = {};                                          // https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassexa
		wc.cbSize        = sizeof(WNDCLASSEX);
		wc.style         = CS_DBLCLKS;                                  // https://docs.microsoft.com/en-us/windows/win32/winmsg/window-class-styles
		wc.lpfnWndProc   = win32_message_pump;                          // A pointer to the window procedure
		wc.cbClsExtra    = 0;                                           // The number of extra bytes to allocate following the window-class structure
		wc.cbWndExtra    = 0;                                           // The number of extra bytes to allocate following the window instance
		wc.hInstance     = g_InstanceHandle;                            // A handle to the instance that contains the window procedure for the class
		wc.hIcon         = LoadIcon(g_InstanceHandle, IDI_APPLICATION); // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadicona
		wc.hIconSm       = LoadIcon(g_InstanceHandle, IDI_WINLOGO);     // A handle to a small icon that is associated with the window class
		wc.hCursor       = LoadCursor(g_InstanceHandle, IDC_ARROW);     // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadcursora
		wc.hbrBackground = nullptr;                                     // A handle to the class background brush
		wc.lpszMenuName  = nullptr;                                     // The resource name of the class menu, as the name appears in the resource file
		wc.lpszClassName = k_HecateWindowClassName;                     // A pointer to a null-terminated string or an atom

		if (!RegisterClassEx(&wc)) {
			MessageBox(nullptr, L"Window class registration failed", L"Error", MB_ICONEXCLAMATION | MB_OK);
			return false;
		}

		auto display_monitors = enumerate_display_devices();
		if (display_monitors.empty())
			throw std::runtime_error("No display devices are available");

		if (display_monitors.size() < m_DisplayDeviceIdx) {
			g_LogWarning << "Display device " << m_DisplayDeviceIdx << " is unavailable, falling back to device #0";			
			m_DisplayDeviceIdx = 0;
			
			list_display_devices(); // might as well show the current set of valid options
		}

		uint32_t window_style =    // https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
			WS_CLIPSIBLINGS |      // Clips child windows relative to each other
			WS_CLIPCHILDREN |      // Excludes the area occupied by child windows when drawing occurs within the parent window
			WS_SYSMENU |           // The window has a window menu on its title bar
			WS_MINIMIZEBOX;        // The window has a minimize button

		uint32_t window_style_ex = // https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
			WS_EX_APPWINDOW;       // Forces a top-level window onto the taskbar when the window is visible

		if (m_MainWindowWidth == -1)
			m_MainWindowWidth = CW_USEDEFAULT;
		if (m_MainWindowHeight == -1)
			m_MainWindowHeight = CW_USEDEFAULT;

		// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexa
		g_WindowHandle = CreateWindowEx(
			window_style_ex,
			k_HecateWindowClassName,
			k_HecateApplicationName,
			window_style,
			CW_USEDEFAULT,      // x (left)
			CW_USEDEFAULT,      // y (top)
			m_MainWindowWidth,  // width 
			m_MainWindowHeight, // height
			nullptr,            // pare nt window handle
			nullptr,            // menu handle
			g_InstanceHandle,   // program instance
			nullptr             // lpParam
		);

		if (!g_WindowHandle) {
			MessageBox(nullptr, L"Window creation failed", L"Error", MB_ICONEXCLAMATION | MB_OK);
			return false;
		}

		// put a pointer to this Platform object in the Window userdata
		SetWindowLongPtr(g_WindowHandle, GWLP_USERDATA, (LONG_PTR)this);

		// apply sizes, center the window
		{	
			auto display_mode = get_current_displaymode(display_monitors[m_DisplayDeviceIdx]);

			// extract virtual desktop coordinates
			int virtual_x      = display_mode.dmPosition.x;
			int virtual_y      = display_mode.dmPosition.y;
			int virtual_width  = display_mode.dmPelsWidth;
			int virtual_height = display_mode.dmPelsHeight;

			// create a centered rect on the associated display device
			RECT rect;

			GetClientRect(g_WindowHandle, &rect);
			m_MainWindowWidth  = rect.right - rect.left;
			m_MainWindowHeight = rect.bottom - rect.top;

			rect.left   = virtual_x + (virtual_width  - m_MainWindowWidth)  / 2;
			rect.top    = virtual_y + (virtual_height - m_MainWindowHeight) / 2;
			rect.right  = rect.left + m_MainWindowWidth;
			rect.bottom = rect.top  + m_MainWindowHeight;

			// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowpos
			SetWindowPos(
				g_WindowHandle,
				HWND_TOP, // hWndInsertAfter
				rect.left,
				rect.top,
				rect.right - rect.left,
				rect.bottom - rect.top,
				0
			);

			ShowWindow(g_WindowHandle, SW_SHOW); // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow

			GetWindowRect(g_WindowHandle, &rect);
			RECT client_rect;
			GetClientRect(g_WindowHandle, &client_rect);

			g_Log << std::format(
				"({}x{}) Window created at ({}, {}); Client rect is {}x{}",
				rect.right - rect.left,
				rect.bottom - rect.top,
				rect.left,
				rect.top,
				client_rect.right - client_rect.left,
				client_rect.bottom - client_rect.top
			);
		}		

		// setup timing
		{
			LARGE_INTEGER frequency;
			QueryPerformanceFrequency(&frequency);                            // https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancefrequency
			g_ClockFrequency = 1.0 / static_cast<double>(frequency.QuadPart);
			QueryPerformanceCounter(&g_StartTime);                            // https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancecounter
		}

		// setup input devices (that are coupled to the window)
		{
			auto* input_system = m_Engine->get<Input>();

			m_Keyboard = std::make_unique<input::Keyboard>(input_system);
			m_Mouse    = std::make_unique<input::Mouse>(input_system);
		}

		return true;
	}

	void Platform::update() {
		MSG message;

		while (PeekMessage(
			&message,
			g_WindowHandle,
			0,        // wMsgFilterMin
			0,        // wMsgFilterMax
			PM_REMOVE
		)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	void Platform::shutdown() {
		System::shutdown();

		if (g_WindowHandle) {
			DestroyWindow(g_WindowHandle); // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-destroywindow
			g_WindowHandle = nullptr;
		}
	}

	unsigned int Platform::get_dpi() const noexcept {
		// if the window moves to another monitor the value may change
		return GetDpiForWindow(g_WindowHandle);
	}

	double Platform::get_absolute_time() {
		if (!g_ClockFrequency) {
			g_LogWarning << "Clock frequency is unknown";
			return 0.0;
		}

		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return static_cast<double>(now.QuadPart) * g_ClockFrequency;
	}

	void Platform::sleep_ms(uint64_t milliseconds) {
		//std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));

		Sleep(static_cast<DWORD>(milliseconds));
	}

	std::vector<const char*> Platform::get_required_vulkan_extensions() {
		return {
			"VK_KHR_win32_surface" // https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#VK_KHR_win32_surface
		};
	}

	std::wstring to_wstring(const std::string& text) {
		if (text.empty())
			return L"";

		// https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
		const auto size_needed = MultiByteToWideChar(
			CP_UTF8, 
			0, 
			&text.at(0),
			static_cast<int>(text.size()),
			nullptr, 
			0
		);

		if (size_needed <= 0)
			throw std::runtime_error("MultiByteToWideChar() failed: " + std::to_string(size_needed));

		std::wstring result(size_needed, 0);
		MultiByteToWideChar(
			CP_UTF8, 
			0, 
			&text.at(0),
			static_cast<int>(text.size()),
			&result.at(0), 
			size_needed
		);

		return result;
	}

	std::string to_string(const std::wstring& wide_string) {
		if (wide_string.empty())
			return "";

		// https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
		const auto size_needed = WideCharToMultiByte(
			CP_UTF8,                 // Code page to use in performing the conversion
			0,                       // Flags indicating the conversion type
			&wide_string.at(0),      // Pointer to the Unicode string to convert
			(int)wide_string.size(), // Size, in characters, of the unicode string
			nullptr,                 // (optional) Pointer to a buffer that receives the converted string
			0,                       // (optional) Size, in bytes, of the buffer indicated by previous arg
			nullptr,                 // (optional) Pointer to the character to use if a character cannot be represented in the specified code page
			nullptr                  // (optional) Pointer to a flag that indicates if the function has used a default character in the conversion
		);

		if (size_needed <= 0)
			throw std::runtime_error("WideCharToMultiByte() failed: " + std::to_string(size_needed));

		std::string result(size_needed, 0);
		WideCharToMultiByte(
			CP_UTF8, 
			0, 
			&wide_string.at(0), 
			(int)wide_string.size(), 
			&result.at(0), 
			size_needed, 
			nullptr, 
			nullptr
		);

		return result;
	}
}