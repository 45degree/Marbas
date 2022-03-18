#version 450

layout(location = 1) in vec2 ourTex;
layout(location = 0) out vec4 FragColor;

layout(binding = 3) uniform sampler2D ourTexture1;
layout(binding = 4) uniform sampler2D ourTexture2;

void main() {
    /* FragColor = mix(texture(ourTexture1, ourTex), texture(ourTexture2, ourTex), 0.2); */
    FragColor = texture(ourTexture1, ourTex);
    /* FragColor = vec4(1, 0, 0, 1); */
}
