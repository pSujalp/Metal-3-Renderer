#pragma once

#include "Mesh.h"
#include "Texture.hpp"


#include <simd/simd.h>

using namespace simd;

struct PBRMaterial{
    Texture * Albedo_texture = nullptr;
    Texture * Specular_Texture = nullptr;
    Texture * Roughness_texture = nullptr;
    Texture * Metallic_texture = nullptr;
    Texture * Normal_texture = nullptr;
};

