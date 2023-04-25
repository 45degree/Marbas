#version 450

layout(location = 0) in vec4 FragPos;
layout(location = 1) in vec4 FragNormal;

layout(location = 0) out vec4 pos;
layout(location = 1) out vec4 normal;

void main() {
  pos = FragPos;
  normal = FragNormal;
}
