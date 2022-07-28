#include "window.h"
#include "platform_strings.h"
#include "platform.h"

#include "../preprocessor.h"
#include "../core/engine.h"
#include "../input/input.h"
#include "../input/mouse.h"
#include "../input/keyboard.h"
#include "../util/flat_map.h"
#include "../util/algorithm.h"

#include <format>

#if HECATE_PLATFORM != HECATE_PLATFORM_WINDOWS
	#error This cpp is specific to windows
#endif

namespace {
	hecate::util::FlatMap<WPARAM, hecate::input::Keyboard::e_Key> g_KeyMapping;

	static constexpr const wchar_t* k_HecateWindowClassName = L"hecate_window_class";

	void populate_keymapping() {
		using eKey = hecate::input::Keyboard::e_Key;

		// VK_ macros (virtual key codes) can be found in <winuser.h>
		// https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes

		// letters * VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
		g_KeyMapping.assign('A', eKey::a);
		g_KeyMapping.assign('B', eKey::b);
		g_KeyMapping.assign('C', eKey::c);
		g_KeyMapping.assign('D', eKey::d);

		g_KeyMapping.assign('E', eKey::e);
		g_KeyMapping.assign('F', eKey::f);
		g_KeyMapping.assign('G', eKey::g);
		g_KeyMapping.assign('H', eKey::h);

		g_KeyMapping.assign('I', eKey::i);
		g_KeyMapping.assign('J', eKey::j);
		g_KeyMapping.assign('K', eKey::k);
		g_KeyMapping.assign('L', eKey::l);

		g_KeyMapping.assign('M', eKey::m);
		g_KeyMapping.assign('N', eKey::n);
		g_KeyMapping.assign('O', eKey::o);
		g_KeyMapping.assign('P', eKey::p);

		g_KeyMapping.assign('Q', eKey::q);
		g_KeyMapping.assign('R', eKey::r);
		g_KeyMapping.assign('S', eKey::s);
		g_KeyMapping.assign('T', eKey::t);

		g_KeyMapping.assign('U', eKey::u);
		g_KeyMapping.assign('V', eKey::v);
		g_KeyMapping.assign('W', eKey::w);
		g_KeyMapping.assign('X', eKey::x);

		g_KeyMapping.assign('Y', eKey::y);
		g_KeyMapping.assign('Z', eKey::z);

		// numbers *.assign(_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
		g_KeyMapping.assign('1', eKey::_1);
		g_KeyMapping.assign('2', eKey::_2);
		g_KeyMapping.assign('3', eKey::_3);
		g_KeyMapping.assign('4', eKey::_4);

		g_KeyMapping.assign('5', eKey::_5);
		g_KeyMapping.assign('6', eKey::_6);
		g_KeyMapping.assign('7', eKey::_7);
		g_KeyMapping.assign('8', eKey::_8);

		g_KeyMapping.assign('9', eKey::_9);
		g_KeyMapping.assign('0', eKey::_0);

		// function keys
		g_KeyMapping.assign(VK_F1, eKey::f1);
		g_KeyMapping.assign(VK_F2, eKey::f2);
		g_KeyMapping.assign(VK_F3, eKey::f3);
		g_KeyMapping.assign(VK_F4, eKey::f4);

		g_KeyMapping.assign(VK_F5, eKey::f5);
		g_KeyMapping.assign(VK_F6, eKey::f6);
		g_KeyMapping.assign(VK_F7, eKey::f7);
		g_KeyMapping.assign(VK_F8, eKey::f8);

		g_KeyMapping.assign(VK_F9,  eKey::f9);
		g_KeyMapping.assign(VK_F10, eKey::f10);
		g_KeyMapping.assign(VK_F11, eKey::f11);
		g_KeyMapping.assign(VK_F12, eKey::f12);

		// symbol keys (assuming US standard keyboard)
		g_KeyMapping.assign(VK_OEM_1, eKey::semicolon);
		g_KeyMapping.assign(VK_OEM_2, eKey::questionmark);
		g_KeyMapping.assign(VK_OEM_3, eKey::tilde);
		g_KeyMapping.assign(VK_OEM_4, eKey::brace_open);
		g_KeyMapping.assign(VK_OEM_5, eKey::vertical_pipe);
		g_KeyMapping.assign(VK_OEM_6, eKey::brace_close);
		g_KeyMapping.assign(VK_OEM_7, eKey::double_quote);
		g_KeyMapping.assign(VK_OEM_8, eKey::oem_8);

		g_KeyMapping.assign(VK_OEM_PLUS,   eKey::plus);
		g_KeyMapping.assign(VK_OEM_MINUS,  eKey::minus);
		g_KeyMapping.assign(VK_OEM_COMMA,  eKey::comma);
		g_KeyMapping.assign(VK_OEM_PERIOD, eKey::period);

		// navigational keys
		g_KeyMapping.assign(VK_UP,    eKey::up_arrow);
		g_KeyMapping.assign(VK_DOWN,  eKey::down_arrow);
		g_KeyMapping.assign(VK_LEFT,  eKey::left_arrow);
		g_KeyMapping.assign(VK_RIGHT, eKey::right_arrow);

		g_KeyMapping.assign(VK_PRIOR, eKey::pg_up);
		g_KeyMapping.assign(VK_NEXT,  eKey::pg_down);
		g_KeyMapping.assign(VK_HOME,  eKey::home);
		g_KeyMapping.assign(VK_END,   eKey::end);

		g_KeyMapping.assign(VK_INSERT, eKey::ins);
		g_KeyMapping.assign(VK_DELETE, eKey::del);

		// everything else
		g_KeyMapping.assign(VK_CONTROL, eKey::ctrl);
		g_KeyMapping.assign(VK_MENU,    eKey::alt);
		g_KeyMapping.assign(VK_SHIFT,   eKey::shift);
		g_KeyMapping.assign(VK_SPACE,   eKey::space);

		g_KeyMapping.assign(VK_TAB,    eKey::tab);
		g_KeyMapping.assign(VK_RETURN, eKey::enter);
		g_KeyMapping.assign(VK_ESCAPE, eKey::escape);
		g_KeyMapping.assign(VK_BACK,   eKey::backspace);
	}

	auto find_key_code(WPARAM wp) {
		if (auto* result = g_KeyMapping[wp])
			return *result;
		else
			return hecate::input::Keyboard::e_Key::undefined;
	}

	LRESULT CALLBACK defer_win_proc(
		HWND   window_handle,
		UINT   msg,
		WPARAM wp,
		LPARAM lp
	) {
		using eMouseButton = hecate::input::Mouse::e_Button;

		// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowlongptra
		LONG_PTR userdata = GetWindowLongPtr(window_handle, GWLP_USERDATA);

		if (userdata == NULL)
			return DefWindowProc(window_handle, msg, wp, lp);

		auto* win = std::bit_cast<hecate::platform::Window*>(userdata);

		switch (msg) {
		case WM_DESTROY: {
				SetWindowLongPtr(window_handle, GWLP_USERDATA, NULL); // remove object association to prevent cascades
				win->close();
			}
			break;

		case WM_CREATE: {} // re-do layouting?
			break;

		// https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-nccalcsize
		case WM_NCCALCSIZE: {}
			break;

		case WM_DPICHANGED: {
				// re-do layouting!
			}
			break;

		case WM_SIZE: {
				RECT rect;
				GetClientRect(window_handle, &rect);
				
				int32_t width  = rect.right  - rect.left;
				int32_t height = rect.bottom - rect.top;

				win->update_size(width, height);
			}
			break;

		// NOTE this is the 'legacy' approach; we might use RawInput in the future
		case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN: 
			{
				using eKey = hecate::input::Keyboard::e_Key;

				bool pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
				bool is_extended = (HIWORD(lp) & KF_EXTENDED) == KF_EXTENDED;

				eKey key = eKey::undefined;

				// special case to determine left/right alt
				if (wp == VK_MENU) {
					if (is_extended)
						key = eKey::right_alt;
					else
						key = eKey::left_alt;
				}

				// special case for left/right control
				if (wp == VK_CONTROL) {
					if (is_extended)
						key = eKey::right_ctrl;
					else
						key = eKey::left_ctrl;
				}

				// extra special case for left/right shift
				if (wp == VK_SHIFT) {

					// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-mapvirtualkeya
					uint32_t left_shift = MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC);
					uint32_t scancode = ((lp & (0xFF << 16)) >> 16);

					if (scancode == left_shift)
						key = eKey::left_shift;
					else
						key = eKey::right_shift;
				}

				if (key == eKey::undefined)
					key = find_key_code(wp);

				win->get_keyboard()->set_state(key, pressed, win);
			} 
			break;
		
		//https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-mousemove
		case WM_MOUSEMOVE: {
				int32_t x_position = GET_X_LPARAM(lp);
				int32_t y_position = GET_Y_LPARAM(lp);

				// position is relative to upper-left corner of the client area
				// -- rescale it so that it's -1.0f..1.0f range and becomes Y-up
				float window_width  = static_cast<float>(win->get_width());
				float window_height = static_cast<float>(win->get_height());

				float relative_x = ( 2.0f * x_position / window_width)  - 1.0f;
				float relative_y = (-2.0f * y_position / window_height) + 1.0f;

				win->get_mouse()->set_position(
					relative_x, 
					relative_y
				);
			}
			break;

		// https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-mousewheel
		case WM_MOUSEWHEEL: {
				int32_t delta_z = GET_WHEEL_DELTA_WPARAM(wp);

				// different OSes may report this differently, so just interpret it as -1, 0 and +1
				if (delta_z != 0) {
					delta_z = std::clamp(delta_z, -1, 1);

					win->get_mouse()->do_scroll(delta_z);
				}
			}
			break;

		case WM_LBUTTONDOWN:
			win->get_mouse()->set_state(eMouseButton::left, true);
			break;

		case WM_LBUTTONUP:
			win->get_mouse()->set_state(eMouseButton::left, false);
			break;

		case WM_LBUTTONDBLCLK:
			win->get_mouse()->do_double_click(eMouseButton::left);
			break;

		case WM_MBUTTONDOWN:
			win->get_mouse()->set_state(eMouseButton::middle, true);
			break;

		case WM_MBUTTONUP:
			win->get_mouse()->set_state(eMouseButton::middle, false);
			break;

		case WM_MBUTTONDBLCLK:
			// pretty exotic!
			win->get_mouse()->do_double_click(eMouseButton::middle);
			break;

		case WM_RBUTTONDOWN:
			win->get_mouse()->set_state(eMouseButton::right, true);
			break;

		case WM_RBUTTONUP:
			win->get_mouse()->set_state(eMouseButton::right, false);
			break;

		case WM_RBUTTONDBLCLK:
			// fancy :P
			win->get_mouse()->do_double_click(eMouseButton::right);
			break;

			// how do many button mice messages work?
		}
		
		return DefWindowProc(window_handle, msg, wp, lp);
	}

	class HecateWindowClass {
	public:
		static HecateWindowClass& instance() {
			static HecateWindowClass x;
			return x;
		}

		const WNDCLASSEX& get() const {
			return m_WindowClass;
		}

	private:
		HecateWindowClass() {
			auto application_handle = GetModuleHandle(nullptr);

			m_WindowClass.cbSize        = sizeof(WNDCLASSEX);
			m_WindowClass.style         = CS_DBLCLKS;                                    // https://docs.microsoft.com/en-us/windows/win32/winmsg/window-class-styles
			m_WindowClass.lpfnWndProc   = defer_win_proc;                                // A pointer to the window procedure
			m_WindowClass.cbClsExtra    = 0;                                             // The number of extra bytes to allocate following the window-class structure
			m_WindowClass.cbWndExtra    = 0;                                             // The number of extra bytes to allocate following the window instance
			m_WindowClass.hInstance     = GetModuleHandle(NULL);                         // A handle to the instance that contains the window procedure for the class
			m_WindowClass.hIcon         = LoadIcon(application_handle, IDI_APPLICATION); // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadicona
			m_WindowClass.hIconSm       = LoadIcon(application_handle, IDI_WINLOGO);     // A handle to a small icon that is associated with the window class
			m_WindowClass.hCursor       = LoadCursor(application_handle, IDC_ARROW);     // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadcursora
			m_WindowClass.hbrBackground = nullptr;                                       // A handle to the class background brush
			m_WindowClass.lpszMenuName  = nullptr;                                       // The resource name of the class menu, as the name appears in the resource file
			m_WindowClass.lpszClassName = k_HecateWindowClassName;                       // A pointer to a null-terminated string or an atom

			if (RegisterClassEx(&m_WindowClass) == 0)
				throw std::runtime_error("Failed to register WindowClass");
		}

		~HecateWindowClass() {
			UnregisterClass(
				m_WindowClass.lpszClassName, 
				GetModuleHandle(NULL)
			);
		}

		WNDCLASSEX m_WindowClass = {}; // https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassexa
	};
	
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
					hecate::platform::to_string(x.DeviceString),
					hecate::platform::to_string(x.DeviceID)
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

namespace hecate::platform {
	Window::Window(
		const std::string& title, // NOTE this also becomes the window name, and vice versa
		Platform*          owner,
		int                width,
		int                height,
		int                display_device_idx
	):
		m_Owner           (owner),
		m_Width           (width),
		m_Height          (height),
		m_DisplayDeviceIdx(display_device_idx)
	{
		if (g_KeyMapping.is_empty())
			populate_keymapping();

		auto display_devices = enumerate_display_devices();
		if (display_devices.empty())
			throw std::runtime_error("No display devices are available");

		if (display_devices.size() < display_device_idx) {
			g_LogWarning << std::format("Display {} unavailable, falling back to primary display", display_device_idx);
			m_DisplayDeviceIdx = 0;

			list_display_devices(); // might as well show the current set of valid options
		}

		const auto& wc = HecateWindowClass::instance().get();

		DWORD window_style =
			WS_CLIPSIBLINGS |
			WS_CLIPCHILDREN |
			WS_SYSMENU |
			WS_MINIMIZEBOX;

		DWORD window_style_ex =
			WS_EX_APPWINDOW;

		if (width == -1)
			m_Width = CW_USEDEFAULT;
		if (height == -1)
			m_Height = CW_USEDEFAULT;

		// we'll adjust position at a later stage
		// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexa
		m_NativeHandle = CreateWindowEx(
			window_style_ex,
			wc.lpszClassName,
			platform::to_wstring(title.data()).c_str(),
			window_style,
			CW_USEDEFAULT,         // x (left)
			CW_USEDEFAULT,         // y (top)
			m_Width,               // width 
			m_Height,              // height
			nullptr,               // parent window handle
			nullptr,               // menu handle
			GetModuleHandle(NULL), // program instance
			nullptr                // lpParam
		);

		if (!m_NativeHandle) {
			MessageBox(nullptr, L"Window creation failed", L"Error", MB_ICONEXCLAMATION | MB_OK);

			throw std::runtime_error("Failed to create window");
		}
		
		// put a pointer to this Platform object in the Window userdata
		SetWindowLongPtr(
			reinterpret_cast<HWND>(m_NativeHandle), 
			GWLP_USERDATA, 
			std::bit_cast<LONG_PTR>(this)
		);
		
		// extend the client (drawable) area across the titlebar + border
		MARGINS margins{ 0, 0, 0, 1 };
		DwmExtendFrameIntoClientArea(
			reinterpret_cast<HWND>(m_NativeHandle), 
			&margins
		); // https://docs.microsoft.com/en-us/windows/win32/api/dwmapi/nf-dwmapi-dwmextendframeintoclientarea

		// determine DPI
		{
			UINT value = GetDpiForWindow((HWND)m_NativeHandle);
			if (value == 0) {
				MessageBox(nullptr, L"Failed to get DPI", L"Error", MB_ICONEXCLAMATION | MB_OK);
				throw std::runtime_error("DPI");
			}

			m_Dpi = static_cast<float>(value);
		}

		// extract virtual desktop coordinates for the indicated display device
		{
			auto display_mode = get_current_displaymode(display_devices[m_DisplayDeviceIdx]);

			int display_virtual_x      = display_mode.dmPosition.x;
			int display_virtual_y      = display_mode.dmPosition.y;
			int display_virtual_width  = display_mode.dmPelsWidth;
			int display_virtual_height = display_mode.dmPelsHeight;

			RECT rect;
			GetClientRect((HWND)m_NativeHandle, &rect);

			m_Width  = rect.right  - rect.left;
			m_Height = rect.bottom - rect.top;

			// center the window
			rect.left   = display_virtual_x + (display_virtual_width  - m_Width)  / 2;
			rect.top    = display_virtual_y + (display_virtual_height - m_Height) / 2;
			rect.right  = rect.left + m_Width;
			rect.bottom = rect.top  + m_Height;

			// adjust window coordinates, set window size, trigger WM_NCCALCSIZE
			// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowpos
			SetWindowPos(
				(HWND)m_NativeHandle,
				HWND_TOP,			     // insertAfter
				rect.left,               // X (left)
				rect.top,                // Y (top)
				rect.right  - rect.left, // cx (width in pixels)
				rect.bottom - rect.top,  // cy (height in pixels)				
				SWP_FRAMECHANGED         // send WM_NCCALCSIZE even if the actual size isn't changed
			);

			ShowWindow((HWND)m_NativeHandle, SW_SHOW);
		}

		// some debug output
		{
			RECT window_rect;
			GetWindowRect((HWND)m_NativeHandle, &window_rect);

			RECT client_rect;
			GetClientRect((HWND)m_NativeHandle, &client_rect);

			g_Log << std::format(
				"({}x{}) Window created at ({}, {}); Client rect is {}x{}",
				window_rect.right  - window_rect.left,
				window_rect.bottom - window_rect.top,
				window_rect.left,
				window_rect.top,
				client_rect.right - client_rect.left,
				client_rect.bottom - client_rect.top
			);
		}

		// setup virtual keyboard/mouse for this window
		{
			auto* x    = Engine::instance().get<Input>();
			m_Keyboard = std::make_unique<Keyboard>(x);
			m_Mouse    = std::make_unique<Mouse>(x);
		}
	}

	Window::~Window() {
		if (m_NativeHandle) {
			DestroyWindow(reinterpret_cast<HWND>(m_NativeHandle));
			m_NativeHandle = nullptr;
		}
	}

	bool Window::is_main_window() const noexcept {
		return (s_MainWindow == m_NativeHandle);
	}

	bool Window::is_minimized() const noexcept {
		return (IsIconic((HWND)m_NativeHandle) == TRUE);
	}

	int Window::get_width()  const noexcept {
		return m_Width;
	}

	int Window::get_height() const noexcept {
		return m_Height;
	}

	int Window::get_display_device_idx() const noexcept {
		return m_DisplayDeviceIdx;
	}

	void Window::update_size(int new_width, int new_height) {
		m_Width  = new_width;
		m_Height = new_height;
	}

	void Window::set_size(int new_width, int new_height) {
		RECT rect;
		GetClientRect((HWND)m_NativeHandle, &rect);

		// keep the same center
		int center_x = rect.left + rect.right / 2;
		int center_y = rect.top + rect.bottom / 2;

		MoveWindow(
			(HWND)m_NativeHandle,
			center_x - new_width / 2,
			center_y - new_height / 2,
			new_width,
			new_height,
			true // repaint
		);
	}

	void Window::set_position(int new_x, int new_y) {
		RECT rect;
		GetClientRect((HWND)m_NativeHandle, &rect);

		// keep the same width/height;
		int width  = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		MoveWindow(
			(HWND)m_NativeHandle,
			new_x,
			new_y,
			width,
			height,
			true // repaint
		);
	}

	void Window::close() {
		m_Owner->close(this);
	}

	void Window::maximize() {
		ShowWindow((HWND)m_NativeHandle, SW_MAXIMIZE);
	}

	void Window::minimize() {
		ShowWindow((HWND)m_NativeHandle, SW_MINIMIZE);
	}

	void Window::restore() {
		ShowWindow((HWND)m_NativeHandle, SW_RESTORE);
	}

	Window::Keyboard* Window::get_keyboard() noexcept {
		return m_Keyboard.get();
	}

	const Window::Keyboard* Window::get_keyboard() const noexcept {
		return m_Keyboard.get();
	}

	Window::Mouse* Window::get_mouse() noexcept {
		return m_Mouse.get();
	}

	const Window::Mouse* Window::get_mouse() const noexcept {
		return m_Mouse.get();
	}
}