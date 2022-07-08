#include "graphics.h"

namespace hecate::graphics {
	Graphics::Graphics():
		System("Graphics")
	{
		add_dependency("Platform"); // needs Platform for WSI
	}

	bool Graphics::init() {
		System::init();

		return true;
	}

	void Graphics::update() {
		
	}

	void Graphics::shutdown() {
		System::shutdown();
	}

	const vk::Instance& Graphics::get_vk_instance() const noexcept {
		return m_VkInstance.get();
	}

	void Graphics::init_vulkan() {

	}
}