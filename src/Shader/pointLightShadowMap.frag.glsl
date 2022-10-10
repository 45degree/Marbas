#version 450

layout(location = 0) in vec4 FragPos;

layout(std140, binding = 1) uniform LightInfo {
  vec3 pos;
  float farPlane;
  mat4 matrices[6];
  mat4 projectMatrix;
  int lightIndex;
};

void main() {
  float lightDistance = length(FragPos.xyz - pos);
  lightDistance = lightDistance / farPlane;
  gl_FragDepth = lightDistance;
}
