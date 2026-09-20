#include "Mesh.h"



Mesh::Mesh(std::vector<VertexData> vertices, std::vector<uint32_t>  indices, MTL::Device * metalDevice){


    VerticesBuffer = metalDevice->newBuffer(vertices.data(), sizeof(VertexData) * vertices.size(), MTL::ResourceStorageModeShared);
    IndicesBuffer = metalDevice->newBuffer(indices.data(), sizeof(uint32_t) * indices.size(), MTL::ResourceStorageModeShared);
    this->IndexCount = indices.size();
}

Mesh::~Mesh(){

    if(VerticesBuffer) VerticesBuffer->release();
    if(IndicesBuffer) IndicesBuffer->release();
}