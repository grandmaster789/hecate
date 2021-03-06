#pragma once

#include <memory>

#include "../dependencies.h"
#include "../core/system.h"

namespace hecate {
	// Subsystem responsible for handling graphics resources
	// (this is the Vulkan interface)
	//
	class Graphics :
		public System
	{
	public:
		Graphics();

		bool init()     override;
		void update()   override;
		void shutdown() override;

		const vk::Instance& get_vk_instance() const noexcept;

	private:
		void init_vulkan();

		vk::UniqueInstance m_VkInstance;
	};
}