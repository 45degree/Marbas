#version 450

layout(location = 0) in vec2 ourTex;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aPosition;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec3 Position;
layout(location = 3) out float AmbientOccu;
layout(location = 4) out float Roughtness;
layout(location = 5) out float Metallic;

layout(binding = 0) uniform sampler2D diffuseTexture;
layout(binding = 1) uniform sampler2D aoTexture;
layout(binding = 2) uniform sampler2D normalTexture;
layout(binding = 3) uniform sampler2D roughnessTexture;
layout(binding = 4) uniform sampler2D metallicTexture;

layout(std140, binding = 1) uniform Matrices {
  mat4 model;
  bool hasNormalTex;
  bool hasAOTex;
  bool hasRoughnessTex;
  bool hasMetallicTex;
};

void main() {
  vec4 color = texture(diffuseTexture, ourTex);
  color[3] = 1;
  FragColor = color;
  if(hasNormalTex) {
    vec3 normal = texture(normalTexture, ourTex).rgb;
    Normal = normalize(normal * 2.0 - 1.0);
    Normal = normalize(TBN * Normal);
  }
  else {
    Normal = normalize(aNormal);
  }

  if(hasAOTex) {
    AmbientOccu = texture(aoTexture, ourTex).r;
  }
  else {
    AmbientOccu = 1.0;
  }

  if(hasRoughnessTex) {
    Roughtness = texture(roughnessTexture, ourTex).r;
  } else {
    Roughtness = 0.0;
  }

  if(hasMetallicTex) {
    Metallic = texture(metallicTexture, ourTex).r;
  } else {
    Metallic = 0.0;
  }

  Position = aPosition;
}
