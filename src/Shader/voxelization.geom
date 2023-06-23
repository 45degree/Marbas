#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

layout(location = 0) in vec3 inNormal[];
layout(location = 1) in vec2 inTexCoor[];

layout(location = 0) out flat int outAxis;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTex;
layout(location = 3) out vec3 outPos;

layout(std140, binding = 2, set = 1) uniform ProjMatrix {
  mat4 projX;
  mat4 projY;
  mat4 projZ;
  vec4 voxelSizeResolution;
  vec3 probePos;
};


void main() {
  vec3 edge1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
  vec3 edge2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
  vec3 normal = abs(cross(edge1, edge2));

  int axis = 0;
  if(normal.x >= normal.y && normal.x >= normal.z) {
    axis = 0;
  } else if (normal.y >= normal.x && normal.y >= normal.z) {
    axis = 1;
  } else {
    axis = 2;
  }

  mat4 projectMat = axis == 0 ? projX : axis == 1 ? projY : projZ;

  for(int i = 0; i < 3; i++) {
    vec4 worldPos = gl_in[i].gl_Position;
    gl_Position = projectMat * worldPos;
    outTex = inTexCoor[i];
    outAxis = axis;
    outNormal = inNormal[i];
    outPos = worldPos.xyz;
    EmitVertex();
  }
  EndPrimitive();
}
