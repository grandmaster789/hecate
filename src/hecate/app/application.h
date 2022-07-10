#pragma once

#include "../core/system.h"

namespace hecate {
	class Application:
		public System
	{
	public:
		explicit Application(const std::string& name);

		virtual bool init()     = 0;
		virtual void update()   = 0;
		virtual void shutdown() = 0;
	};

	template <typename T>
	concept c_Application = std::derived_from<T, Application>;
}