#version 450

#define MAX_DIRECTIONAL_LIGHT_COUNT 32
#define CASCADE_COUNT 3

#extension GL_GOOGLE_include_directive : enable

#include "common/Shadow.glsl"

layout(location = 0) in flat int axis;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec2 InTex;
layout(location = 3) in vec3 InPos;

/**
 * input gbuffer
 */
layout(binding = 0, set = 0) uniform sampler2DArray directionalShadowMap;
layout(binding = 1, set = 0, rgba32f) uniform writeonly image3D voxelScene;

/**
 * voxel info
 */
layout(std140, binding = 0, set = 1) uniform VoxelInfo{
  mat4 projX;
  mat4 projY;
  mat4 projZ;
  mat4 viewMatrix;
  int voxelResolution;
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

void main() {
  // the framebuffer size must be equal to the voxel resolution;
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
  vec4 color;
  if(texInfo.x == 1) {
    color = texture(diffuseTexture, InTex);
  }else {
    color = diffuseColor;
  }
  color.w = 1;

  // calculate shadow
  // for(int i = 0; i < shadowLightCount; i++) {
  //   int index = shadowLightIndexList[i];
  //   float shadow = DirectionLightShadow(InPos, InNormal, viewMatrix, directionalShadowMap, lightInfo[index]);
  //   color.xyz *= (1 - shadow);
  // }

  // color = vec4(InPos, axis);
  imageStore(voxelScene, voxelPos, color);
}

