#version 450

layout(location = 0) in vec2 TexCoords;
layout(location = 0) out float shadowInfo;

layout(std140, binding = 0) uniform LightInfo {
  mat4 lightMatrices[5];
  mat4 cameraViewMatrix;
  float cascadePlaneDistances[5];
  float farPlane;
  float lightDir;
};

layout(binding = 0) uniform sampler2D g_position;
layout(binding = 1) uniform sampler2D g_normal;
layout(binding = 2) uniform sampler2DArray g_shadowMap;

// TODO: use macro instead
int cascadeCount = 4;

void main() {
  vec4 position  = texture(g_position, TexCoords);
  vec3 normal = vec3(texture(g_normal, TexCoords));

  vec4 cameratViewPos = cameraViewMatrix * position;
  float depthValue = abs(cameratViewPos.z);

  int layer = -1;
  for(int i = 0; i < cascadeCount; i++) {
    if(depthValue < cascadePlaneDistances[i]) {
      layer = i;
      break;
    }
  }
  if(layer == -1) {
    layer = cascadeCount;
  }

  vec4 lightViewPos = lightSpaceMatrices[layer] * position;
  float currentDepth = lightViewPos.z / lightViewPos.w;

  if(currentDepth > 1.0) shadowInfo = 0; // no shadow if the position is out of far plane.

  float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
  const float biasModifier = 0.5f;
  if (layer == cascadeCount) {
    bias *= 1 / (farPlane * biasModifier);
  }
  else {
    bias *= 1 / (cascadePlaneDistances[layer] * biasModifier);
  }

  // PCF
  float shadow = 0.0;
  vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
  for(int x = -1; x <= 1; ++x) {
    for(int y = -1; y <= 1; ++y) {
      float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
      shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
    }
  }
  shadow /= 9.0;
  shadowInfo = shadow;
}
