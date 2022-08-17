#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aAxis;
layout(location = 2) in vec2 aSize;

layout(location = 0) out vec2 aTex;

layout(std140, binding=0) uniform Matrices {
  mat4 view;
  mat4 projection;
};

layout(std140, binding=1) uniform CameraInfo {
  vec3 right;
  vec3 up;
};


void main() {
  const vec2 uvData[6] = vec2[6] (
    vec2(0.0, 0.0), vec2(0.0, 1.0),vec2(1.0, 1.0),
    vec2(0.0, 0.0), vec2(1.0, 1.0), vec2(1.0, 0.0)
  );

#ifdef OPENGL
  aTex = uvData[gl_VertexID];
#else
  aTex = uvData[gl_VertexIndex];
#endif

  float x = aTex.x;
  float y = aTex.y;

  vec3 newPos = aPos + ((x - 0.5) * aSize.x) * normalize(vec3(0, 1, 0)) + (y * aSize.y) * normalize(right);
  vec4 posRes = vec4(newPos.x, newPos.y, newPos.z, 1.0);
  gl_Position = projection * view * posRes;
}
