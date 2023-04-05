#version 450

#extension GL_GOOGLE_include_directive : enable

#include "common/Common.glsl"

layout(location = 0) in vec3 TexCoords;
layout(location = 0) out vec4 FragColor;

layout(binding = 0, set = 1) uniform sampler2D skybox;

layout(std140, binding = 0, set = 0) uniform Matrices {
  mat4 view;
  mat4 projection;
};

layout(std140, binding = 1, set = 0) uniform ClearValue {
  uint isClear;
  vec4 clearValue;
};

void main() {
  if(isClear == 1) {
    FragColor = clearValue;
  }else {
    vec2 uv = DirToCubemapUV2D(TexCoords);
    FragColor = texture(skybox, uv);
  }
}
