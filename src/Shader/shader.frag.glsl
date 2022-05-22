#version 450

layout(location = 0) in vec2 ourTex;

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D diffuseTexture;

void main() {
  vec4 color = texture(diffuseTexture, ourTex);
  color[3] = 1;
  FragColor = color;
}
