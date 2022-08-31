#version 450

layout(location = 0) in vec2 aTex;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D gColor;
layout(binding = 1) uniform sampler2D lightDepthTexture;
layout(binding = 2) uniform sampler2D gPosition;
layout(binding = 3) uniform sampler2D gNormal;

layout(std140, binding = 0) uniform CameraMatrix {
  mat4 VIEW;
  mat4 PROJETION;
  vec3 RIGHT;
  vec3 UP;
  vec3 POS;
};

layout(std140, binding = 1) uniform LightMatrix {
  mat4 view;
  mat4 projection;
};

layout(std140, binding = 2) uniform LightInfo {
  vec3 lightPos;
};

float ShadowCalculation(vec4 fragPosLightSpace, float bias) {
  // perform perspective divide
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

  if(projCoords.x < -1 || projCoords.x > 1) return 0.0;
  if(projCoords.y < -1 || projCoords.y > 1) return 0.0;
  if(projCoords.z < -1 || projCoords.z > 1) return 0.0;

  // transform to [0,1] range
  projCoords = projCoords * 0.5 + 0.5;

  // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
  float closestDepth = texture(lightDepthTexture, projCoords.xy).r; 

  // get depth of current fragment from light's perspective
  float currentDepth = projCoords.z;

  float shadow = 0.0;
  vec2 texelSize = 1.0 / textureSize(lightDepthTexture, 0);
  for(int x = -1; x <= 1; ++x) {
    for(int y = -1; y <= 1; ++y) {
      float pcfDepth = texture(lightDepthTexture, projCoords.xy + vec2(x, y) * texelSize).r; 
      shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
  }
  shadow /= 9.0;

  return shadow;
}

void main() {
  vec3 position = texture(gPosition, aTex).rgb;
  vec3 normal = texture(gNormal, aTex).rgb;
  vec4 ligthViewPosition = projection * view * vec4(position, 1.0);

  vec3 lightDir = normalize(lightPos - position);
  float bias = max(0.05 * (1.0 - dot(normalize(normal), lightDir)), 0.005);

  float shadow = ShadowCalculation(ligthViewPosition, bias);
  outColor = (1 - shadow) * texture(gColor, aTex);
}
