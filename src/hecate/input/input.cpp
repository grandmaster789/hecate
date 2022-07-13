#include "input.h"
#include "../util/algorithm.h"
#include "../core/logger.h"

namespace hecate {
    Input::Input(): 
        System("Input") 
    {
    }

    bool Input::init() {
        System::init();

        return true;
    }

    void Input::update() {
    }

    void Input::shutdown() {
        System::shutdown();
    }

    void Input::register_device(Keyboard* kbd) {
        if (!util::contains(m_Keyboards, kbd))
            m_Keyboards.push_back(kbd);
        else
            g_Log << "Duplicate keyboard registration, discarding...\n";
    }

    void Input::unregister_device(Keyboard* kbd) {
        auto it = util::find(m_Keyboards, kbd);

        if (it != std::end(m_Keyboards))
            m_Keyboards.erase(it);
        else
            g_Log << "Cannot unregister unlisted keyboard, ignoring...";
    }

    void Input::register_device(Mouse* m) {
        if (!util::contains(m_Mice, m))
            m_Mice.push_back(m);
        else
            g_Log << "Duplicate mouse registration, discarding...";
    }

    void Input::unregister_device(Mouse* m) {
        auto it = util::find(m_Mice, m);

        if (it != std::end(m_Mice))
            m_Mice.erase(it);
        else
            g_Log << "Cannot unregister unlisted mouse, ignoring...";
    }

    const std::vector<Input::Keyboard*>& Input::get_keyboards() const {
        return m_Keyboards;
    }

    const std::vector<Input::Mouse*>& Input::get_mice() const {
        return m_Mice;
    }

    const Input::Keyboard& Input::get_keyboard() const {
        return *m_Keyboards.front();
    }

    const Input::Mouse& Input::get_mouse() const {
        return *m_Mice.front();
    }
}