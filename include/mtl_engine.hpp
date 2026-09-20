#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <simd/simd.h>
#include "RenderPipelinePSO.h"
#include "DeletionQueue.h"
#include <iostream>
#include "VertexData.hpp"
#include "Indices.hpp"
#include "Texture.hpp"

#include "magic_enum/magic_enum_all.hpp"

class MTLEngine {
public:
    void init();
    void run();
    void cleanup();

private:
    void initDevice();
    void initWindow();

    void createTriangle();
    void createDefaultLibrary();
    void createCommandQueue();
    void createRenderPipeline();

    void encodeRenderCommand(MTL::RenderCommandEncoder* renderEncoder);
    void sendRenderCommand();
    void draw();

    MTL::Device* metalDevice;
    GLFWwindow* glfwWindow;
    void* metalLayerHandle = nullptr;          
    CA::MetalDrawable* metalDrawable = nullptr;

    MTL::Library* metalDefaultLibrary;
    MTL::CommandQueue* metalCommandQueue;
    MTL::CommandBuffer* metalCommandBuffer;
    RenderPipelinePSO * renderPSO;
    MTL::Buffer* triangleVertexBuffer;
    DeletionQueue dq;

    Texture * texture;
    
};