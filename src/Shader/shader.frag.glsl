#version 450

layout(location = 0) in vec2 ourTex;

layout(location = 0) out vec4 FragColor;

layout(std140, binding = 1) uniform SceneInfo {
  vec3 cameraPos;
};

layout(binding = 0) uniform samplerCube skybox;
layout(binding = 1) uniform sampler2D diffuseTexture;

void main() {
  FragColor = texture(diffuseTexture, ourTex);
}
