#version 450

layout(location = 0) in vec3 aPos;

layout(std140, binding = 1, set = 0) uniform ModelMatrix {
  mat4 model;
};

void main() {
  gl_Position = model * vec4(aPos, 1.0);
}
