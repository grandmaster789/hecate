#pragma once

#include <iosfwd>
#include <utility>
#include <array>

namespace hecate {
    class Input;

    namespace input {
        // [TODO] when I've got some math basics in place, the XY stuff should be some
        //        kind of point struct (plus directions perhaps?)
        // [TODO] do I want dragging support?

        // [NOTE] X/Y coordinates are normalized to [-1..1] within the associated window

        class Mouse {
        public:
            enum class e_Button {
                left,
                right,
                middle
            };

            Mouse(Input* manager);
            ~Mouse();

            Mouse             (const Mouse&) = delete;
            Mouse& operator = (const Mouse&) = delete;
            Mouse             (Mouse&&)      = delete;
            Mouse& operator = (Mouse&&)      = delete;

            bool is_down(e_Button button) const;
            bool is_up  (e_Button button) const;

            std::pair<float, float> get_position() const;

            // These set the state and broadcast the appropriate event
            void set_state(e_Button button, bool pressed);
            void set_position(float x, float y);

            void do_double_click(e_Button button);
            void do_scroll(int amount);
            void do_enter_window();
            void do_leave_window();

            // --------------------- Events -----------------------
            struct OnMoved {
                Mouse* m_Mouse;
                float  m_X;
                float  m_Y;

                float m_DeltaX;
                float m_DeltaY;
            };

            struct OnButtonPressed {
                Mouse*   m_Mouse;
                float    m_X;
                float    m_Y;
                e_Button m_Button;
            };

            struct OnButtonReleased {
                Mouse*   m_Mouse;
                float    m_X;
                float    m_Y;
                e_Button m_Button;
            };

            struct OnDoubleClick {
                Mouse*   m_Mouse;
                float    m_X;
                float    m_Y;
                e_Button m_Button;
            };

            struct OnScroll {
                Mouse* m_Mouse;
                int    m_ScrollAmount;
            };

            struct OnEnterWindow {
                Mouse*  m_Mouse;
            };

            struct OnLeaveWindow {
                Mouse*  m_Mouse;
            };

        private:
            Input* m_Manager = nullptr;

            std::array<bool, 3> m_Buttons = {};

            float m_X = 0;
            float m_Y = 0;
        };

        std::ostream& operator << (std::ostream& os, const Mouse::e_Button& button);
                                  
        std::ostream& operator << (std::ostream& os, const Mouse::OnMoved&          mm);
        std::ostream& operator << (std::ostream& os, const Mouse::OnButtonPressed&  bp);
        std::ostream& operator << (std::ostream& os, const Mouse::OnButtonReleased& br);
        std::ostream& operator << (std::ostream& os, const Mouse::OnDoubleClick&    bc);
        std::ostream& operator << (std::ostream& os, const Mouse::OnScroll&         ms);
        std::ostream& operator << (std::ostream& os, const Mouse::OnEnterWindow&    ew);
        std::ostream& operator << (std::ostream& os, const Mouse::OnLeaveWindow&    lw);

        std::ostream& operator << (std::ostream& os, const Mouse& m);
    }
}