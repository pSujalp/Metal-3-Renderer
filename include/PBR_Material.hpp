#pragma once

#include "Mesh.h"
#include "Texture.hpp"


struct PBRMaterial{
    Texture * Albedo_texture;
    Texture * Specular_Texture;
    Texture * Roughness_texture;
    Texture * Metallic_texture;
    Texture * Normal_texture;;
};