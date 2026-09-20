
#include <metal_stdlib>
using namespace metal;

#include "VertexData.hpp"
#include "Indices.hpp"

struct VertexOut {
    float4 position [[position]];
    float2 texCoords;
    float3 color;
};


vertex VertexOut vertexShader(uint vertexID [[vertex_id]],
             constant VertexData* vertexPositions[[buffer(BUFFER_INDEX::Position)]]){
    
    VertexOut v;
    v.position = float4(vertexPositions[vertexID].position, 1.0f);
    v.texCoords = vertexPositions[vertexID].texCoords;
    v.color = vertexPositions[vertexID].color;
                
    
    return v;
}

fragment float4 fragmentShader(VertexOut in [[stage_in]],
                texture2d<float> colorTexture [[texture(TEXTURE_INDEX::BASE_COLOR)]]) {
    
    constexpr sampler textureSampler (mag_filter::linear,min_filter::linear);

    const float4 colorSample = colorTexture.sample(textureSampler, in.texCoords);
    return colorSample;
}