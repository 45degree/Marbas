#version 450

layout (triangles, invocations = 6) in;
layout (triangle_strip, max_vertices=18) out;

layout(location = 0) out vec4 WorldPos;

layout(std140, binding = 0) uniform ViewMatrix {
  mat4 viewMatrix[6];
  mat4 projectMatrix;
};

void main() {
  gl_Layer = gl_InvocationID;
  for(int i = 0; i < 3; ++i) {
    WorldPos = gl_in[i].gl_Position;
    gl_Position = projectMatrix * viewMatrix[gl_InvocationID] * WorldPos;
    EmitVertex();
  }
  EndPrimitive();
}
