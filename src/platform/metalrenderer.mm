#include "metalrenderer.h"

#include <MetalKit/MetalKit.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Metal/Metal.h>
#include <objc/objc.h>

#include <iostream>

#include <chrono>

namespace JohanGame2024 {

struct MetalRendererData {
  id<MTLDevice> device;
  id<MTLCommandQueue> commandQueue;
  MTLRenderPassDescriptor *renderPassDescriptor;

  CAMetalLayer *layer;
  id<CAMetalDrawable> drawable;
  id<MTLCommandBuffer> commandBuffer;
  id<MTLRenderCommandEncoder> renderEncoder;

  glm::vec4 clearColor = {0, 0, 0, 1};

  Ref<Window> window;
};

static MetalRendererData *s_data = nullptr;

void MetalRenderer::init(const Ref<Window> &window) {
  s_data = new MetalRendererData();

  s_data->device = MTLCreateSystemDefaultDevice();
  s_data->commandQueue = [s_data->device newCommandQueue];

  NSWindow *nswin = (NSWindow *)window->nativeWindow();
  CAMetalLayer *layer = [CAMetalLayer layer];
  layer.device = s_data->device;
  layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
  nswin.contentView.layer = layer;
  nswin.contentView.wantsLayer = YES;

  s_data->renderPassDescriptor = [MTLRenderPassDescriptor new];

  s_data->layer = layer;

  s_data->window = window;
}

void MetalRenderer::shutdown() { delete s_data; }

void MetalRenderer::begin() {
  auto layer = s_data->layer;
  auto renderPassDescriptor = s_data->renderPassDescriptor;
  auto commandQueue = s_data->commandQueue;

  auto window = s_data->window;
  int width = window->framebufferWidth();
  int height = window->framebufferHeight();

  layer.drawableSize = CGSizeMake(width, height);
  id<CAMetalDrawable> drawable = [layer nextDrawable];

  id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
  renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(
      s_data->clearColor.r * s_data->clearColor.a,
      s_data->clearColor.g * s_data->clearColor.a,
      s_data->clearColor.b * s_data->clearColor.a, s_data->clearColor.a);
  renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
  renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
  renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
  id<MTLRenderCommandEncoder> renderEncoder =
      [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
  [renderEncoder pushDebugGroup:@"ImGui demo"];

  s_data->drawable = drawable;
  s_data->commandBuffer = commandBuffer;
  s_data->renderEncoder = renderEncoder;
}

void MetalRenderer::end() {
  auto renderEncoder = s_data->renderEncoder;
  auto commandBuffer = s_data->commandBuffer;
  auto drawable = s_data->drawable;

  [renderEncoder popDebugGroup];
  [renderEncoder endEncoding];

  [commandBuffer presentDrawable:drawable];
  [commandBuffer commit];

  s_data->renderEncoder = nullptr;
  s_data->commandBuffer = nullptr;
  s_data->drawable = nullptr;
}

void MetalRenderer::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                             const glm::vec4 &color) {}
} // namespace JohanGame2024
