#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

// layout(location = 0) out vec4 FragPos; // FragPos from GS (output per emitvertex)

layout(std140, binding = 1) uniform LightMatrix {
  int lightIndex;
  mat4 view;
  mat4 projection;
};

void main() {
  gl_Layer = lightIndex;
  for(int i = 0; i < 3; ++i) {
    // for each triangle's vertices
    gl_Position = gl_in[i].gl_Position;
    EmitVertex();
  }
  EndPrimitive();
}
