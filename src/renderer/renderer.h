#pragma once

#include <glm/glm.hpp>

#include "core/window.h"

namespace JohanGame2024
{
    class Renderer
    {
    public:
        static void init(const Ref<Window> &window);
        static void shutdown();

        static void begin();
        static void end();
        
        static void drawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color);
    };
}