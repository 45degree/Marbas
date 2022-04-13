#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in ivec2 aTexID;

layout(location = 0) out vec2 ourTex;
layout(location = 1) out ivec2 texId;

layout(std140, binding = 0) uniform Matrices {
    mat4 model;
    mat4 view;
    mat4 projection;
};

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourTex = aTexCoord;
    texId = aTexID;
}
