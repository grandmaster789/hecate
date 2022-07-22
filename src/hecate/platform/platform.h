#pragma once

#include "../core/system.h"
#include "../dependencies.h"
#include <cstdint>

namespace hecate {
	namespace input {
		class Keyboard;
		class Mouse;
	}

	namespace platform {
		class Window;

		// DLL
		void*           load_dynamic_library(const char* name);
		void            unload_dynamic_library(void* handle);
		DynamicFunction get_dynamic_symbol(void* module_handle, const char* function_name);
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

		// WSI
		void close(platform::Window* window);

		// Filesystem

		// Clock
		static double get_absolute_time(); // time in seconds since starting the program
		static void   sleep_ms(uint64_t milliseconds);

		// Vulkan
		static std::vector<const char*> get_required_vulkan_extensions();

	private:
		using WindowPtr = std::unique_ptr<platform::Window>;

		// persistent variables
		int m_MainWindowWidth  = -1; // use -1 for OS default
		int m_MainWindowHeight = -1; // use -1 for OS default
		int m_DisplayDeviceIdx = 0;

		std::vector<WindowPtr> m_Windows;
	};
}