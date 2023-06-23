#version 450

#extension GL_GOOGLE_include_directive : enable
#include "common/MicroFace.glsl"

layout(location = 0) in vec2 inTex;
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outReflectColor;

layout(binding = 0, set = 0) uniform sampler2D gWorldPosRoughness;
layout(binding = 1, set = 0) uniform sampler2D gNormalMetallic;
layout(binding = 2, set = 0) uniform sampler2D gDiffuse;

layout(binding = 0, set = 1) uniform sampler3D voxelRadiance;
layout(std140, binding = 1, set = 1) uniform VoxelInfo {
  vec4 voxelSizeResolution;
  vec3 probePos;
};

layout(binding = 0, set = 2) uniform CameraInfo {
  vec3 cameraPos;
};

float coneWeights[6] = float[](0.25, 0.15, 0.15, 0.15, 0.15, 0.15);//各锥体权重

vec3 coneDirections[6] = vec3[] ( 
  vec3(0, 0, 1),
  vec3(0, 0.866025,0.5),
  vec3(0.823639, 0.267617, 0.5),
  vec3(0.509037, -0.700629, 0.5),
  vec3(-0.509037, -0.700629, 0.5),
  vec3(-0.823639, 0.267617, 0.5)
);

/**
 * @brief calculate the direct light color in PBR
 *
 * @param N the normal vector of shading point
 * @param L the light vector(shading point -> light)
 * @param V the camera veiew vector(shading point -> camera pos)
 * @param albedo the albedo of shading point
 * @param metallic the metallic of shading point
 * @param roughness the roughness of shading point
 */
vec3
CalculateSpecular(vec3 N, vec3 V, vec3 albedo, float metallic, float roughness) {
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic);
  return FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
}

vec4
SampleVoxels(vec3 worldPos, float lod) {
	vec3 clipPos = (worldPos - probePos) / (voxelSizeResolution.xyz * voxelSizeResolution.w);
	vec3 ndc = clipPos + 0.5;
	return textureLod(voxelRadiance, ndc, lod);
}

vec4
ConeTracing(vec3 pos, vec3 direction, vec3 normal, float tanHalfAngle, float maxDist, float alphaThresh){
  float voxelWorldSize = voxelSizeResolution.x;
	vec3 start = pos + normal * voxelWorldSize;

	float d = voxelWorldSize;

  vec3 color = vec3(0);
	float alpha = 0.0;
	float occlusion = 0.0;

  while(d < maxDist && alpha< alphaThresh){
     float diameter = max(voxelWorldSize, 2.0 * tanHalfAngle * d);
     float lodLevel = log2(diameter / voxelWorldSize);

     vec4 voxelColor = SampleVoxels(start + direction * d, lodLevel);
     occlusion += ((1.0 - alpha) * voxelColor.a) / (1.0 + 0.03 * diameter);
     color += (1.0 - alpha) * voxelColor.rgb;
     alpha += (1.0 - alpha) * voxelColor.a;
     d += diameter;
  }
  return vec4(color, occlusion);
}

void main() {
  vec3 worldPos = texture(gWorldPosRoughness, inTex).xyz;
  float roughness = texture(gWorldPosRoughness, inTex).w;
  vec3 normal = texture(gNormalMetallic, inTex).xyz;
  float metallic = texture(gNormalMetallic, inTex).w;
  vec4 diffuse = texture(gDiffuse, inTex);

  vec3 wo = normalize(cameraPos - worldPos);
  vec3 N = normalize(normal);

  // check if the pos is in the probe
  vec3 posDir = abs(worldPos - probePos);
  vec3 halfProbeWorldSize = voxelSizeResolution.xyz * voxelSizeResolution.w * 0.5;
  if(posDir.x > halfProbeWorldSize.x || posDir.y > halfProbeWorldSize.y || posDir.z > halfProbeWorldSize.z) {
    outColor = vec4(0, 0, 0, 0);
    outReflectColor = vec4(0, 0, 0, 0);
    return;
  }

  /**
   * use gram schmidt orthogonalization to generate a random TBN
   */

  // guide cannot be collinear with normal
	vec3 guide = vec3(0.0f, 1.0f, 0.0f);
	if (abs(dot(N, guide)) > 0.9f)
	  guide = vec3(0.0f, 0.0f, 1.0f);

	vec3 T = normalize(guide - dot(N, guide) * N);
	vec3 B = cross(T, N);
  mat3 TBN = mat3(T,B,N);

  float maxDistance = voxelSizeResolution.x * voxelSizeResolution.w;

  //叠加六个锥体
  vec4 diffuseReflection = vec4(0, 0, 0, 0);
  for(int i = 0; i < 6; i++) {
    diffuseReflection += coneWeights[i] * ConeTracing(worldPos, normalize(TBN * coneDirections[i]) , N, 0.577, maxDistance, 0.95);
  }
  outColor.xyz = diffuse.xyz * diffuseReflection.xyz;
  outColor.a = 1;

  // 计算反射
  vec3 reflectDir = reflect(-wo, normal);
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, diffuse.xyz, metallic);

  float aperture = clamp(tan(HALF_PI * roughness), 0.0174533, PI);
  outReflectColor.xyz = ConeTracing(worldPos, reflectDir, N, aperture, maxDistance, 0.95).xyz;
  outReflectColor.xyz = F0 * outReflectColor.xyz;
}
