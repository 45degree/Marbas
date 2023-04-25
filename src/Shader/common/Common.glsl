#ifndef MARBAS_COMMON_COMMON_GLSL
#define MARBAS_COMMON_COMMON_GLSL

#include "ConstValue.glsl"


/**
 * the common struct
 */

// TODO: add this to compile option
#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

struct DirectionLightInfo {
  vec2 direction;
  ivec3 color;
  float energy;
  vec4 atlasViewport;
  vec4 cascadePlaneDistances[CASCADE_COUNT + 1];
  mat4 lightMatrix[CASCADE_COUNT + 1];
};


/**
 * the common functions
 */

/**
 * @see https://blog.csdn.net/silangquan/article/details/17464821
 */
vec3
RayIntersectSphere(const vec3 spherePoint, const float sphereRadius, const vec3 rayPoint, const vec3 rayDir) {
  vec3 l = spherePoint - rayPoint;
  float s = dot(l, rayDir);
  float l_2 = dot(l, l);
  float r_2 = sphereRadius * sphereRadius;
  if(s < 0 &&  l_2 > r_2) return vec3(-1, 0, 0);

  float m_2 = l_2 - s * s;
  if(m_2 > r_2) return vec3(-1, 0, 0);

  float q = sqrt(r_2 - m_2);
  return vec3(1, s - q, s + q);
}

/**
 * @brief convert a direction to a Equirectangular UV
 * @see https://learnopengl.com/PBR/IBL/Diffuse-irradiance
 */
vec2
DirToCubemapUV2D(vec3 dir) {
  const vec2 invAtan = vec2(1 / (2 * PI), 1 / PI);
  dir = normalize(dir);

  vec2 uv = vec2(atan(dir.z, dir.x), asin(dir.y));
  uv *= invAtan;
  uv += 0.5;

  return uv;
}

/**
 * @brief convert a Equirectangular UV to direction, the inverse operator of DirToCubemapUV2D.
 * @see DirToCubemapUV2D
 */
vec3
CubemapUV2DToDir(vec2 uv) {
  float theta = (1.0 - uv.y) * PI;
  float phi = (uv.x * 2 - 1) * PI;

  float x = sin(theta) * cos(phi);
  float z = sin(theta) * sin(phi);
  float y = cos(theta);

  return vec3(x, y, z);
}

#endif
