#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 outTex;

layout(push_constant) uniform Model {
  mat4 model;
};

void main() {
  gl_Position = model * vec4(aPos, 1.0);
  outNormal = aNormal;
  outTex = aTexCoord;
}
