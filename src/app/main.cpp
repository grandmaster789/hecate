#include <iostream>

#include "app/application.h"
#include "core/engine.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "platform/window.h"

using namespace hecate;

class HecateApplication :
	public Application,
	public MessageHandler<input::Keyboard::OnKeyPressed>,
	//public MessageHandler<input::Mouse::OnMoved>,
	public MessageHandler<input::Mouse::OnScroll>,
	public MessageHandler<input::Mouse::OnButtonPressed>,
	public MessageHandler<input::Mouse::OnDoubleClick>
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

	void operator()(const input::Mouse::OnMoved& mm) {
		g_Log << std::format("({}, {})", mm.m_X, mm.m_Y);
	}

	void operator()(const input::Mouse::OnScroll& os) {
		g_Log << "Scroll " << os.m_ScrollAmount;
	}

	void operator()(const input::Mouse::OnButtonPressed& bp) {
		g_Log << "Clicked " << bp.m_Button << " @(" << bp.m_X << ", " << bp.m_Y << ")";
	}

	void operator()(const input::Mouse::OnDoubleClick& dc) {
		g_Log << "Double " << dc.m_Button;
	}
};

int main() {
	auto& engine = Engine::instance();

	engine.set_application<HecateApplication>();
	engine.start();

	return 0;
}