#pragma once

#include <iosfwd>

namespace hecate {
    class Input;

    namespace input {
        class Keyboard {
        public:
            // grouped (groups in random order though)
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
                alt,
                shift,
                space,
                tab,
                enter,
                escape,
                backspace,

                up,
                left,
                right,
                down,
                pg_up,
                pg_down,
                home,
                end,
                ins,
                del
            };

            Keyboard(Input* manager);
            ~Keyboard();

            Keyboard             (const Keyboard&) = delete;
            Keyboard& operator = (const Keyboard&) = delete;
            Keyboard             (Keyboard&&)                 = delete;
            Keyboard& operator = (Keyboard&&) = delete;

            bool is_down(e_Key key) const;
            bool is_up  (e_Key key) const;

            void set_state(e_Key key, bool pressed);

            // --------------------- Events -----------------------
            struct OnKeyPressed {
                Keyboard* kbd;
                e_Key     key;
            };

            struct OnKeyReleased {
                Keyboard* kbd;
                e_Key     key;
            };

        private:
            Input* m_Manager = nullptr;

            bool m_Keys[256] = {};
        };

        std::ostream& operator << (std::ostream& os, const Keyboard::e_Key&         key);
        std::ostream& operator << (std::ostream& os, const Keyboard::OnKeyPressed&  kp);
        std::ostream& operator << (std::ostream& os, const Keyboard::OnKeyReleased& kr);
    }
}