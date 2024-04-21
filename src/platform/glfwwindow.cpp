#include "glfwwindow.h"

#include "core/application.h"

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <iostream>

namespace JohanGame2024
{

    static void glfw_error_callback(int error, const char *description)
    {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
    }

    GlfwWindow::GlfwWindow(const WindowProps &props)
    {
        m_title = props.title;
        m_width = props.width;
        m_height = props.height;

        glfwSetErrorCallback(glfw_error_callback);

        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return;
        }

        // metal window
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_window = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);

        if (!m_window)
        {
            std::cerr << "Failed to create GLFW window" << std::endl;
            return;
        }
    }

    GlfwWindow::~GlfwWindow()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void GlfwWindow::onUpdate()
    {
        if (glfwWindowShouldClose(m_window))
        {
            Application::instance().close();
        }
        glfwPollEvents();
    }

    void *GlfwWindow::nativeWindow() const
    {
        return glfwGetCocoaWindow(m_window);
    }

    uint32_t GlfwWindow::framebufferWidth() const
    {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        return width;
    }

    uint32_t GlfwWindow::framebufferHeight() const
    {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        return height;
    }

    Ref<Window> Window::create(const WindowProps &props)
    {
        return std::make_shared<GlfwWindow>(props);
    }
}
