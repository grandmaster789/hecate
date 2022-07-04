#include "surface_state.h"

#include <ostream>

namespace hecate::graphics {
	std::ostream& operator << (std::ostream& os, const e_SurfaceState& ss) {
		switch (ss) {
		case e_SurfaceState::no_window:     os << "No window";     break;
		case e_SurfaceState::has_window:    os << "Has window";    break;
		case e_SurfaceState::has_device:    os << "Has device";    break;
		case e_SurfaceState::has_swapchain: os << "Has swapchain"; break;
		default:
			os << "[UNKNOWN]";
		}

		return os;
	}

	std::ostream& operator << (std::ostream& os, const e_SurfaceLost& sl) {
		switch (sl) {
		case e_SurfaceLost::not_lost:       os << "Not lost";       break;
		case e_SurfaceLost::window_lost:    os << "Window lost";    break;
		case e_SurfaceLost::device_lost:    os << "Device lost";    break;
		case e_SurfaceLost::swapchain_lost: os << "Swapchain lost"; break;
		default:
			os << "[UNKNOWN]";
		}

		return os;
	}
}