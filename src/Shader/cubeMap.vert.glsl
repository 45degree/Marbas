#version 450

layout(location = 0) in vec3 aPos;

layout(location = 0) out vec3 TexCoords;

layout(std140, binding = 0) uniform Matrices {
  mat4 model;
  mat4 view;
  mat4 projection;
};

void main() {
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
