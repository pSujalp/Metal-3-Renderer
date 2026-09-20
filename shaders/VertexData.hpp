

    #pragma once
    #include <simd/simd.h>

    using namespace simd;

    struct VertexData {
        float3 position;
        float3 normal;
        float2 texCoords;
    };


    struct MVP{
        matrix_float4x4 mvp;
    };