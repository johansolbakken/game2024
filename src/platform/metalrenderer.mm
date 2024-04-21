#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "metalrenderer.h"

#include <MetalKit/MetalKit.h>
#include <cstring>

#include <Metal/Metal.h>
#include <objc/objc.h>

#include <iostream>

#include <chrono>

namespace JohanGame2024 {

struct Vertex {
  glm::vec3 position;
};

struct Uniform {
  glm::mat4 mvp;
  glm::vec4 color;
};

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

  id<MTLRenderPipelineState> pipelineState;
  id<MTLBuffer> vertexBuffer;
  id<MTLBuffer> indexBuffer;
  uint32_t indexCount;
  id<MTLBuffer> uniformBuffer;

  id<MTLDepthStencilState> depthStencilState;
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

  std::string cubeShader = R"(
    #include <metal_stdlib>
    using namespace metal;

    struct VertexIn {
      float3 position [[attribute(0)]];
    };

    struct VertexOut {
      float4 position [[position]];
      float4 color;
    };

    struct Uniform {
      float4x4 mvp;
      float4 color;
    };

    vertex VertexOut vertex_main(const VertexIn vertex_in [[stage_in]],
                                 constant Uniform &uniforms [[buffer(1)]]) {
      VertexOut vertex_out;
      vertex_out.position = uniforms.mvp * float4(vertex_in.position, 1.0);
      vertex_out.color = uniforms.color;
      return vertex_out;
    }

    fragment float4 fragment_main(VertexOut vertex_out [[stage_in]]) {
      return vertex_out.color;
    }
  )";

  NSError *error = nil;
  id<MTLLibrary> library =
      [s_data->device newLibraryWithSource:@(cubeShader.c_str())
                                   options:nil
                                     error:&error];
  if (error) {
    std::cerr << "Failed to create library: "
              << error.localizedDescription.UTF8String << std::endl;
    return;
  }

  MTLVertexDescriptor *vertexDescriptor = [MTLVertexDescriptor new];
  vertexDescriptor.attributes[0].format = MTLVertexFormatFloat3;
  vertexDescriptor.attributes[0].offset = 0;
  vertexDescriptor.attributes[0].bufferIndex = 0;

  vertexDescriptor.layouts[0].stride = sizeof(Vertex);
  vertexDescriptor.layouts[0].stepRate = 1;
  vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;

  MTLRenderPipelineDescriptor *pipelineDescriptor =
      [MTLRenderPipelineDescriptor new];
  pipelineDescriptor.vertexFunction =
      [library newFunctionWithName:@"vertex_main"];
  pipelineDescriptor.fragmentFunction =
      [library newFunctionWithName:@"fragment_main"];
  pipelineDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
  pipelineDescriptor.vertexDescriptor = vertexDescriptor;

  id<MTLRenderPipelineState> pipelineState =
      [s_data->device newRenderPipelineStateWithDescriptor:pipelineDescriptor
                                                     error:&error];
  if (error) {
    std::cerr << "Failed to create pipeline state: "
              << error.localizedDescription.UTF8String << std::endl;
    return;
  }

  s_data->renderPassDescriptor.colorAttachments[0].texture = nullptr;
  s_data->renderPassDescriptor.colorAttachments[0].loadAction =
      MTLLoadActionClear;
  s_data->renderPassDescriptor.colorAttachments[0].storeAction =
      MTLStoreActionStore;
  s_data->renderPassDescriptor.colorAttachments[0].clearColor =
      MTLClearColorMake(0.0, 0.0, 0.0, 1.0);

  s_data->renderPassDescriptor.depthAttachment.texture = nullptr;
  s_data->renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
  s_data->renderPassDescriptor.depthAttachment.storeAction =
      MTLStoreActionDontCare;
  s_data->renderPassDescriptor.depthAttachment.clearDepth = 1.0;

  s_data->renderPassDescriptor.stencilAttachment.texture = nullptr;
  s_data->renderPassDescriptor.stencilAttachment.loadAction =
      MTLLoadActionClear;
  s_data->renderPassDescriptor.stencilAttachment.storeAction =
      MTLStoreActionDontCare;
  s_data->renderPassDescriptor.stencilAttachment.clearStencil = 0;

  s_data->pipelineState = pipelineState;

  Vertex cubeVertices[] = {
      // Front face
      {{-1.0, -1.0, 1.0}}, // Vertex 0
      {{1.0, -1.0, 1.0}},  // Vertex 1
      {{1.0, 1.0, 1.0}},   // Vertex 2
      {{-1.0, 1.0, 1.0}},  // Vertex 3
      // Back face
      {{-1.0, -1.0, -1.0}}, // Vertex 4
      {{1.0, -1.0, -1.0}},  // Vertex 5
      {{1.0, 1.0, -1.0}},   // Vertex 6
      {{-1.0, 1.0, -1.0}},  // Vertex 7
  };

  uint16_t indices[] = {// Front face
                        0, 1, 2, 2, 3, 0,
                        // Right face
                        1, 5, 6, 6, 2, 1,
                        // Back face
                        7, 6, 5, 5, 4, 7,
                        // Left face
                        4, 0, 3, 3, 7, 4,
                        // Top face
                        3, 2, 6, 6, 7, 3,
                        // Bottom face
                        4, 5, 1, 1, 0, 4};

  Uniform uniform = {
      glm::mat4(1.0),
      {1.0f, 0.0f, 0.0f, 1.0f},
  };

  s_data->vertexBuffer =
      [s_data->device newBufferWithBytes:cubeVertices
                                  length:sizeof(cubeVertices)
                                 options:MTLResourceStorageModeShared];
  s_data->indexBuffer =
      [s_data->device newBufferWithBytes:indices
                                  length:sizeof(indices)
                                 options:MTLResourceStorageModeShared];
  s_data->indexCount = sizeof(indices) / sizeof(indices[0]);

  s_data->uniformBuffer =
      [s_data->device newBufferWithBytes:&uniform
                                  length:sizeof(uniform)
                                 options:MTLResourceStorageModeShared];

  MTLTextureDescriptor *depthTextureDescriptor = [MTLTextureDescriptor
      texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                   width:window->framebufferWidth()
                                  height:window->framebufferHeight()
                               mipmapped:NO];
  depthTextureDescriptor.usage = MTLTextureUsageRenderTarget;
  depthTextureDescriptor.storageMode = MTLStorageModePrivate;
  id<MTLTexture> depthTexture =
      [s_data->device newTextureWithDescriptor:depthTextureDescriptor];

  s_data->renderPassDescriptor.depthAttachment.texture = depthTexture;
  s_data->renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
  s_data->renderPassDescriptor.depthAttachment.storeAction =
      MTLStoreActionDontCare;
  s_data->renderPassDescriptor.depthAttachment.clearDepth = 1.0;

  MTLDepthStencilDescriptor *depthStencilDescriptor =
      [MTLDepthStencilDescriptor new];
  depthStencilDescriptor.depthCompareFunction = MTLCompareFunctionLess;
  depthStencilDescriptor.depthWriteEnabled = YES;
  id<MTLDepthStencilState> depthStencilState = [s_data->device
      newDepthStencilStateWithDescriptor:depthStencilDescriptor];

  s_data->depthStencilState = depthStencilState;
}

void MetalRenderer::shutdown() {
  s_data->device = nil;
  s_data->commandQueue = nil;
  s_data->renderPassDescriptor = nil;

  delete s_data;
}

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

  // [renderEncoder setViewport:MTLViewport{0.0, 0.0, (double)width,
  //                                        (double)height, 0.0, 1.0}]

  [renderEncoder setDepthStencilState:s_data->depthStencilState];

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

void MetalRenderer::clearColor(const glm::vec4 &color) {
  s_data->clearColor = color;
}

void MetalRenderer::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                             const glm::vec4 &color) {}

void MetalRenderer::drawCube(const glm::vec3 &position, const glm::vec3 &size,
                             const glm::vec4 &color) {
  auto renderEncoder = s_data->renderEncoder;
  auto pipelineState = s_data->pipelineState;
  auto vertexBuffer = s_data->vertexBuffer;
  auto indexBuffer = s_data->indexBuffer;
  auto uniformBuffer = s_data->uniformBuffer;

  auto window = s_data->window;

  static float rotation = 0.0;
  rotation += 0.01;

  glm::mat4 model = glm::translate(glm::mat4(1.0), position);
  model = glm::rotate(model, rotation, glm::vec3(0.0, 1.0, 0.0));
  model = glm::scale(model, size);
  glm::mat4 view =
      glm::lookAt(glm::vec3(3.0, 3.0, 3.0), glm::vec3(0.0, 0.0, 0.0),
                  glm::vec3(0.0, 1.0, 0.0));
  glm::mat4 projection = glm::perspective(
      glm::radians(45.0f), (float)window->width() / window->height(), 0.1f,
      1000.0f);

  Uniform uniform = {
      projection * view * model,
      {color.r, color.g, color.b, color.a},
  };

  memcpy([uniformBuffer contents], &uniform, sizeof(uniform));

  [renderEncoder setRenderPipelineState:pipelineState];
  [renderEncoder setVertexBuffer:vertexBuffer offset:0 atIndex:0];
  [renderEncoder setVertexBuffer:uniformBuffer offset:0 atIndex:1];
  [renderEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                            indexCount:s_data->indexCount
                             indexType:MTLIndexTypeUInt16
                           indexBuffer:s_data->indexBuffer
                     indexBufferOffset:0];
}
} // namespace JohanGame2024
