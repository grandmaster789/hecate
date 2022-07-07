#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <functional>
#include <iostream>
#include <type_traits>

#include "../dependencies.h"

#include "mediator.h"

namespace hecate::app {
	class Application;
}

namespace hecate::core {
	/*
	*	Current Subsystem features:
	*		Engine-managed initialization/shutdown including inter-system dependency management
	*		Engine-managed updating of running subsystems
	*		Per-subsystem settings via combined 'hecate.json'
	*		
	*	Considerations:
	*		Per-system logs? currently everything uses a global log though
	*/

	struct RequestShutdown { class System* m_System = nullptr; };

	class System:
		public MessageHandler<RequestShutdown>
	{
	private:
		struct JsonProperties {
			std::string                                m_VariableName;
			std::function<nlohmann::json()>            m_GetFn;
			std::function<void(const nlohmann::json&)> m_SetFn;
		};

	public:
		friend class Engine;
		friend class Settings;

		using Dependencies = std::vector<std::string>; // based on System names (which must be unique)
		using Settings = std::vector<JsonProperties>;

		explicit System(const std::string& unique_system_name);
		virtual ~System() = default;

		// no-copy, move-only semantics
		System             (const System&) = delete;
		System& operator = (const System&) = delete;
		System             (System&&) noexcept = default;
		System& operator = (System&&) noexcept = default;

		virtual void init();
		virtual void update();
		virtual void shutdown(); // prepare for cleanup - finalize settings to be serialized etc

		const std::string& get_name() const;

		const Dependencies& get_dependencies()     const;
		const Settings&     get_settings()         const;
		
		void operator()(const RequestShutdown& req);

		friend std::ostream& operator << (std::ostream& os, const System& s);

	protected:
		void add_dependency(const std::string& system_name);

		template <typename T>
		void register_setting(
			const std::string& json_key, 
			T*                 member_variable
		);

		class Engine* m_Engine = nullptr;

	private:
		std::string  m_Name;
		Dependencies m_Dependencies;
		Settings     m_Settings;		
	};

	template <typename T>
	concept c_System = 
		 std::derived_from<T, System> &&
		!std::derived_from<T, app::Application>;
}

#include "system.inl"