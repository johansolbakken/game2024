#pragma once

#include "core/window.h"

struct GLFWwindow;

namespace JohanGame2024
{
    class GlfwWindow : public Window
    {
    public:
        GlfwWindow(const WindowProps &props);
        virtual ~GlfwWindow();

        void onUpdate() override;

        void *nativeWindow() const override;

        uint32_t width() const override { return m_width; }
        uint32_t height() const override { return m_height; }
        uint32_t framebufferWidth() const override;
        uint32_t framebufferHeight() const override;

    private:
        GLFWwindow *m_window;
        std::string m_title;
        uint32_t m_width, m_height;
    };
}