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
  float FAR;
  float NEAR;
} view;

vec4 grid(vec3 fragPos3D, float gridSize) {
  vec2 coord = fragPos3D.xz * gridSize;

  vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
  float line = min(grid.x, grid.y);
  float lineResult = 1.4 - min(line, 1.4);
  vec4 color = vec4(0.8, 0.8, 0.8, 0.1 * lineResult);

  // z axis
  if(lineResult > 0) {
    if(fragPos3D.x > -1 && fragPos3D.x < 1 && line < grid.y)
        color = vec4(0, 0, 1, lineResult);
    // x axis
    if(fragPos3D.z > -1 && fragPos3D.z < 1 && line < grid.x)
        color = vec4(1, 0, 0, lineResult);
  }

  return color;
}

float computeDepth(vec3 pos) {
  vec4 clip_space_pos = view.perspective * view.view * vec4(pos.xyz, 1.0);
  float clip_space_depth = clip_space_pos.z / clip_space_pos.w;
  float far = 1;
  float near = 0;

  float depth = (((far-near) * clip_space_depth) + near + far) / 2.0;
  return depth;
}

float computeLinearDepth(vec3 pos) {
  float near = view.NEAR;
  float far = view.FAR;
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

  outColor = grid(fragPos3D, 0.1) * float(t > 0); // adding multiple resolution for the grid
  outColor.a *= fading;
}
