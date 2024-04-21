#include "application.h"

#include "renderer/renderer.h"

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

        Renderer::init(m_window);

        m_running = true;
    }

    Application::~Application()
    {
        Renderer::shutdown();
    }

    void Application::run()
    {
        while (m_running)
        {
            Renderer::begin();
            Renderer::drawQuad({0.0f, 0.0f}, {0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f});
            Renderer::end();
            m_window->onUpdate();
        }
    }
}