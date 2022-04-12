#version 450

layout(location = 0) in vec2 ourTex;
layout(location = 1) in vec2 texId;

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D ourTextures[32];

void main() {
    /* FragColor = mix(texture(ourTexture1, ourTex), texture(ourTexture2, ourTex), 0.2); */
    int id = int(texId[0]);
    /* if(id != -1) { */
    /*     FragColor = texture(ourTextures[id], ourTex); */
    /* } */

    if(id == 0) {
        FragColor = vec4(1, 0, 0, 1);
    }
    else if(id == 1) {
        FragColor = vec4(0, 1, 0, 1);
    }
    else if(id == 2) {
        FragColor = vec4(0, 0, 1, 1);
    }
    else if(id == 3) {
        FragColor = vec4(1, 1, 0, 1);
    }
    else if(id == 4) {
        FragColor = vec4(0, 1, 1, 1);
    }
    else {
        FragColor = vec4(1, 1, 1, 1);
    }
}
