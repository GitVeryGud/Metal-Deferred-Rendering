
#include <metal_stdlib>

using namespace metal;

#include "ShaderTypes.h"

#include "ShaderCommon.h"

// Per-vertex inputs fed by vertex buffer laid out with MTLVertexDescriptor in Metal API
struct DescriptorDefinedVertex
{
    float3 position  [[attribute(VertexAttributePosition)]];
    float2 tex_coord [[attribute(VertexAttributeTexcoord)]];
    half3 normal     [[attribute(VertexAttributeNormal)]];
    half3 tangent    [[attribute(VertexAttributeTangent)]];
    half3 bitangent  [[attribute(VertexAttributeBitangent)]];
};

struct ColorInOut
{
    float4 position [[position]];
    float2 tex_coord;
    float2 shadow_uv;
    half   shadow_depth;
    float3 eye_position;
    half3  tangent;
    half3  bitangent;
    half3  normal;
};

vertex ColorInOut gbuffer_vertex(DescriptorDefinedVertex  in        [[ stage_in ]],
                                 constant FrameData      &frameData [[ buffer(BufferIndexFrameData) ]])
{
    ColorInOut out;

    float4 model_position = float4(in.position, 1.0);
    // Make position a float4 to perform 4x4 matrix math on it
    float4 eye_position = frameData.temple_modelview_matrix * model_position;
    out.position = frameData.projection_matrix * eye_position;
    out.tex_coord = in.tex_coord;

    out.eye_position = eye_position.xyz;

    // Rotate tangents, bitangents, and normals by the normal matrix
    half3x3 normalMatrix = half3x3(frameData.temple_normal_matrix);

    float3 shadow_coord = (frameData.shadow_mvp_xform_matrix * model_position ).xyz;
    out.shadow_uv = shadow_coord.xy;
    out.shadow_depth = half(shadow_coord.z);

    // Calculate tangent, bitangent and normal in eye's space
    out.tangent = normalize(normalMatrix * in.tangent);
    out.bitangent = -normalize(normalMatrix * in.bitangent);
    out.normal = normalize(normalMatrix * in.normal);

    return out;
}

fragment GBufferData gbuffer_fragment(ColorInOut               in           [[ stage_in ]],
                                      constant FrameData      &frameData    [[ buffer(BufferIndexFrameData) ]],
                                      texture2d<half>          baseColorMap [[ texture(TextureIndexBaseColor) ]],
                                      texture2d<half>          normalMap    [[ texture(TextureIndexNormal) ]],
                                      texture2d<half>          specularMap  [[ texture(TextureIndexSpecular) ]],
                                      depth2d<float>           shadowMap    [[ texture(TextureIndexShadow) ]])
{
    constexpr sampler linearSampler(mip_filter::linear,
                                    mag_filter::linear,
                                    min_filter::linear);

    half4 base_color_sample = baseColorMap.sample(linearSampler, in.tex_coord.xy);
    half4 normal_sample = normalMap.sample(linearSampler, in.tex_coord.xy);
    half specular_contrib = specularMap.sample(linearSampler, in.tex_coord.xy).r;

    // Fill in on-chip geometry buffer data
    GBufferData gBuffer;

    // Calculate normal in eye space
    half3 tangent_normal = normalize((normal_sample.xyz * 2.0) - 1.0);

    half3 eye_normal = (tangent_normal.x * in.tangent +
                        tangent_normal.y * in.bitangent +
                        tangent_normal.z * in.normal);

    eye_normal = normalize(eye_normal);

    constexpr sampler shadowSampler(coord::normalized,
                                    filter::linear,
                                    mip_filter::none,
                                    address::clamp_to_edge,
                                    compare_func::less);


    half shadow_sample = shadowMap.sample_compare(shadowSampler, in.shadow_uv, in.shadow_depth);

    // Store shadow with albedo in unused fourth channel
    gBuffer.albedo_specular = half4(base_color_sample.xyz, specular_contrib);

    // Store the specular contribution with the normal in unused fourth channel.
    gBuffer.normal_shadow = half4(eye_normal.xyz, shadow_sample);

    gBuffer.depth = in.eye_position.z;

    return gBuffer;
}

