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

#include "Camera.h"

#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 
#include <glm/ext/matrix_transform.hpp> 
#include <glm/ext/matrix_clip_space.hpp> 
#include <glm/ext/scalar_constants.hpp> 

#include "Sphere.h"

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

    void ProcessKeyboardInput(float deltaTime);

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    
    MTL::Device* metalDevice;
    GLFWwindow* glfwWindow;
    void* metalLayerHandle = nullptr;          
    CA::MetalDrawable* metalDrawable = nullptr;

    MTL::Library* metalDefaultLibrary;
    MTL::CommandQueue* metalCommandQueue;
    MTL::CommandBuffer* metalCommandBuffer;
    RenderPipelinePSO * renderPSO;

    MTL::DepthStencilState * metalDSO;

    MTL::Buffer* triangleVertexBuffer;
    MTL::Buffer* transformationBuffer;

    MTL::Buffer*SphereVertexBuffer;
    MTL::Buffer * SphereIndexedBuffer;

    DeletionQueue dq;

    Texture * texture;
    
    Sphere * sphere;


    Camera camera;


    float deltaTime = 0.0f;	
    float lastFrame = 0.0f;
    float lastX;
    float lastY;
    bool firstMouse = true;
    bool rightMouseButtonPressed = false;

    
};