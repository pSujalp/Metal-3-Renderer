#include "Mesh.h"



Mesh::Mesh(std::vector<VertexData> vertices, std::vector<uint32_t>  indices, std::string mat_name, MTL::Device * metalDevice){

    VerticesBuffer = metalDevice->newBuffer(vertices.data(), sizeof(VertexData) * vertices.size(), MTL::ResourceStorageModeShared);
    IndicesBuffer = metalDevice->newBuffer(indices.data(), sizeof(uint32_t) * indices.size(), MTL::ResourceStorageModeShared);
    this->IndexCount = indices.size();
    this->mat_name = mat_name;
}

Mesh::~Mesh(){

    if(VerticesBuffer) VerticesBuffer->release();
    if(IndicesBuffer) IndicesBuffer->release();
}


void Mesh::Draw(MTL::RenderCommandEncoder * encoder, MTL::RenderPipelineState * Rpso, MTL::DepthStencilState * DSO , PBRMaterial pbr_mat, MTL::Buffer * transformationBuffer){

        encoder->setRenderPipelineState(Rpso);
        encoder->setDepthStencilState(DSO);
        encoder->setVertexBuffer(VerticesBuffer, 0, (NS::UInteger)BUFFER_INDEX::Position);
        encoder->setVertexBuffer(transformationBuffer, 0, (NS::UInteger)BUFFER_INDEX::MVP);
        encoder->setFragmentTexture(pbr_mat.Albedo_texture->texture, (NS::UInteger)TEXTURE_INDEX::BASE_COLOR);
        encoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, (NS::UInteger) IndexCount , MTL::IndexTypeUInt32, IndicesBuffer,0);
}