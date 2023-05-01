#version 450

#extension GL_GOOGLE_include_directive : enable

// TODO: add this to xmake option
#ifndef SSAO_SAMPLE_COUNT
#define SSAO_SAMPLE_COUNT 64
#endif

#include "common/Random.glsl"

layout(location = 0) in vec2 inTex;
layout(location = 0) out float outColor;

layout(binding = 0, set = 0) uniform sampler2D posTexture;
layout(binding = 1, set = 0) uniform sampler2D normalTexture;
layout(binding = 2, set = 0) uniform sampler2D depthTexture;

layout(std140, binding = 0, set = 1) uniform CameraInfo {
  float near;
  float far;
  vec3 position;
  mat4 viewMatrix;
  mat4 projectMatrix;
};

vec3 samplePoint[SSAO_SAMPLE_COUNT];

float
lerp(float a, float b, float f) {
  return a + f * (b - a);
}

void
GenerateSamplePoint(vec3 pos) {
  for(int i = 0; i < SSAO_SAMPLE_COUNT; i++) {
    float scale = float(i) / SSAO_SAMPLE_COUNT;
    scale = lerp(0.0, 1.0, scale * scale);

    vec3 point = Random3DTo3D(pos + vec3(i, i, i));
    point.xy = point.xy * 2.0 - 1.0;
    point *= scale;

    samplePoint[i] = point;
  }
}

float
UnpackDepthToView(float depth, float near, float far) {
  /**
   * Z_c = far / (far - near) * Z_v - near * far / (far - near)
   * W_c = Z_v
   * depth = Z_c / W_c
   */
  return near * far / (far - depth * (far - near));
}


void
main() {
  vec3 pos = texture(posTexture, inTex).xyz;
  vec3 normal = texture(normalTexture, inTex).xyz;

  GenerateSamplePoint(pos);
  vec3 randomRotate = vec3(0, 0, 0);
  randomRotate.xy = Random3DTo2D(pos) * 2.0 - 1.0;

  vec3 tangent = normalize(randomRotate - normal * dot(randomRotate, normal));
  vec3 bitangent = cross(normal, tangent);
  mat3 TBN = mat3(tangent, bitangent, normal);

  float occlusion = 0.0;
  for(int i = 0; i < SSAO_SAMPLE_COUNT; i++) {
    vec3 samplePoint = TBN * samplePoint[i] + pos;
    vec4 samplePointInView = viewMatrix * vec4(samplePoint, 1.0);
    float depth = samplePointInView.z;
    vec4 sampleInNDC =  projectMatrix * samplePointInView;
    sampleInNDC.xyz /= sampleInNDC.w;
    sampleInNDC.xy = sampleInNDC.xy * 0.5 + 0.5; // convert to [0, 1]

    float sampleDepth = -UnpackDepthToView(texture(depthTexture, sampleInNDC.xy).r, near, far);
    occlusion += (sampleDepth >= depth ? 1.0 : 0.0);
  }
  occlusion = 1.0 - (occlusion / SSAO_SAMPLE_COUNT);
  outColor = occlusion;
}
