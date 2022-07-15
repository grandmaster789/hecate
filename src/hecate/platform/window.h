#pragma once

#include <string>
#include <memory>

namespace hecate {
	class Platform;

	namespace input {
		class Mouse;
		class Keyboard;
	}

	namespace platform {
		class Window {
		public:
			using Mouse    = input::Mouse;
			using Keyboard = input::Keyboard;

			Window(
				const std::string& title,
				Platform*          owner,
				int                width              = -1, // use -1 for default
				int                height             = -1, // use -1 for default
				int                display_device_idx = 0
			);

			Window             (const Window&)     = delete;
			Window& operator = (const Window&)     = delete;
			Window             (Window&&) noexcept = default;
			Window& operator = (Window&&) noexcept = default;

			[[nodiscard]] bool is_main_window() const noexcept;
			void close();

			[[nodiscard]] int get_width()              const noexcept;
			[[nodiscard]] int get_height()             const noexcept;
			[[nodiscard]] int get_display_device_idx() const noexcept;

			[[nodiscard]]       Keyboard* get_keyboard()       noexcept;
			[[nodiscard]] const Keyboard* get_keyboard() const noexcept;
			[[nodiscard]]       Mouse*    get_mouse()          noexcept;
			[[nodiscard]] const Mouse*    get_mouse()    const noexcept;

		private:
			int   m_Width            = 800;
			int   m_Height           = 600;
			int   m_DisplayDeviceIdx = 0;
			float m_Dpi              = 96.0f; // horizontal DPI

			void*               m_NativeHandle = nullptr;
			inline static void* s_MainWindow   = nullptr;

			std::unique_ptr<Keyboard> m_Keyboard;
			std::unique_ptr<Mouse>    m_Mouse;

			Platform* m_Owner = nullptr;
		};
	}
}