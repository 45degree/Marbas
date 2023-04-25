#version 450

#define MAX_DIRECTIONAL_LIGHT_COUNT 32
#define CASCADE_COUNT 3

#extension GL_GOOGLE_include_directive : enable

#include "common/Shadow.glsl"

layout(location = 0) in vec2 inTex;
layout(location = 0) out vec4 outColor;

layout(binding = 0, set = 0) uniform sampler2D gDiffuse;
layout(binding = 1, set = 0) uniform sampler2D gNormal_Metallic_Roughness;
layout(binding = 2, set = 0) uniform sampler2D gPosition;
layout(binding = 3, set = 0) uniform sampler2D gAO;
layout(binding = 4, set = 0) uniform sampler2DArray directionalShadowMap;

layout(std140, binding = 0, set = 1) uniform DirectionalLightList {
  int shadowLightCount;
  int unshadowLightCount;
  int shadowLightIndexList[MAX_DIRECTIONAL_LIGHT_COUNT];
  int unshadowLightIndexList[MAX_DIRECTIONAL_LIGHT_COUNT];
  DirectionLightInfo lightInfo[MAX_DIRECTIONAL_LIGHT_COUNT];
};

layout(std140, binding = 0, set = 2) uniform Camera {
  mat4 cameraView;
};

void main() {
  vec3 worldPos = texture(gPosition, inTex).xyz;
  vec3 color = vec3(0, 0, 0);

  for(int i = 0; i < shadowLightCount; i++) {
    int index = shadowLightIndexList[i];
    float shadow = DirectionLightShadow(worldPos, cameraView, directionalShadowMap, lightInfo[index]);

    // TODO: use PBR
    color += (1 - shadow) * texture(gDiffuse, inTex).xyz;
  }

  outColor = vec4(color, 1.0);
}
