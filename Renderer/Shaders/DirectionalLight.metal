
#include <metal_stdlib>

using namespace metal;

#include "ShaderTypes.h"

#include "ShaderCommon.h"

struct QuadInOut
{
    float4 position [[position]];
    
    float3 eye_position;
};

vertex QuadInOut
deferred_direction_lighting_vertex(constant SimpleVertex * vertices  [[ buffer(BufferIndexMeshPositions) ]],
                                   constant FrameData    & frameData [[ buffer(BufferIndexFrameData) ]],
                                   uint                    vid       [[ vertex_id ]])
{
    QuadInOut out;

    out.position = float4(vertices[vid].position, 0, 1);

    float4 unprojected_eye_coord = frameData.projection_matrix_inverse * out.position;
    out.eye_position = unprojected_eye_coord.xyz / unprojected_eye_coord.w;

    return out;
}

half4
deferred_directional_lighting_fragment_common(QuadInOut            in,
                                              constant FrameData & frameData,
                                              float                depth,
                                              half4                normal_shadow,
                                              half4                albedo_specular)
{

    half sun_diffuse_intensity = dot(normal_shadow.xyz, half3(frameData.sun_eye_direction.xyz));

    sun_diffuse_intensity = max(sun_diffuse_intensity, 0.h);

    half3 sun_color = half3(frameData.sun_color.xyz);

    half3 diffuse_contribution = albedo_specular.xyz * sun_diffuse_intensity * sun_color;

    // Calculate specular contributioh from directional light

    // Used eye_space depth to determine the position of the fragment in eye_space
    float3 eye_space_fragment_pos = normalize(in.eye_position) * depth;


    float4 eye_light_direction = frameData.sun_eye_direction;

    // Specular Contribution
    float3 halfway_vector = normalize(eye_space_fragment_pos - eye_light_direction.xyz );

    half specular_intensity = half(frameData.sun_specular_intensity);

    half specular_shininess = albedo_specular.w * half(frameData.shininess_factor);

    half specular_factor = powr(max(dot(half3(normal_shadow.xyz),half3(halfway_vector)),0.0h), specular_intensity);

    half3 specular_contribution = specular_factor * half3(albedo_specular.xyz) * specular_shininess * sun_color;

    half3 color = diffuse_contribution + specular_contribution;

    // Shadow Contribution
    half shadowSample = normal_shadow.w;

    // Lighten the shadow to account for some ambience
    shadowSample += .1h;

    // Account for values greater than 1.0 (after lightening shadow)
    shadowSample = saturate(shadowSample);

    color *= shadowSample;

    return half4(color, 1);
}


fragment AccumLightBuffer
deferred_directional_lighting_fragment_single_pass(
    QuadInOut            in        [[ stage_in ]],
    constant FrameData & frameData [[ buffer(BufferIndexFrameData) ]],
    GBufferData          GBuffer)
{
    AccumLightBuffer output;
    output.lighting =
        deferred_directional_lighting_fragment_common(in, frameData, GBuffer.depth,  GBuffer.normal_shadow,  GBuffer.albedo_specular);

    return output;
}


fragment half4
deferred_directional_lighting_fragment_traditional(
    QuadInOut            in                      [[ stage_in ]],
    constant FrameData & frameData               [[ buffer(BufferIndexFrameData) ]],
    texture2d<half>      albedo_specular_GBuffer [[ texture(RenderTargetAlbedo) ]],
    texture2d<half>      normal_shadow_GBuffer   [[ texture(RenderTargetNormal) ]],
    texture2d<float>     depth_GBuffer           [[ texture(RenderTargetDepth)  ]])
{
    uint2 position = uint2(in.position.xy);

    float depth = depth_GBuffer.read(position.xy).x;
    half4 normal_shadow = normal_shadow_GBuffer.read(position.xy);
    half4 albedo_specular = albedo_specular_GBuffer.read(position.xy);

    return deferred_directional_lighting_fragment_common(in, frameData, depth, normal_shadow, albedo_specular);
}

