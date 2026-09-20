

#pragma once
#include <simd/simd.h>

using namespace simd;

struct VertexData {
    float3 position;
    float2 texCoords;
    float3 color;
};


struct MVP{
    matrix_float4x4 mvp;
};