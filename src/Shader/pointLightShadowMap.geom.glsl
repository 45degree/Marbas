#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

layout(location = 0) out vec4 FragPos; // FragPos from GS (output per emitvertex)

layout(std140, binding = 1) uniform LightInfo {
  vec3 pos;
  float farPlane;
  mat4 matrices[6];
  mat4 projectMatrix;
  int lightIndex;
};

void main() {
  for(int face = 0; face < 6; ++face) {
    gl_Layer = 6 * lightIndex + face; // built-in variable that specifies to which face we render.
    for(int i = 0; i < 3; ++i) {
      // for each triangle's vertices
      FragPos = gl_in[i].gl_Position;
      gl_Position = projectMatrix * matrices[face] * FragPos;
      EmitVertex();
    }
    EndPrimitive();
  }
}
