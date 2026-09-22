

#pragma once
#include <simd/simd.h>

using namespace simd;

struct VertexData
{
    float3 position;
    float3 normal;
    float2 texCoords;
};

struct MVP
{
    matrix_float4x4 mvp;
};

struct PBR_COLOR{
    float4 base_color = float4{1.0f,1.0f,1.0f,1.0f};
};