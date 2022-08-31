#version 450

// #include "common/common.glsl"

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(std140, binding = 0) uniform CameraMatrix {
  mat4 VIEW;
  mat4 PROJETION;
  vec3 RIGHT;
  vec3 UP;
  vec3 POS;
};

layout(std140, binding = 1) uniform LightMatrix {
  mat4 view;
  mat4 projection;
};

layout(std140, binding = 2) uniform ModelMatrix {
  mat4 model;
};

void main() {
  vec4 position = model * vec4(aPos, 1.0);

  gl_Position = projection * view * position;
}
