#version 450

layout(location = 0) out vec3 nearPoint;
layout(location = 1) out vec3 farPoint;

layout(std140, binding = 0) uniform Matrices {
  mat4 view;
  mat4 perspective;
  vec3 right;
  vec3 up;
  vec3 pos;
} view;

vec3 gridPlane[6] = vec3[] (
  vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
  vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) {
  mat4 viewInv = inverse(view);
  mat4 projInv = inverse(projection);
  vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
  return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
#ifdef OPENGL
  vec3 p = gridPlane[gl_VertexID].xyz;
#else
  vec3 p = gridPlane[gl_VertexIndex].xyz;
#endif
  nearPoint = UnprojectPoint(p.x, p.y, 0.0, view.view, view.perspective).xyz; // unprojecting on the near plane
  farPoint = UnprojectPoint(p.x, p.y, 1.0, view.view, view.perspective).xyz; // unprojecting on the far plane
  gl_Position = vec4(p, 1.0);
}

