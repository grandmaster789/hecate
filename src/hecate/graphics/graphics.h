#pragma once

#include <memory>

#include "../dependencies.h"
#include "../core/system.h"

namespace hecate::graphics {
	// Subsystem responsible for handling graphics resources
	// (this is the Vulkan interface)
	//
	class Graphics :
		public core::System
	{
	public:
		Graphics();

		void init() override;
		void update() override;
		void shutdown() override;

		const vk::Instance& get_vk_instance(() const noexcept;

	private:
		void init_vulkan();

		vk::UniqueInstance m_VkInstance;
	};
}