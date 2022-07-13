#pragma once

#include "../core/system.h"

#include <vector>

namespace hecate::input {
    class Keyboard;
    class Mouse;
}

namespace hecate {
    class Input: 
        public System {
    public:
        using Keyboard = input::Keyboard;
        using Mouse    = input::Mouse;

        Input();

        bool init()     override;
        void update()   override; // NOTE some input devices may need polling, which can be done here
        void shutdown() override;

        void register_device  (Keyboard* kbd);
        void unregister_device(Keyboard* kbd);

        void register_device  (Mouse* m);
        void unregister_device(Mouse* m);

        const Keyboard& get_keyboard() const; // typically the system will only have one keyboard
        const Mouse&    get_mouse()    const; // typically the system will only have one mouse

        const std::vector<Keyboard*>& get_keyboards() const;
        const std::vector<Mouse*>&    get_mice()      const;

    private:
        // NOTE these are non-owning; the owner is the Platform object
        std::vector<Keyboard*> m_Keyboards;
        std::vector<Mouse*>    m_Mice;
    };
}