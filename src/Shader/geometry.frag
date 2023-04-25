#version 450

layout(location = 0) in vec2 ourTex;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aPosition;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 Position;
layout(location = 2) out vec4 Normal;

layout(std140, binding = 0, set = 0) uniform Matrices {
  uvec4 texInfo; // [diffuse, normal, roughness, metallic]
  vec4 diffuseColor;
  float metallicValue;
  float roughnessValue;
};

layout(std140, binding = 1, set = 0) uniform ModelMatrix {
  mat4 model;
};

layout(binding = 2, set = 0) uniform sampler2D diffuseTexture;
layout(binding = 3, set = 0) uniform sampler2D normalTexture;
layout(binding = 4, set = 0) uniform sampler2D roughnessTexture;
layout(binding = 5, set = 0) uniform sampler2D metallicTexture;

void main() {

  if(texInfo.x == 1) {
    vec4 color = texture(diffuseTexture, ourTex);
    color[3] = 1;
    FragColor = color;
  }else {
    FragColor = diffuseColor;
  }

  Normal = vec4(0);
  if(texInfo.y == 1) {
    vec3 normal = texture(normalTexture, ourTex).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    Normal.xy = normalize(TBN * normal).xy;
  }
  else {
    Normal.xy = normalize(aNormal).xy;
  }

  if(texInfo.z == 1) {
    Normal.w = texture(roughnessTexture, ourTex).r;
  }

  if(texInfo.w == 1) {
    Normal.z = texture(metallicTexture, ourTex).r;
  } 

  Position = vec4(aPosition, 1.0);
}
