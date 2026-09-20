#pragma once 
#include <vector>

#include <simd/simd.h>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include "VertexData.hpp"

using namespace simd;

class Mesh{

    public:

    Mesh() = default;
    ~Mesh();
    Mesh(std::vector<VertexData> vertices, std::vector<uint32_t>  indices, MTL::Device * metalDevice);
    

    MTL::Buffer * VerticesBuffer;
    MTL::Buffer * IndicesBuffer;

    size_t IndexCount = 0;





};