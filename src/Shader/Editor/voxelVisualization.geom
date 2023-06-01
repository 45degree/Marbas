#version 450

layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

layout(location = 0) out flat vec4 voxelColor;

layout(binding = 0, set = 0) uniform sampler3D voxelImage;

const vec4 cubeVertices[8] = vec4[8] (
  vec4( 0.5f,  0.5f,  0.5f, 0.0f),
  vec4( 0.5f,  0.5f, -0.5f, 0.0f),
  vec4( 0.5f, -0.5f,  0.5f, 0.0f),
  vec4( 0.5f, -0.5f, -0.5f, 0.0f),
  vec4(-0.5f,  0.5f,  0.5f, 0.0f),
  vec4(-0.5f,  0.5f, -0.5f, 0.0f),
  vec4(-0.5f, -0.5f,  0.5f, 0.0f),
  vec4(-0.5f, -0.5f, -0.5f, 0.0f)
);

const int cubeIndices[24]  = int[24] (
  0, 2, 1, 3, // right
  6, 4, 7, 5, // left
  5, 4, 1, 0, // up
  6, 7, 2, 3, // down
  4, 6, 0, 2, // front
  1, 3, 5, 7  // back
);

layout(std140, binding = 0, set = 1) uniform VoxelInfo {
  vec2 voxelResolutionSize;
  vec3 voxelCenter;
};

layout(std140, binding = 1, set = 1) uniform CameraInfo {
  mat4 viewMatrix;
  mat4 projection;
};

void main() {
  // get the voxel center of world
  vec3 worldPos = gl_in[0].gl_Position.xyz;
  int voxelResolution = int(voxelResolutionSize.x);
  float voxelSize = voxelResolutionSize.y;
  int halfResolution = voxelResolution / 2;
  worldPos += vec3(-halfResolution, -halfResolution, -halfResolution);
  worldPos *= voxelSize;
  worldPos += voxelCenter;

  vec4 projPoint = projection * viewMatrix * vec4(worldPos, 1);
  projPoint /= projPoint.w;
  if(projPoint.x < -1 || projPoint.x > 1) return;
  if(projPoint.y < -1 || projPoint.y > 1) return;
  if(projPoint.z < 0 || projPoint.z > 1) return;

  // it's an empty voxel
  vec4 color = texelFetch(voxelImage, ivec3(gl_in[0].gl_Position.xyz), 0);
  if(color.a < 1e-5) return;

  for(int face = 0; face < 6; ++face)	{
    for(int vertex = 0; vertex < 4; ++vertex) {
      vec4 point = voxelSize * cubeVertices[cubeIndices[face * 4 + vertex]] + vec4(worldPos, 1.0);
      gl_Position = projection * viewMatrix * point;
      voxelColor = color;
      EmitVertex();
    }

    EndPrimitive();
  }
}
