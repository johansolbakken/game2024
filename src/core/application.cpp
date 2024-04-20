#include "application.h"

namespace JohanGame2024
{
    Application *Application::s_instance = nullptr;

    Application::Application()
    {
        if (s_instance)
        {
            std::cout << "Application already exists!" << std::endl;   
            return;
        }

        s_instance = this;

        m_window = Window::create();

        m_running = true;
    }

    Application::~Application()
    {
    }

    void Application::run()
    {
        while (m_running)
        {
            m_window->onUpdate();
        }
    }
}