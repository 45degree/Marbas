#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

layout(location = 0) out vec4 FragPos;    // FragPos from GS (output per emitvertex)
layout(location = 1) out vec4 FragNormal;

layout(std140, binding = 1) uniform CameraInfo {
  mat4 projectMatrix;
  mat4 matrices[6];
};

in VS_OUT {
  layout(location = 0) vec3 normal;
} gs_in[];

void main() {
  for(int face = 0; face < 6; ++face) {
    gl_Layer = face; // built-in variable that specifies to which face we render.
    for(int i = 0; i < 3; ++i) {
      // for each triangle's vertices
      FragPos = gl_in[i].gl_Position;
      FragNormal = vec4(gs_in[i].normal, 1);
      gl_Position = projectMatrix * matrices[face] * FragPos;
      EmitVertex();
    }
    EndPrimitive();
  }
}
