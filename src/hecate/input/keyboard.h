#pragma once

#include <iosfwd>
#include <array>

namespace hecate {
    class Input;

    namespace platform {
        class Window;
    }

    namespace input {
        class Keyboard {
        public:
            using Window = platform::Window;

            // grouped (groups in random order though)
            // NOTE not *all* keys are represented here - should add as needed
            enum class e_Key {
                undefined,

                a,
                b,
                c,
                d,
                e,
                f,
                g,
                h,
                i,
                j,
                k,
                l,
                m,
                n,
                o,
                p,
                q,
                r,
                s,
                t,
                u,
                v,
                w,
                x,
                y,
                z,

                _1,
                _2,
                _3,
                _4,
                _5,
                _6,
                _7,
                _8,
                _9,
                _0,

                f1,
                f2,
                f3,
                f4,
                f5,
                f6,
                f7,
                f8,
                f9,
                f10,
                f11,
                f12,

                semicolon,
                questionmark,
                tilde,
                brace_open,
                vertical_pipe,
                brace_close,
                double_quote,
                oem_8,
                plus,
                minus,
                comma,
                period,

                ctrl,
                left_ctrl,
                right_ctrl,
                alt,
                left_alt,
                right_alt,
                shift,
                left_shift,
                right_shift,
                space,
                tab,
                enter,
                escape,
                backspace,

                up_arrow,
                left_arrow,
                right_arrow,
                down_arrow,
                pg_up,
                pg_down,
                home,
                end,
                ins,
                del,

                __count // not an actual key :P
            };

            Keyboard(Input* manager);
            ~Keyboard();

            Keyboard             (const Keyboard&)     = delete;
            Keyboard& operator = (const Keyboard&)     = delete;
            Keyboard             (Keyboard&&) noexcept = default;
            Keyboard& operator = (Keyboard&&) noexcept = default;

            bool is_down(e_Key key) const;
            bool is_up  (e_Key key) const;

            void set_state(
                e_Key   key, 
                bool    pressed, 
                Window* win = nullptr // (optional) pointer to the active window handling the input
            );

            // --------------------- Events -----------------------
            struct OnKeyPressed {
                Keyboard* kbd;
                e_Key     key;
                Window*   win; // the active window
            };

            struct OnKeyReleased {
                Keyboard* kbd;
                e_Key     key;
                Window*   win; // the active window
            };

        private:
            Input* m_Manager = nullptr;

            std::array<bool, std::to_underlying(e_Key::__count)> m_Keys;
        };

        std::ostream& operator << (std::ostream& os, const Keyboard::e_Key&         key);
        std::ostream& operator << (std::ostream& os, const Keyboard::OnKeyPressed&  kp);
        std::ostream& operator << (std::ostream& os, const Keyboard::OnKeyReleased& kr);
    }
}