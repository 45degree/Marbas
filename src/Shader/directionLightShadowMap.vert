#version 450

layout(location = 0) in vec3 aPos;

layout(push_constant) uniform Constant {
  mat4 model;
  int lightIndex;
};

void main() {
  gl_Position = model * vec4(aPos, 1.0);
}
