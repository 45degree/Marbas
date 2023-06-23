#version 450

#define MAX_DIRECTIONAL_LIGHT_COUNT 32
#define CASCADE_COUNT 3

#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_image_load_formatted : enable

#include "common/Shadow.glsl"

layout(location = 0) in flat int axis;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec2 InTex;
layout(location = 3) in vec3 InPos;

/**
 * input gbuffer
 */
layout(binding = 0, set = 0) uniform sampler2DArray directionalShadowMap;

/**
 * voxel info
 */
layout(binding = 0, set = 1, r32ui) uniform volatile coherent uimage3D voxelDiffuse;
layout(binding = 1, set = 1, r32ui) uniform volatile coherent uimage3D voxelNormal;
layout(std140, binding = 2, set = 1) uniform VoxelInfo{
  mat4 projX;
  mat4 projY;
  mat4 projZ;
  vec4 voxelSizeResolution;
  vec3 probePos;
};

/**
 *  mesh gpu data
 */
layout(std140, binding = 0, set = 2) uniform Matrices {
  uvec4 texInfo; // [diffuse, normal, metallic, roughness]
  vec4 diffuseColor;
  float metallicValue;
  float roughnessValue;
};
layout(binding = 1, set = 2) uniform sampler2D diffuseTexture;
layout(binding = 2, set = 2) uniform sampler2D normalTexture;
layout(binding = 3, set = 2) uniform sampler2D roughnessTexture;
layout(binding = 4, set = 2) uniform sampler2D metallicTexture;

/**
 * light info data
 */
layout(std140, binding = 0, set = 3) uniform DirectionalLightList {
  int shadowLightCount;
  int unshadowLightCount;
  int shadowLightIndexList[MAX_DIRECTIONAL_LIGHT_COUNT];
  int unshadowLightIndexList[MAX_DIRECTIONAL_LIGHT_COUNT];
  DirectionLightInfo lightInfo[MAX_DIRECTIONAL_LIGHT_COUNT];
};

vec3 EncodeNormal(vec3 normal) {
    return normal * 0.5f + vec3(0.5f);
}

vec3 DecodeNormal(vec3 normal) {
    return normal * 2.0f - vec3(1.0f);
}

vec4 convRGBA8ToVec4(uint val) {
    return vec4(float((val & 0x000000FF)), 
    float((val & 0x0000FF00) >> 8U), 
    float((val & 0x00FF0000) >> 16U), 
    float((val & 0xFF000000) >> 24U));
}

uint convVec4ToRGBA8(vec4 val) {
    return (uint(val.w) & 0x000000FF) << 24U | 
    (uint(val.z) & 0x000000FF) << 16U | 
    (uint(val.y) & 0x000000FF) << 8U | 
    (uint(val.x) & 0x000000FF);
}

#define imageAtomicRGBA8Avg(grid, coords, value) do {                                       \
    (value).rgb *= 255.0;                                                                   \
    uint newVal = convVec4ToRGBA8((value));                                                 \
    uint prevStoredVal = 0;                                                                 \
    uint curStoredVal;                                                                      \
    uint numIterations = 0;                                                                 \
                                                                                            \
    while((curStoredVal = imageAtomicCompSwap((grid), (coords), prevStoredVal, newVal))     \
            != prevStoredVal                                                                \
            && numIterations < 255)                                                         \
    {                                                                                       \
        prevStoredVal = curStoredVal;                                                       \
        vec4 rval = convRGBA8ToVec4(curStoredVal);                                          \
        rval.rgb = (rval.rgb * rval.a);                                                     \
        vec4 curValF = rval + (value);                                                      \
        curValF.rgb /= curValF.a;                                                           \
        newVal = convVec4ToRGBA8(curValF);                                                  \
        ++numIterations;                                                                    \
    }                                                                                       \
}while(false);

void main() {
  // the framebuffer size must be equal to the voxel resolution;
  int voxelResolution = int(voxelSizeResolution.w);
  ivec3 camPos = ivec3(gl_FragCoord.xy, voxelResolution * gl_FragCoord.z);
  ivec3 voxelPos;
  if( axis == 0 ) {
    // camera view from x axis to -x axis, up vector is (0, 1, 0)
    voxelPos.z = voxelResolution - 1 - camPos.x;
    voxelPos.y = voxelResolution - 1 - camPos.y;
    voxelPos.x = voxelResolution - 1 - camPos.z;
  } else if( axis == 1 ) {
    // camera view from y axis to -y axis, up vector is (0, 0, -1)
    voxelPos.z = camPos.y; // voxelResolution - 1 - camPos.y; // camPos.y
    voxelPos.x = camPos.x;
    voxelPos.y = voxelResolution - 1 - camPos.z;
  } else if( axis == 2) {
    // camera view from z axis to -z axis, up vector is (0, 1, 0)
    voxelPos.x = camPos.x;
    voxelPos.y = voxelResolution - 1 - camPos.y;
    voxelPos.z = voxelResolution - 1 - camPos.z;
  }

  // add shadowmap
  vec4 color = vec4(0);
  if(texInfo.x == 1) {
    color = pow(texture(diffuseTexture, InTex), vec4(2.2));
    // color = texture(diffuseTexture, InTex);
    color.w = 1;
  }else {
    color = diffuseColor;
  }

  // color = vec4(InPos, axis);
  imageAtomicRGBA8Avg(voxelDiffuse, voxelPos, color);

  vec4 normal = vec4(EncodeNormal(normalize(InNormal.xyz)), 1.0);
  imageAtomicRGBA8Avg(voxelNormal, voxelPos, normal);
  // imageStore(voxelDiffuse, voxelPos, color);
  // imageStore(voxelNormal, voxelPos, vec4(InNormal, 1));
}

