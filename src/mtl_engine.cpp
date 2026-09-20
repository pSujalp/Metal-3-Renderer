#include "mtl_engine.hpp"
#include "metal_view_bridge.h"
#include "autorelease_pool.h"

void MTLEngine::init()
{
    initDevice();
    initWindow();

    createTriangle();

    camera = Camera(glm::vec3(0.0f, 0.0f, 0.f));

    createDepthAndTextures();

    createDefaultLibrary();
    createCommandQueue();
    createRenderPipeline();
}

void MTLEngine::createDepthAndTextures()
{
    int width, height;
    glfwGetFramebufferSize(glfwWindow, &width, &height);

    // Multisampled color target, resolved into the drawable each frame
    MTL::TextureDescriptor *msaaTextureDescriptor = MTL::TextureDescriptor::alloc()->init();
    msaaTextureDescriptor->setTextureType(MTL::TextureType2DMultisample);
    msaaTextureDescriptor->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
    msaaTextureDescriptor->setWidth(width);
    msaaTextureDescriptor->setHeight(height);
    msaaTextureDescriptor->setSampleCount(sampleCount);
    msaaTextureDescriptor->setStorageMode(MTL::StorageModePrivate);
    msaaTextureDescriptor->setUsage(MTL::TextureUsageRenderTarget);

    renderTarget = metalDevice->newTexture(msaaTextureDescriptor);

    // Depth must use the same sample count as the color target
    MTL::TextureDescriptor *depthTextureDescriptor = MTL::TextureDescriptor::alloc()->init();
    depthTextureDescriptor->setTextureType(MTL::TextureType2DMultisample);
    depthTextureDescriptor->setPixelFormat(MTL::PixelFormatDepth32Float);
    depthTextureDescriptor->setWidth(width);
    depthTextureDescriptor->setHeight(height);
    depthTextureDescriptor->setSampleCount(sampleCount);
    depthTextureDescriptor->setStorageMode(MTL::StorageModePrivate);
    depthTextureDescriptor->setUsage(MTL::TextureUsageRenderTarget);

    depthTexture = metalDevice->newTexture(depthTextureDescriptor);

    msaaTextureDescriptor->release();
    depthTextureDescriptor->release();
}

void MTLEngine::run()
{
    while (!glfwWindowShouldClose(glfwWindow))
    {
        {
            AutoreleasePoolGuard pool;
            metalDrawable = MetalViewBridge::NextDrawable(metalLayerHandle);
            draw();
        }
        glfwPollEvents();
    }
}

void MTLEngine::cleanup()
{
    glfwTerminate();

    dq.push_function([this]
                     {
                         metalDevice->release();
                         renderTarget->release();
                         depthTexture->release();
                     });

    dq.flush();
}

void MTLEngine::initDevice()
{
    metalDevice = MTL::CreateSystemDefaultDevice();
}

void MTLEngine::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindow = glfwCreateWindow(800, 600, "Metal Engine", NULL, NULL);

    glfwSetMouseButtonCallback(glfwWindow, mouse_button_callback);

    if (!glfwWindow)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    int width, height;
    glfwGetFramebufferSize(glfwWindow, &width, &height);

    metalLayerHandle = MetalViewBridge::CreateAndAttachLayer(
        glfwWindow, metalDevice, MTL::PixelFormatBGRA8Unorm, width, height);
}

void MTLEngine::createTriangle()
{
    VertexData triangleVertices[] = {
        {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
        {{0.0f, 0.5f, 0.0f}, {0.5f, 1.0f}}};

    triangleVertexBuffer = metalDevice->newBuffer(&triangleVertices, sizeof(triangleVertices), MTL::ResourceStorageModeShared);

    transformationBuffer = metalDevice->newBuffer(sizeof(MVP), MTL::ResourceStorageModeShared);

    dq.push_function([this]
                     {
        if(triangleVertexBuffer) triangleVertexBuffer->release(); });
    dq.push_function([this]
                     {
        if(transformationBuffer) transformationBuffer->release(); });
}

void MTLEngine::createDefaultLibrary()
{
    metalDefaultLibrary = metalDevice->newDefaultLibrary();
    if (!metalDefaultLibrary)
    {
        std::cerr << "Failed to load default library.";
        std::exit(-1);
    }
    dq.push_function([this]
                     {if(metalDefaultLibrary) metalDefaultLibrary->release(); });
}

void MTLEngine::createCommandQueue()
{
    metalCommandQueue = metalDevice->newCommandQueue();
}

void MTLEngine::createRenderPipeline()
{

    renderPSO = new RenderPipelinePSO("vertexShader", "fragmentShader", metalDefaultLibrary, metalLayerHandle, metalDevice);

    texture = new Texture("assets/texel_checker.png", metalDevice);

    MTL::DepthStencilDescriptor *dsd = MTL::DepthStencilDescriptor::alloc()->init();
    dsd->setDepthCompareFunction(MTL::CompareFunctionLessEqual);
    dsd->setDepthWriteEnabled(true);
    metalDSO = metalDevice->newDepthStencilState(dsd);

    dsd->release();

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uv;
    std::vector<unsigned int> indices;

    sphere = new Sphere(5, 30, 20);

    std::vector<VertexData> positionsVertex;
    positionsVertex.reserve(sphere->positions.size());
    for (size_t i = 0; i < sphere->positions.size(); i++)
    {
        const glm::vec3 &p = sphere->positions[i];
        const glm::vec2 &t = sphere->uv[i];
        positionsVertex.push_back(VertexData{{p.x, p.y, p.z}, {t.x, t.y}});
    }

    SphereVertexBuffer = metalDevice->newBuffer(positionsVertex.data(), positionsVertex.size() * sizeof(VertexData), MTL::ResourceStorageModeShared);
    SphereIndexedBuffer = metalDevice->newBuffer(sphere->indices.data(), sphere->indices.size() * sizeof(unsigned int), MTL::ResourceStorageModeShared);
}

void MTLEngine::draw()
{
    sendRenderCommand();
    ProcessKeyboardInput(deltaTime);

    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void MTLEngine::sendRenderCommand()
{
    metalCommandBuffer = metalCommandQueue->commandBuffer();

    MTL::RenderPassDescriptor *renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
    MTL::RenderPassColorAttachmentDescriptor *cd = renderPassDescriptor->colorAttachments()->object(0);
    MTL::RenderPassDepthAttachmentDescriptor *depthAttachment = renderPassDescriptor->depthAttachment();

    depthAttachment->setTexture(depthTexture);
    depthAttachment->setLoadAction(MTL::LoadActionClear);
    depthAttachment->setStoreAction(MTL::StoreActionDontCare);
    depthAttachment->setClearDepth(1.0);

    cd->setTexture(renderTarget);
    cd->setResolveTexture(metalDrawable->texture());
    cd->setLoadAction(MTL::LoadActionClear);
    cd->setClearColor(MTL::ClearColor(41.0f / 255.0f, 42.0f / 255.0f, 48.0f / 255.0f, 1.0));
    cd->setStoreAction(MTL::StoreActionMultisampleResolve);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

    static float accumulatedDegrees = 0.0f;
    const float rotationSpeedDegreesPerSecond = 45.0f;
    accumulatedDegrees += rotationSpeedDegreesPerSecond * deltaTime;
    if (accumulatedDegrees >= 360.0f)
        accumulatedDegrees -= 360.0f;
    float angleInRadians = accumulatedDegrees * (M_PI / 180.0f);
    model = glm::rotate(model, angleInRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 viewMatrix = camera.GetViewMatrix();

    float aspectRatio = (float)800 / (float)600;
    float fov = camera.Zoom;
    float nearZ = 0.1f;
    float farZ = 100.0f;
    glm::mat4 perspectiveMatrix = glm::perspective(fov, aspectRatio, nearZ, farZ);
    glm::mat4 MVP_GLM = perspectiveMatrix * viewMatrix * model;

    MVP mvp1;
    mvp1.mvp = *reinterpret_cast<matrix_float4x4 *>(&MVP_GLM);

    memcpy(transformationBuffer->contents(), &mvp1, sizeof(MVP));

    MTL::RenderCommandEncoder *renderCommandEncoder = metalCommandBuffer->renderCommandEncoder(renderPassDescriptor);
    encodeRenderCommand(renderCommandEncoder);
    renderCommandEncoder->endEncoding();

    metalCommandBuffer->presentDrawable(metalDrawable);
    metalCommandBuffer->commit();
    metalCommandBuffer->waitUntilCompleted();

    renderPassDescriptor->release();
}

void MTLEngine::encodeRenderCommand(MTL::RenderCommandEncoder *renderCommandEncoder)
{
    renderCommandEncoder->setRenderPipelineState(renderPSO->RenderPSO);
    renderCommandEncoder->setDepthStencilState(metalDSO);
    renderCommandEncoder->setVertexBuffer(SphereVertexBuffer, 0, (NS::UInteger)BUFFER_INDEX::Position);
    renderCommandEncoder->setVertexBuffer(transformationBuffer, 0, (NS::UInteger)BUFFER_INDEX::MVP);
    renderCommandEncoder->setFragmentTexture(texture->texture, (NS::UInteger)TEXTURE_INDEX::BASE_COLOR);

    MTL::PrimitiveType typeTriangle = MTL::PrimitiveTypeTriangle;

    renderCommandEncoder->drawIndexedPrimitives(typeTriangle, sphere->indexCount, MTL::IndexTypeUInt32, SphereIndexedBuffer, 0);
}

void MTLEngine::ProcessKeyboardInput(float deltaTime)
{
    if (glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(glfwWindow, true);
    if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    if (glfwGetKey(glfwWindow, GLFW_KEY_M) == GLFW_PRESS)
        glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void MTLEngine::mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        double xpos = width / 2.0;
        double ypos = height / 2.0;
        glfwSetCursorPos(window, xpos, ypos);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}