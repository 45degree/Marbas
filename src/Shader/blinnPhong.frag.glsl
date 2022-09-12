#version 450

#define MAX_DIRECTION_LIGHT_COUNT 3
#define MAX_POINT_LIGHT_COUNT 3

struct PointLight {
  vec3 pos;
  vec3 color;
  float farPlane;
};

struct DirectionLight {
  vec3 pos;
  vec3 color;
  vec3 direction;
  mat4 view;
  mat4 projection;
};

layout(location = 0) in vec2 TexCoords;
layout(location = 0) out vec4 FragColor;

layout(std140, binding = 0) uniform Matrices {
  mat4 VIEW;
  mat4 PROJETION;
  vec3 RIGHT;
  vec3 UP;
  vec3 POS;
};

layout(std140, binding = 1) uniform DirectionLightInfo {
  DirectionLight lights[MAX_DIRECTION_LIGHT_COUNT];
  uint count;
  vec3 viewPos;
} dirLightInfo;

layout(std140, binding = 2) uniform PointLightInfo {
  PointLight lights[MAX_POINT_LIGHT_COUNT];
  uint count;
  vec3 viewPos;
} pointLightsInfo;

layout(binding = 0) uniform sampler2D gColor;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gPosition;
layout(binding = 3) uniform samplerCubeArray gPointLightShadow;
layout(binding = 4) uniform sampler2DArray gDirectionLightShadow;

float PointLightShadow(vec3 fragPos, vec3 normal, int lightIndex){
  vec3 lightPos = pointLightsInfo.lights[lightIndex].pos;
  vec3 lightDir = normalize(fragPos - lightPos);
  float distance = length(fragPos - lightPos);
  float bias = max(0.05 * (1.0 - dot(normalize(normal), -lightDir)), 0.005);

  float depth = texture(gPointLightShadow, vec4(lightDir, lightIndex)).r;
  depth *= pointLightsInfo.lights[lightIndex].farPlane;

  return distance - bias > depth ? 1.0 : 0.0;
}

float DirectionLightShadow(vec4 fragPos, int lightIndex, float bias) {

  vec3 projCoords = fragPos.xyz / fragPos.w;

  if(projCoords.x < -1 || projCoords.x > 1) return 0.0;
  if(projCoords.y < -1 || projCoords.y > 1) return 0.0;
  if(projCoords.z < -1 || projCoords.z > 1) return 0.0;

  // transform to [0,1] range
  projCoords = projCoords * 0.5 + 0.5;

  float depth = texture(gDirectionLightShadow, vec3(projCoords.xy, lightIndex)).r;
  float currentDepth = projCoords.z;

  float shadow = 0.0;
  vec2 texelSize = 1.0 / textureSize(gDirectionLightShadow, 0).xy;
  for(int x = -1; x <= 1; ++x) {
    for(int y = -1; y <= 1; ++y) {
      vec2 samplePos = projCoords.xy + vec2(x, y) * texelSize;
      float pcfDepth = texture(gDirectionLightShadow, vec3(samplePos, lightIndex)).r; 
      shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
  }
  shadow /= 9.0;

  return shadow;
}

void main() {
  // Retrieve data from gbuffer
  vec3 FragPos = texture(gPosition, TexCoords).rgb;
  vec3 Normal = texture(gNormal, TexCoords).rgb;
  vec3 Diffuse = texture(gColor, TexCoords).rgb;
  float Specular = texture(gColor, TexCoords).a;

  // Then calculate lighting as usual
  vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component

  for(int i = 0; i < min(pointLightsInfo.count, MAX_POINT_LIGHT_COUNT); i++) {
    vec3 viewDir  = normalize(pointLightsInfo.viewPos - FragPos);
    vec3 lightPos = pointLightsInfo.lights[i].pos;
    vec3 lightColor = pointLightsInfo.lights[i].color;
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lightColor;

    float distance = length(lightPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32.0);
    vec3 specular = lightColor * spec * Specular;

    float shadow = PointLightShadow(FragPos, Normal, i);

    lighting += (1 - shadow) * (diffuse + specular);
  }

  for(int i = 0; i < min(dirLightInfo.count, MAX_DIRECTION_LIGHT_COUNT); i++) {
    vec3 viewDir  = normalize(dirLightInfo.viewPos - FragPos);
    vec3 lightColor = dirLightInfo.lights[i].color;
    vec3 lightDir = -normalize(dirLightInfo.lights[i].direction);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lightColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32.0);
    vec3 specular = lightColor * spec * Specular;

    mat4 viewMatrix = dirLightInfo.lights[i].view;
    mat4 projectionMatrix = dirLightInfo.lights[i].projection;

    float bias = max(0.05 * (1.0 - dot(normalize(Normal), lightDir)), 0.005);
    vec4 cameraSpacePos = projectionMatrix * viewMatrix * vec4(FragPos, 1.0);

    float shadow = DirectionLightShadow(cameraSpacePos, i, bias);
    lighting += (1 - shadow) * (diffuse + specular);
  }
  FragColor = vec4(lighting, 1.0);
}
