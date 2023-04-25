#version 450

#extension GL_GOOGLE_include_directive : enable

#include "common/Common.glsl"

#define MAX_DIRECTIONAL_LIGHT_COUNT 32
#define CASCADE_COUNT 3

layout (triangles, invocations = CASCADE_COUNT + 1) in;
layout (triangle_strip, max_vertices=3) out;

layout(std140, binding = 0, set = 1) uniform DirectionalLightList {
  int shadowLightCount;
  int unshadowLightCount;
  int shadowLightIndexList[MAX_DIRECTIONAL_LIGHT_COUNT];
  int unshadowLightIndexList[MAX_DIRECTIONAL_LIGHT_COUNT];
  DirectionLightInfo lightInfo[MAX_DIRECTIONAL_LIGHT_COUNT];
};

layout(push_constant) uniform CurrentLight {
  int lightIndex;
};

void main() {
  for (int i = 0; i < 3; ++i) {
    gl_Position = lightInfo[lightIndex].lightMatrix[gl_InvocationID] * gl_in[i].gl_Position;
    gl_Layer = gl_InvocationID;
    EmitVertex();
  }
  EndPrimitive();
}
