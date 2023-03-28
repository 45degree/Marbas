#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(location = 0) out vec3 normal;

layout(std140, binding = 0) uniform MeshInfo {
  mat4 model;
  bool hasNormalTex;
  bool hasAOTex;
  bool hasRoughnessTex;
  bool hasMetallicTex;
} meshInfo;

void main() {
  gl_Position = meshInfo.model * vec4(aPos, 1.0);
  normal = aNormal;
}
