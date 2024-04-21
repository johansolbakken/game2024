#pragma once

#include "glm/glm.hpp"

#include "core/window.h"

namespace JohanGame2024
{
    class MetalRenderer
    {
    public:
        static void init(const Ref<Window> &window);
        static void shutdown();

        static void begin();
        static void end();

        static void clearColor(const glm::vec4 &color);

        static void drawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color);
    };
}