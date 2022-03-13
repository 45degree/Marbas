#version 450

layout(location = 0) out vec4 FragColor;
layout(location = 3) in vec3 ourColor;
layout(location = 4) in vec2 ourTex;

layout(binding = 0) uniform sampler2D u_Texture;
layout(binding = 1) uniform sampler2D ourTexture2;

void main() {
    /* FragColor = mix(texture(ourTexture, ourTex), texture(ourTexture2, ourTex), 0.2); */
    /* FragColor = texture(u_Texture, ourTex); */
    FragColor = vec4(1, 1, 1, 1);
}
