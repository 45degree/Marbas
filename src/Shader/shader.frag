#version 450

layout(location = 1) in vec2 ourTex;
layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D ourTextures[32];

void main() {
    /* FragColor = mix(texture(ourTexture1, ourTex), texture(ourTexture2, ourTex), 0.2); */
    FragColor = texture(ourTextures[0], ourTex);
    /* FragColor = vec4(1, 0, 0, 1); */
}
