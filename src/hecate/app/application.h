#pragma once

#include "../core/system.h"

namespace hecate::app {
	class Application:
		public core::System
	{
	public:
		explicit Application(const std::string& name);

		virtual void init()     = 0;
		virtual void update()   = 0;
		virtual void shutdown() = 0;
	};

	template <typename T>
	concept c_Application = std::derived_from<T, Application>;
}