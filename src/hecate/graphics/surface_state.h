#pragma once

#include <iosfwd>

namespace hecate::graphics {
	enum class e_SurfaceState {
		no_window,
		has_window,
		has_device,
		has_swapchain
	};

	enum class e_SurfaceLost {
		not_lost,
		swapchain_lost,
		device_lost,
		window_lost
	};

	std::ostream& operator << (std::ostream& os, const e_SurfaceState& ss);
	std::ostream& operator << (std::ostream& os, const e_SurfaceLost& sl);
}