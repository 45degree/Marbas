#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

layout(location = 0) out vec4 WorldPos;

layout(std140, binding = 0) uniform CameraMatrix {
  mat4 VIEW;
  mat4 PROJETION;
  vec3 RIGHT;
  vec3 UP;
  vec3 POS;
};

layout(std140, binding = 1) uniform ViewMatrix {
  mat4 viewMatrix[6];
  mat4 projectMatrix;
  float roughness;
};

void main() {
  for(int face = 0; face < 6; ++face) {
    gl_Layer = face; // built-in variable that specifies to which face we render.
    for(int i = 0; i < 3; ++i) {
      // for each triangle's vertices
      WorldPos = gl_in[i].gl_Position;
      gl_Position = projectMatrix * viewMatrix[face] * WorldPos;
      EmitVertex();
    }
    EndPrimitive();
  }
}
