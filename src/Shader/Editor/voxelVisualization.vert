#version 450

layout(std140, binding = 0, set = 1) uniform VoxelInfo {
  vec2 voxelResolutionSize;
  vec3 voxelCenter;
};

void main() {
  ivec3 voxelPos;
  int voxelResolution = int(voxelResolutionSize.x);
  voxelPos.z = gl_VertexIndex / (voxelResolution * voxelResolution);
  int _tmp = gl_VertexIndex % (voxelResolution * voxelResolution);
  voxelPos.y = _tmp / voxelResolution;
  voxelPos.x = _tmp % voxelResolution; 

  gl_Position = vec4(voxelPos, 1.0);
}
