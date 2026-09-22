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

        if(pbr_mat.Albedo_texture->texture){
            encoder->setFragmentTexture(pbr_mat.Albedo_texture->texture, (NS::UInteger)TEXTURE_INDEX::BASE_COLOR);
        }

        if(pbr_mat.Metallic_texture->texture){
            encoder->setFragmentTexture(pbr_mat.Metallic_texture->texture, (NS::UInteger)TEXTURE_INDEX::METALLIC);
        }
        if(pbr_mat.Normal_texture->texture){
            encoder->setFragmentTexture(pbr_mat.Normal_texture->texture, (NS::UInteger)TEXTURE_INDEX::NORMAL);
        }
        if(pbr_mat.Specular_Texture->texture){
            encoder->setFragmentTexture(pbr_mat.Specular_Texture->texture, (NS::UInteger)TEXTURE_INDEX::SPECULAR);
        }
        if(pbr_mat.Roughness_texture->texture){
            encoder->setFragmentTexture(pbr_mat.Roughness_texture->texture, (NS::UInteger)TEXTURE_INDEX::ROUGHNESS);
        }
        
        
        
        
        
        encoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, (NS::UInteger) IndexCount , MTL::IndexTypeUInt32, IndicesBuffer,0);
}