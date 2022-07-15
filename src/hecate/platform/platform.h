#pragma once

#include "../core/system.h"
#include "../dependencies.h"
#include <cstdint>

namespace hecate {
	namespace input {
		class Keyboard;
		class Mouse;
	}

	//
	// This is responsible for platform-specific stuff, WSI etc
	// The idea is to have this be the interface, while using CMake to differentiate which cpp to implement it
	// 
	// NOTE there is some bundling of responsibilities here, might be better to factor some stuff out
	// 
	class Platform:
		public System
	{
	public:
		Platform();

		bool init()     override;
		void update()   override;
		void shutdown() override;

		unsigned int get_dpi() const noexcept; // only valid after the window is created

		// Filesystem

		// HID
		      input::Keyboard* get_keyboard();
		const input::Keyboard* get_keyboard() const;
		      input::Mouse*    get_mouse();
		const input::Mouse*    get_mouse() const;

		// Clock
		static double get_absolute_time(); // time in seconds since starting the program
		static void   sleep_ms(uint64_t milliseconds);

		// Vulkan
		static std::vector<const char*> get_required_vulkan_extensions();

	private:
		// persistent variables
		int m_MainWindowWidth  = -1; // use -1 for OS default
		int m_MainWindowHeight = -1; // use -1 for OS default
		int m_DisplayDeviceIdx = 0;

		// input devices (respond when the window is active)
		std::unique_ptr<input::Keyboard> m_Keyboard;
		std::unique_ptr<input::Mouse>    m_Mouse;
	};

	// NOTE the standard library implementation got deprecated in c++17 without a replacement (!), 
	//      so we'll have to depend on OS-level facilities instead...
	std::wstring to_wstring(const std::string& string);
	std::string  to_string(const std::wstring& wide_string);
}