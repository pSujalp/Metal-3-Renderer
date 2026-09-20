#pragma once


#include "Mesh.h"
#include <string>
#include <ufbx.h>


class Model{

    public:

    std::vector<Mesh*> meshes;

    Model() = default;
    ~Model();
    Model(const std::string &str, MTL::Device * metalDevice);


};