#include "renderer.h"

#include "platform/metalrenderer.h"

namespace JohanGame2024
{
    void Renderer::init(const Ref<Window> &window)
    {
        MetalRenderer::init(window);
    }

    void Renderer::shutdown()
    {
        MetalRenderer::shutdown();
    }

    void Renderer::begin()
    {
        MetalRenderer::begin();
    }

    void Renderer::end()
    {
        MetalRenderer::end();
    }

    void Renderer::drawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
    {
        MetalRenderer::drawQuad(position, size, color);
    }
} // namespace JohanGame2024