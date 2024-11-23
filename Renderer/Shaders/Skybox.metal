
#include <metal_stdlib>

using namespace metal;

#include "ShaderTypes.h"

// Per-vertex inputs fed by vertex buffer laid out with MTLVertexDescriptor in Metal API
struct SkyboxVertex
{
    float4 position [[attribute(VertexAttributePosition)]];
    float3 normal   [[attribute(VertexAttributeNormal)]];
};

struct SkyboxInOut
{
    float4 position [[position]];
    float3 normal;
    float3 lightDirection;
};

vertex SkyboxInOut skybox_vertex(SkyboxVertex        in        [[ stage_in ]],
                                 constant FrameData &frameData [[ buffer(BufferIndexFrameData) ]])
{
    SkyboxInOut out;

    // Add vertex pos to fairy position and project to clip-space
    out.position = frameData.projection_matrix * in.position;

    // Pass position through as texcoord
    out.normal = in.normal;
    
    out.lightDirection = frameData.sun_eye_direction.xyz;

    return out;
}

fragment half4 skybox_fragment(SkyboxInOut    in  [[ stage_in ]])
{
    float3 u_SkyColor = float3(0.2f, 0.4f, 1.0f);
    float3 u_HorizonColor = float3(0.3f, 0.7f, 0.9f);
    float3 u_GroundColor = float3(0.2f, 0.6f, 0.2f);
    float u_SunSize = 0.03f;
    float3 u_LightColor = float3(1.f, 1.f, 0.6f);
    
    //float3 u_LightDirection = float3(in.lightDirection.x, 0, in.lightDirection.z);
    
    float3 normal = normalize(in.normal);
    float3 lightDir = -normalize(in.lightDirection);

    float3 dayTint = float3(1.0);
    float3 dawnTint = float3(2.0, 0.7, 0.5);
    float3 nightTint = float3(0.05, 0.05, 0.1);

    float3 color;
    if (normal.y > 0.0) {
        color = mix(u_HorizonColor, u_SkyColor, normal.y);
    }
    else {
        color = u_GroundColor;
    }

    float sunValue = max(dot(normal, -lightDir) - (1.0 - u_SunSize), 0.0) / u_SunSize;
    float3 sunColor;
    if(normal.y > 0.0) {
        sunColor = u_LightColor * float3(pow(sunValue, 3.0));
    }
    else {
        sunColor = float3(0.0);
    }

    float3 tint;
    if(lightDir.y > 0.0) {
        tint = mix(dawnTint, nightTint, lightDir.y);
    }
    else {
        tint = mix(dawnTint, dayTint, -lightDir.y);
    }

    float4 out = float4((color + sunColor) * tint, 1.0);
    return half4( out );
}

