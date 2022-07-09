#pragma once

#include "../core/system.h"
#include "../dependencies.h"
#include <cstdint>

namespace hecate::platform {
	//
	// This is responsible for platform-specific stuff, WSI etc
	// The idea is to have this be the interface, while using CMake to differentiate which cpp to implement it
	// 
	class Platform :
		public core::System
	{
	public:
		Platform();

		bool init()     override;
		void update()   override;
		void shutdown() override;

		// Filesystem

		// Clock
		static double get_absolute_time(); // time in seconds since starting the program
		static void   sleep_ms(uint64_t milliseconds);

	private:
		// persistent variables
		int  m_MainWindowWidth  = 800;
		int  m_MainWindowHeight = 600;
		bool m_Fullscreen       = false;
		int  m_DisplayDeviceIdx = 0;
	};
}

namespace hecate {
	std::wstring to_wstring(const std::string& string);
	std::string  to_string(const std::wstring& wide_string);
}