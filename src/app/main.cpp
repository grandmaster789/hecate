#include <iostream>

#include "app/application.h"
#include "core/engine.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "platform/window.h"

using namespace hecate;

class HecateApplication :
	public Application,
	public MessageHandler<input::Keyboard::OnKeyPressed>
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

	void operator()(const input::Keyboard::OnKeyPressed& kp) {
		if (kp.key == input::Keyboard::e_Key::escape)
			kp.win->close();

		g_Log << "Key pressed: " << kp.key;
	}
};

int main() {
	auto& engine = Engine::instance();

	engine.set_application<HecateApplication>();
	engine.start();

	return 0;
}