
#include <metal_stdlib>
using namespace metal;

#include "VertexData.hpp"
#include "Indices.hpp"

struct VertexOut {
    float4 position [[position]];
    float2 texCoords;
};


vertex VertexOut vertexShader(uint vertexID [[vertex_id]],
             constant VertexData* vertexPositions[[buffer(BUFFER_INDEX::Position)]],
             constant MVP * mvp [[buffer(BUFFER_INDEX::MVP)]]){
    
    VertexOut v;
    v.position = mvp->mvp * float4(vertexPositions[vertexID].position, 1.0f);
    v.texCoords = vertexPositions[vertexID].texCoords;
                
    return v;
}

fragment float4 fragmentShader(VertexOut in [[stage_in]],
                texture2d<float> colorTexture [[texture(TEXTURE_INDEX::BASE_COLOR)]]) {
    
    constexpr sampler textureSampler (mag_filter::linear,min_filter::linear);

    


    float4 baseColour = colorTexture.sample(textureSampler, in.texCoords);


    return baseColour;
}