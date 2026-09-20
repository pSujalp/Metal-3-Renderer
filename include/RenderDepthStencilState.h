#pragma once

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <string>
#include "metal_view_bridge.h"

class RenderDepthStencilState{
    public:

    MTL::DepthStencilState * metalDSO;

    RenderDepthStencilState(MTL::CompareFunction compFunc, bool setDepthWriteEnabled,MTL::Device * metalDevice){


    MTL::DepthStencilDescriptor *dsd = MTL::DepthStencilDescriptor::alloc()->init();
    dsd->setDepthCompareFunction(compFunc);
    dsd->setDepthWriteEnabled(setDepthWriteEnabled);
    metalDSO = metalDevice->newDepthStencilState(dsd);

    dsd->release();

    }


    ~RenderDepthStencilState(){

        if(metalDSO) metalDSO->release();
    }
};