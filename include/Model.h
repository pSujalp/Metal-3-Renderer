#pragma once


#include "Mesh.h"
#include <string>
#include <ufbx.h>
#include "PBR_Material.hpp"
#include <unordered_map>
#include <iostream>

class Model{

    public:

    std::vector<Mesh*> meshes;
    std::unordered_map<std::string, PBRMaterial> pbr_textures_map;


    Model() = default;
    ~Model();
    Model(const std::string &str, MTL::Device * metalDevice);
    void loadTextures(const ufbx_material_list materiallist,MTL::Device * metalDevice);
    void Draw(MTL::RenderCommandEncoder * encoder, MTL::RenderPipelineState * Rpso, MTL::DepthStencilState * DSO, MTL::Buffer * transformationBuffer);


};