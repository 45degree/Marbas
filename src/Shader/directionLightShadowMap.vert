#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(std140, binding = 0) uniform ModelMatrix {
  mat4 model;
};

void main() {
  gl_Position = model * vec4(aPos, 1.0);
}
