#pragma once

#include "core/base.h"

namespace JohanGame2024
{
    struct WindowProps
    {
        std::string title;
        uint32_t width;
        uint32_t height;

        WindowProps(const std::string &title = "JohanGame2024",
                    uint32_t width = 1280,
                    uint32_t height = 720)
            : title(title), width(width), height(height) {}
    };

    class Window
    {
    public:
        virtual ~Window() = default;
        virtual void onUpdate() = 0;

        static Ref<Window> create(const WindowProps &props = WindowProps());
    };
}