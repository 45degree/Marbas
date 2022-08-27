#version 450

// #include "common/common.glsl"

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(location = 0) out vec2 ourTex;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec3 Position;

layout(std140, binding = 0) uniform CameraMatrix {
  mat4 view;
  mat4 projection;
};

layout(std140, binding = 1) uniform Matrices {
  mat4 model;
};

void main() {
  ourTex = aTexCoord;

  mat3 normalMatrix = transpose(inverse(mat3(model)));
  Normal = normalMatrix * aNormal;

  vec4 position = model * vec4(aPos, 1.0);
  Position =position.xyz;

  gl_Position = projection * view * position;
}
