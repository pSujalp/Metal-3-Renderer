
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
                texture2d<float> colorTexture [[texture(TEXTURE_INDEX::ALBEDO)]],
                texture2d<float> normalTexture [[texture(TEXTURE_INDEX::NORMAL)]],
                texture2d<float> roughTexture [[texture(TEXTURE_INDEX::ROUGHNESS)]],
                texture2d<float> metallicTexture [[texture(TEXTURE_INDEX::METALLIC)]],
                texture2d<float> specularTexture [[texture(TEXTURE_INDEX::SPECULAR)]],
                constant PBR_COLOR & pbr_color [[buffer(TEXTURE_INDEX::BASE_COLOR)]]) {
    
    constexpr sampler textureSampler (mag_filter::linear,min_filter::linear);


    float4 baseColour;

    if (!is_null_texture(colorTexture))
                       baseColour= colorTexture.sample(textureSampler, in.texCoords);

    else baseColour = pbr_color.base_color;

    
    
    return baseColour;
}