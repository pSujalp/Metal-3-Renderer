#pragma once

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <string>
#include "metal_view_bridge.h"

class RenderPipelinePSO
{

public:
    MTL::RenderPipelineState *RenderPSO;

    RenderPipelinePSO(std::string VertexShader, std::string FragmentShader, MTL::Library *metalDefaultLibrary, void* metalLayerHandle, MTL::Device * metalDevice)
    {
        MTL::Function *vertexShader = nullptr;
        if (!VertexShader.empty()) {
            vertexShader = metalDefaultLibrary->newFunction(NS::String::string(VertexShader.c_str(), NS::ASCIIStringEncoding));
            assert(vertexShader);
        }

        MTL::Function *fragmentShader = nullptr;
        if (!FragmentShader.empty()) {
            fragmentShader = metalDefaultLibrary->newFunction(NS::String::string(FragmentShader.c_str(), NS::ASCIIStringEncoding));
            assert(fragmentShader);
        }

        MTL::RenderPipelineDescriptor *renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
        renderPipelineDescriptor->setLabel(NS::String::string("Rendering Pipeline", NS::ASCIIStringEncoding));
        if (vertexShader) renderPipelineDescriptor->setVertexFunction(vertexShader);
        if (fragmentShader) renderPipelineDescriptor->setFragmentFunction(fragmentShader);
        assert(renderPipelineDescriptor);

        MTL::PixelFormat pixelFormat = MetalViewBridge::GetPixelFormat(metalLayerHandle);
        renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(pixelFormat);
        renderPipelineDescriptor->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);

        NS::Error *error = nullptr;
        RenderPSO = metalDevice->newRenderPipelineState(renderPipelineDescriptor, &error);
        assert(RenderPSO);

        renderPipelineDescriptor->release();
    }

    ~RenderPipelinePSO(){
        if(RenderPSO) RenderPSO->release();
    }
};