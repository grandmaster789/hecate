#include "graphics.h"

namespace hecate::graphics {
	Graphics::Graphics():
		System("Graphics")
	{
	}

	void Graphics::init() {

	}
	void Graphics::update() {
		
	}

	void Graphics::shutdown() {

	}

	const vk::Instance& Graphics::get_vk_instance() const noexcept {
		return m_VkInstance.get();
	}

	void Graphics::init_vulkan() {

	}
}