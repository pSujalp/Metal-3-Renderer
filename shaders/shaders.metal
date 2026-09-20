
#include <metal_stdlib>
using namespace metal;

#include "VertexData.hpp"


struct VertexOut {
    float4 position [[position]];
    float3 color;
};


vertex VertexOut vertexShader(uint vertexID [[vertex_id]],
             constant VertexData* vertexPositions){
    
    VertexOut v;
    v.position = float4(vertexPositions[vertexID].position, 1.0f);
    v.color = vertexPositions[vertexID].color;
                
    
    return v;
}

fragment float4 fragmentShader(VertexOut in [[stage_in]]) {

    return float4(in.color,1.0f);
}