#include <iostream>

#include "app/application.h"
#include "core/engine.h"

class HecateApplication :
	public hecate::Application
{
public:
	HecateApplication():
		Application("Hecate")
	{
	}

	bool init() override {
		g_Log << "Initializing application";
		return true;
	}

	void update() override {
	}

	void shutdown() override {
		g_Log << "Application shutting down";
	}
};

int main() {
	auto& engine = hecate::Engine::instance();

	engine.set_application<HecateApplication>();
	engine.start();

	return 0;
}