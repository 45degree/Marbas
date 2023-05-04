#ifndef SHADOW_GLSL
#define SHADOW_GLSL

#include "Common.glsl"

/**
 * @brief calculate the shadow value
 *
 * @param fragPos the fragment position from the shadow camera, calculate by MVP_Shadow * worldPosition
 */
float DirectionLightShadow(vec4 fragPos, int layer, sampler2DArray shadowAtlas, vec4 viewport, float bias) {
  vec3 projCoords = fragPos.xyz / fragPos.w;

  if(projCoords.x < -1 || projCoords.x > 1) return 0.0;
  if(projCoords.y < -1 || projCoords.y > 1) return 0.0;
  if(projCoords.z < 0 || projCoords.z > 1) return 0.0;

  // transform to the viewport range
  vec2 uv = projCoords.xy;
  uv = 0.5 * viewport.zw * (uv + 1) + viewport.xy;

  float depth = texture(shadowAtlas, vec3(uv, layer)).r;
  float currentDepth = projCoords.z;

  float shadow = 0.0;
  vec2 texelSize = 1.0 / (viewport.zw * textureSize(shadowAtlas, 0).x);
  for(int x = -1; x <= 1; ++x) {
    for(int y = -1; y <= 1; ++y) {
      vec2 samplePos = uv + vec2(x, y) * texelSize;
      float pcfDepth = texture(shadowAtlas, vec3(samplePos, layer)).r; 
      shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
  }
  shadow /= 9.0;

  return shadow;
}

float DirectionLightShadow(vec3 worldPos, mat4 cameraView, sampler2DArray shadowAtlas,
                           const in DirectionLightInfo lightInfo) {
  vec4 cameraCoordPos = cameraView * vec4(worldPos, 1.0);
  float depthValue = abs(cameraCoordPos.z);

  int layer = -1;
  for (int i = 0; i < CASCADE_COUNT; ++i) {
    if (depthValue < lightInfo.cascadePlaneDistances[i].x) {
        layer = i;
        break;
    }
  }

  if (layer == -1) {
    layer = CASCADE_COUNT;
  }
  vec4 fragPosLightSpace = lightInfo.lightMatrix[layer] * vec4(worldPos, 1.0);

  float bias = 0.005;
  bias *= 1 / (lightInfo.cascadePlaneDistances[layer].x * 0.5f);
  return DirectionLightShadow(fragPosLightSpace, layer, shadowAtlas, lightInfo.atlasViewport, bias);
}

#endif
