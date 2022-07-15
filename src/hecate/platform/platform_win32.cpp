#include "platform.h"
#include "window.h"
#include "../dependencies.h"
#include "../core/engine.h"
#include "../input/mouse.h"
#include "../input/keyboard.h"
#include "../input/input.h"
#include "../util/flat_map.h"

#include <thread>
#include <chrono>

#if HECATE_PLATFORM != HECATE_PLATFORM_WINDOWS
	#error This is platform-specific, please remove it from the CMakeList so that it doesn't get compiled for any platform other than windows
#endif

namespace {
	double        g_ClockFrequency = 1;       // frequency in Hz
	LARGE_INTEGER g_StartTime      = {};      // 
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

		// setup timing
		{
			LARGE_INTEGER frequency;
			QueryPerformanceFrequency(&frequency);                            // https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancefrequency
			g_ClockFrequency = 1.0 / static_cast<double>(frequency.QuadPart);
			QueryPerformanceCounter(&g_StartTime);                            // https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancecounter
		}

		// create main window
		m_Windows.push_back(std::make_unique<platform::Window>(
			"Hecate",
			this,
			m_MainWindowWidth,
			m_MainWindowHeight,
			m_DisplayDeviceIdx
		));

		return true;
	}

	void Platform::update() {
		if (m_Windows.empty())
			m_Engine->stop();
		else {
			MSG msg = {};

			while (PeekMessage(
				&msg,
				nullptr,  // hWnd
				0,        // message filter min
				0,        // message filter max
				PM_REMOVE // remove mesasge from the queue, if any
			)) {
				if (msg.message == WM_QUIT)
					m_Engine->stop(); // allow any window to close the entire application

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	void Platform::shutdown() {
		System::shutdown();		
	}

	void Platform::close(platform::Window* window) {
		if (window->is_main_window()) {
			// save size/display information
			m_MainWindowWidth  = window->get_width();
			m_MainWindowHeight = window->get_height();
			m_DisplayDeviceIdx = window->get_display_device_idx();
		}

		std::erase_if(
			m_Windows, 
			[window](const auto& ptr) { 
				return ptr.get() == window;
			}
		);
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
}