#version 450

layout(location = 0) in vec3 nearPoint;
layout(location = 1) in vec3 farPoint;
layout(location = 0) out vec4 outColor;

layout(std140, binding = 0) uniform Matrices {
  mat4 view;
  mat4 perspective;
  vec3 right;
  vec3 up;
  vec3 pos;
} view;

vec4 grid(vec3 fragPos3D, float scale, bool drawAxis) {
  vec2 coord = fragPos3D.xz * scale;
  vec2 derivative = fwidth(coord);
  vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
  float line = min(grid.x, grid.y);

  float minimumz = min(derivative.y, 1);
  float minimumx = min(derivative.x, 1);

  vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
  // z axis
  if(fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
      color = vec4(0, 0, 1, 1);
  // x axis
  if(fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
      color = vec4(1, 0, 0, 1);
  return color;
}

vec4 grid2(vec3 fragPos3D, float gridSize) {
  float wx = fragPos3D.x;
  float wz = fragPos3D.z;

  float x0 = abs(fract(wx / gridSize ) - 0.5) / fwidth(wx) * gridSize / 2.0;
  float z0 = abs(fract(wz / gridSize ) - 0.5) / fwidth(wz) * gridSize / 2.0;

  float v0 = 1.0 - clamp(min(x0, z0), 0.0, 1.0);
  vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(v0, 1.0));
  return color;
}

float computeDepth(vec3 pos) {
  vec4 clip_space_pos = view.perspective * view.view * vec4(pos.xyz, 1.0);
  float clip_space_depth = clip_space_pos.z / clip_space_pos.w;
  float far = 1.0;
  float near = 0.0;

  float depth = (((far-near) * clip_space_depth) + near + far) / 2.0;
  return depth;
}

float computeLinearDepth(vec3 pos) {
  float near = 0.0;
  float far = 1.0;
  vec4 clip_space_pos = view.perspective * view.view * vec4(pos.xyz, 1.0);
  float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
  float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between 0.01 and 100
  return linearDepth / far; // normalize
}

void main() {
  float t = -nearPoint.y / (farPoint.y - nearPoint.y);
  vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

  gl_FragDepth = computeDepth(fragPos3D);

  float linearDepth = computeLinearDepth(fragPos3D);
  float fading = max(0, (0.5 - linearDepth));

  // outColor = (grid(fragPos3D, 1, true) + grid(fragPos3D, 0.1, true)) * float(t > 0); // adding multiple resolution for the grid
  outColor = grid2(fragPos3D, 10) * float(t > 0); // adding multiple resolution for the grid
  // outColor.a *= fading;
}
