#version 450

#define MAX_DIRECTIONAL_LIGHT_COUNT 32
#define CASCADE_COUNT 3

#extension GL_GOOGLE_include_directive : enable

#include "common/Shadow.glsl"
#include "common/MicroFace.glsl"

layout(location = 0) in vec2 inTex;
layout(location = 0) out vec4 outColor;

layout(binding = 0, set = 0) uniform sampler2D gDiffuse;
layout(binding = 1, set = 0) uniform sampler2D gNormalMetallic;
layout(binding = 2, set = 0) uniform sampler2D gPositionRoughness;
layout(binding = 3, set = 0) uniform sampler2D gAO;
layout(binding = 4, set = 0) uniform sampler2DArray directionalShadowMap;
layout(binding = 5, set = 0) uniform sampler2D gIndirectDiffuse;
layout(binding = 6, set = 0) uniform sampler2D gIndirectSpecular;

layout(std140, binding = 0, set = 1) uniform DirectionalLightList {
  int directionLightCount;
  DirectionLightInfo lightInfo[MAX_DIRECTIONAL_LIGHT_COUNT];
};

layout(std140, binding = 0, set = 2) uniform Camera {
  vec3 cameraPos;
  mat4 cameraView;
};

/**
 * @brief calculate the direct light color in PBR
 *
 * @param N the normal vector of shading point
 * @param L the light vector(shading point -> light)
 * @param V the camera veiew vector(shading point -> camera pos)
 * @param radiance the radiance of light(light color * light energy)
 * @param albedo the albedo of shading point
 * @param metallic the metallic of shading point
 * @param roughness the roughness of shading point
 */
vec3
CalculateDirectLightColor(vec3 N, vec3 L, vec3 V, vec3 radiance, vec3 albedo, float metallic, float roughness) {
  vec3 H = normalize(V + L);

  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic);
  vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
  float NDF = DistributionGGX(N, H, roughness);
  float G = GeometrySmith(N, V, L, roughness);

  vec3 nominator = NDF * G * F;
  float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
  vec3 specular = nominator / denominator;

  vec3 KS = F;
  vec3 KD = vec3(1.0) - KS;
  KD *= 1.0 - metallic;

  float NDotL = max(dot(N, L), 0.0);

  return (KD * albedo / PI + specular) * radiance * NDotL;
}

void main() {
  vec3 worldPos = texture(gPositionRoughness, inTex).xyz;
  vec3 normal = texture(gNormalMetallic, inTex).xyz;
  vec3 albedo = texture(gDiffuse, inTex).xyz;
  float metallic = texture(gNormalMetallic, inTex).w;
  float roughness = texture(gPositionRoughness, inTex).w;
  float ao = texture(gAO, inTex).x;

  vec3 V = normalize(cameraPos - worldPos);

  vec3 color = vec3(0, 0, 0);

  // calculate unshadow light
  for(int i = 0; i < directionLightCount; i++) {
    float shadow = 0.0;
    vec3 L = -lightInfo[i].directionShadow.xyz;
    vec3 radiance = lightInfo[i].colorEnergy.xyz * lightInfo[i].colorEnergy.w;
    vec3 Lo = CalculateDirectLightColor(normal, L, V, radiance, albedo, metallic, roughness);
    if(lightInfo[i].directionShadow.w > 0.5) {
      shadow = DirectionLightShadow(worldPos, normal, cameraView, directionalShadowMap, lightInfo[i]);
    }
    color += (1 - shadow) * Lo;
  }

  color += 2 * (texture(gIndirectSpecular, inTex).xyz + texture(gIndirectDiffuse, inTex).xyz);
  // color += 2 * (texture(gIndirectDiffuse, inTex).xyz);

  // color += vec3(0.03) * albedo; // * ao;
  color = color / (color + vec3(1.0));
  color = pow(color, vec3(1.0/2.2));

  outColor = vec4(color, 1.0);
}
