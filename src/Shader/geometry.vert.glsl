#version 450

// #include "common/common.glsl"

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(location = 0) out vec2 ourTex;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec3 Position;
layout(location = 3) out mat3 TBN;

layout(std140, binding = 0) uniform CameraMatrix {
  mat4 view;
  mat4 projection;
  vec3 right;
  vec3 up;
  vec3 pos;
};

layout(std140, binding = 1) uniform Matrices {
  mat4 model;
  bool hasNormalTex;
  bool hasAOTex;
  bool hasRoughnessTex;
  bool hasMetallicTex;
};

void main() {
  ourTex = aTexCoord;

  mat3 normalMatrix = transpose(inverse(mat3(model)));
  Normal = normalMatrix * aNormal;

  vec4 position = model * vec4(aPos, 1.0);
  Position = position.xyz;

  vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
  vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
  vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));

  TBN = mat3(T, B, N);

  gl_Position = projection * view * position;
}
