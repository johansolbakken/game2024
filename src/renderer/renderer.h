#pragma once

#include "core/window.h"

namespace JohanGame2024
{
    class Renderer
    {
    public:
        static void init(const Ref<Window> &window);
        static void shutdown();

        static void resize(uint32_t width, uint32_t height);

        static void begin();
        static void end();

        static void clearColor(const glm::vec4 &color);
        
        static void drawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color);
        static void drawCube(const glm::vec3 &position, const glm::vec3 &size, const glm::vec4 &color);
    };
}