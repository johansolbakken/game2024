#pragma once

#include "core/window.h"

namespace JohanGame2024
{
    class Application
    {
    public:
        Application();
        ~Application();

        void run();

        void close() { m_running = false; }

        static Application &instance() { return *s_instance; }

    private:
        bool m_running;

        Ref<Window> m_window;

        static Application *s_instance;
    };
}