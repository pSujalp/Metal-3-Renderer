#pragma once 
#include <vector>

#include <simd/simd.h>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include "VertexData.hpp"
#include "Texture.hpp"
#include "Indices.hpp"
#include "PBR_Material.hpp"

using namespace simd;

class Mesh{

    public:

    Mesh() = default;
    ~Mesh();
    Mesh(std::vector<VertexData> vertices, std::vector<uint32_t>  indices, std::string mat_name,MTL::Device * metalDevice);
    void Draw(MTL::RenderCommandEncoder * encoder, MTL::RenderPipelineState * Rpso, MTL::DepthStencilState * DSO, PBRMaterial pbr_mat , MTL::Buffer * transformationBuffer);
    

    MTL::Buffer * VerticesBuffer;
    MTL::Buffer * IndicesBuffer;
    size_t IndexCount = 0;
    std::string mat_name;





};