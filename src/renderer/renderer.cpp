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

    void Renderer::resize(uint32_t width, uint32_t height)
    {
        MetalRenderer::resize(width, height);
    }

    void Renderer::begin()
    {
        MetalRenderer::begin();
    }

    void Renderer::end()
    {
        MetalRenderer::end();
    }

    void Renderer::clearColor(const glm::vec4 &color)
    {
        MetalRenderer::clearColor(color);
    }

    void Renderer::drawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
    {
        MetalRenderer::drawQuad(position, size, color);
    }

    void Renderer::drawCube(const glm::vec3 &position, const glm::vec3 &size, const glm::vec4 &color)
    {
        MetalRenderer::drawCube(position, size, color);
    }

} // namespace JohanGame2024