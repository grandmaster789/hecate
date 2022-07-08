#pragma once

#include "../core/system.h"
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

		// WSI
		static bool message_pump();

		// Memory
		static void* allocate   (uint64_t num_bytes, bool aligned_storage);
		static void  deallocate (void* block, bool aligned_storage);
		static void* zero_memory(void* block, uint64_t num_bytes);
		static void* copy_memory(void* dest, const void* source, uint64_t num_bytes);
		static void* set_memory (void* dest, int32_t value, uint64_t num_bytes);

		// Filesystem

		// Clock
		static double get_absolute_time(); // time in seconds since starting the program
		static void   sleep_ms(uint64_t milliseconds);

		// Console 
		static void console_write(const char* message,       uint8_t color = 1);
		static void console_write_error(const char* message, uint8_t color = 1);
	};
}