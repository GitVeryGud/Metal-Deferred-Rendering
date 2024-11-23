
#include <metal_stdlib>

using namespace metal;

// Include header shared between this Metal shader code and C code executing Metal API commands
#include "ShaderTypes.h"

// Include header shared between all Metal shader code files
#include "ShaderCommon.h"

struct ProbeInOut
{
    float4 position [[position]];
    half3 color;
    half2 tex_coord;
};

vertex ProbeInOut probe_vertex(constant SimpleVertex      *vertices        [[ buffer(BufferIndexMeshPositions) ]],
                               const device PointLight    *light_data      [[ buffer(BufferIndexLightsData) ]],
                               const device vector_float4 *light_positions [[ buffer(BufferIndexLightsPosition) ]],
                               constant FrameData         &frameData       [[ buffer(BufferIndexFrameData) ]],
                               uint                        iid             [[ instance_id ]],
                               uint                        vid             [[ vertex_id ]])
{
    ProbeInOut out;

    float3 vertex_position = float3(vertices[vid].position.xy,0);

    float4 probe_eye_pos = light_positions[iid];

    float4 vertex_eye_position = float4(frameData.probe_size * vertex_position + probe_eye_pos.xyz, 1);

    out.position = frameData.projection_matrix * vertex_eye_position;

    // Pass probe color through
    out.color = half3(light_data[iid].light_color.xyz);

    // Convert model position which ranges from [-1, 1] to texture coordinates which ranges
    // from [0-1]
    out.tex_coord = 0.5 * (half2(vertices[vid].position.xy) + 1);

    return out;
}

fragment half4 probe_fragment(ProbeInOut      in       [[ stage_in ]],
                              texture2d<half> colorMap [[ texture(TextureIndexAlpha) ]])
{
    constexpr sampler linearSampler (mip_filter::linear,
                                     mag_filter::linear,
                                     min_filter::linear);

    half4 c = colorMap.sample(linearSampler, float2(in.tex_coord));

    half3 fragColor = in.color * c.x;

    return half4(fragColor, c.x);
}

