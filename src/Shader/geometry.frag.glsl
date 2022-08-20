#version 450

layout(location = 0) in vec2 ourTex;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aPosition;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec3 Position;

layout(binding = 0) uniform sampler2D diffuseTexture;

void main() {
  vec4 color = texture(diffuseTexture, ourTex);
  color[3] = 1;
  FragColor = color;
  Normal = normalize(aNormal);
  Position = aPosition;
}
