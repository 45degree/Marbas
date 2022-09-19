#version 450

layout(location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform CameraMatrix {
  mat4 VIEW;
  mat4 PROJETION;
  vec3 RIGHT;
  vec3 UP;
  vec3 POS;
};

void main() {
  gl_Position = vec4(aPos, 1.0);
}
