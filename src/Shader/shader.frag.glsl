#version 450

layout(location = 0) in vec2 ourTex;
layout(location = 1) flat in ivec2 texId;

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D ourTextures[32];

void main() {
    vec4 texColor = texture(ourTextures[0], ourTex);

    FragColor = texColor;
}
