#include "mtl_engine.hpp"
#include "metal_view_bridge.h"
#include "autorelease_pool.h"



void MTLEngine::init() {
    initDevice();
    initWindow();


    createTriangle();


    createDefaultLibrary();
    createCommandQueue();
    createRenderPipeline();
}

void MTLEngine::run() {
    while (!glfwWindowShouldClose(glfwWindow)) {
        {
            AutoreleasePoolGuard pool;
            metalDrawable = MetalViewBridge::NextDrawable(metalLayerHandle);
            draw();
        }
        glfwPollEvents();
    }
}

void MTLEngine::cleanup() {
    glfwTerminate();

    dq.push_function([this]{
        metalDevice->release();
    });

    dq.flush();
    
}

void MTLEngine::initDevice() {
    metalDevice = MTL::CreateSystemDefaultDevice();
}

void MTLEngine::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindow = glfwCreateWindow(800, 600, "Metal Engine", NULL, NULL);

    if (!glfwWindow) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    int width, height;
    glfwGetFramebufferSize(glfwWindow, &width, &height);

    metalLayerHandle = MetalViewBridge::CreateAndAttachLayer(
        glfwWindow, metalDevice, MTL::PixelFormatBGRA8Unorm, width, height);
}

void MTLEngine::createTriangle() {
    VertexData triangleVertices[] = {
       {{0.5f, -0.5f, 0.0f},  {1.0f, 0.0f, 0.0f}},  
       {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},  
       {{0.0f,  0.5f, 0.0f,}, {0.0f, 0.0f, 1.0f }}
    };

    triangleVertexBuffer = metalDevice->newBuffer(&triangleVertices, sizeof(triangleVertices), MTL::ResourceStorageModeShared);

    dq.push_function([this]{
        if(triangleVertexBuffer) triangleVertexBuffer->release();
    });
}

void MTLEngine::createDefaultLibrary() {
    metalDefaultLibrary = metalDevice->newDefaultLibrary();
    if (!metalDefaultLibrary) {
        std::cerr << "Failed to load default library.";
        std::exit(-1);
    }
    dq.push_function([this]{

        if(metalDefaultLibrary) metalDefaultLibrary->release();
    });
}

void MTLEngine::createCommandQueue() {
    metalCommandQueue = metalDevice->newCommandQueue();
}

void MTLEngine::createRenderPipeline() {


    renderPSO = new RenderPipelinePSO("vertexShader","fragmentShader",metalDefaultLibrary,metalLayerHandle,metalDevice);
}

void MTLEngine::draw() {
    sendRenderCommand();
}

void MTLEngine::sendRenderCommand() {
    metalCommandBuffer = metalCommandQueue->commandBuffer();

    MTL::RenderPassDescriptor* renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
    MTL::RenderPassColorAttachmentDescriptor* cd = renderPassDescriptor->colorAttachments()->object(0);
    cd->setTexture(metalDrawable->texture());
    cd->setLoadAction(MTL::LoadActionClear);
    cd->setClearColor(MTL::ClearColor(41.0f/255.0f, 42.0f/255.0f, 48.0f/255.0f, 1.0));
    cd->setStoreAction(MTL::StoreActionStore);

    MTL::RenderCommandEncoder* renderCommandEncoder = metalCommandBuffer->renderCommandEncoder(renderPassDescriptor);
    encodeRenderCommand(renderCommandEncoder);
    renderCommandEncoder->endEncoding();

    metalCommandBuffer->presentDrawable(metalDrawable);
    metalCommandBuffer->commit();
    metalCommandBuffer->waitUntilCompleted();


    renderPassDescriptor->release();
}

void MTLEngine::encodeRenderCommand(MTL::RenderCommandEncoder* renderCommandEncoder) {
    renderCommandEncoder->setRenderPipelineState(renderPSO->RenderPSO);
    renderCommandEncoder->setVertexBuffer(triangleVertexBuffer, 0, 0);
    MTL::PrimitiveType typeTriangle = MTL::PrimitiveTypeTriangle;
    NS::UInteger vertexStart = 0;
    NS::UInteger vertexCount = 3;
    renderCommandEncoder->drawPrimitives(typeTriangle, vertexStart, vertexCount);
}