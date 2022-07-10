#include "system.h"
#include "engine.h"
#include "../util/algorithm.h"
#include "logger.h"

namespace hecate {
	System::System(const std::string& unique_system_name):
		m_Name(unique_system_name)
	{
	}
	
	bool System::init() {
		g_Log << "Starting subsystem " << get_name();

		return true;
	}

	void System::update() {
	}

	void System::shutdown() {
		g_Log << "Stopped subsystem " << get_name();
	}

	const std::string& System::get_name() const {
		return m_Name;
	}

	const System::Dependencies& System::get_dependencies() const {
		return m_Dependencies;
	}

	const System::Settings& System::get_settings() const {
		return m_Settings;
	}

	void System::add_dependency(const std::string& system_name) {
		if (!util::contains(m_Dependencies, system_name))
			m_Dependencies.push_back(std::string(system_name));
		else
			g_Log << "Ignoring duplicate dependency: " << system_name;
	}

	void System::operator()(const RequestShutdown& req) {
		if (req.m_System == this)
			shutdown();
	}

	std::ostream& operator << (std::ostream& os, const System& s) {
		os << s.get_name();
		return os;
	}
}