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
} dirLightInfo;

layout(std140, binding = 2) uniform PointLightInfo {
  PointLight lights[MAX_POINT_LIGHT_COUNT];
  uint count;
} pointLightsInfo;

layout(binding = 0) uniform sampler2D gColor;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gPosition;
layout(binding = 3) uniform samplerCubeArray gPointLightShadow;
layout(binding = 4) uniform sampler2DArray gDirectionLightShadow;
layout(binding = 5) uniform sampler2D gRoughness;
layout(binding = 6) uniform sampler2D gMetallic;
layout(binding = 7) uniform sampler2D gAO;

const float PI = 3.14159265359;

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

vec3 fresnelSchlick(float cosTheta, vec3 FO) {
  return FO + (1.0 - FO) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
  float a = roughness * roughness;
  float a2 = a * a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH*NdotH;

  float nom = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
  float r = roughness + 1.0;
  float k = (r * r) / 8.0;

  float nom = NdotV;
  float denom = NdotV * (1.0 - k) + k;

  return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2  = GeometrySchlickGGX(NdotV, roughness);
  float ggx1  = GeometrySchlickGGX(NdotL, roughness);

  return ggx1 * ggx2;
}

void main() {
  // Retrieve data from gbuffer
  vec3 worldPos = texture(gPosition, TexCoords).rgb;
  vec3 Normal = texture(gNormal, TexCoords).rgb;
  vec3 viewDir  = normalize(POS - worldPos);
  vec3 Albedo = texture(gColor, TexCoords).rgb;
  float Metallic = texture(gMetallic, TexCoords).r;
  float Roughness = texture(gRoughness, TexCoords).r;
  float AO = texture(gAO, TexCoords).r;

  // Then calculate lighting as usual
  vec3 lighting  = Albedo * 0.1; // hard-coded ambient component

  vec3 F0 = vec3(0.04);
  F0 = mix(F0, Albedo, Metallic);

  vec3 Lo = vec3(0.0);
  for(int i = 0; i < min(pointLightsInfo.count, MAX_POINT_LIGHT_COUNT); i++) {
    vec3 lightPos = pointLightsInfo.lights[i].pos;
    vec3 lightColor = pointLightsInfo.lights[i].color;

    vec3 lightDir = normalize(lightPos - worldPos);
    float lightDistance = length(lightPos - worldPos);
    vec3 H = normalize(viewDir + lightDir);
    float attenuation = 1.0 / (lightDistance * lightDistance);
    vec3 radiance = lightColor * attenuation;

    // cook-torrance brdf
    float NDF = DistributionGGX(Normal, H, Roughness);
    float G = GeometrySmith(Normal, viewDir, lightDir, Roughness);
    vec3 F = fresnelSchlick(max(dot(H, Normal), 0.0), F0);

    vec3 KS = F;
    vec3 KD = vec3(1.0) - KS;
    KD *= 1.0 - Metallic;

    vec3 nominator = NDF * G * F;
    float denominator = 4.0 * max(dot(Normal, viewDir), 0.0) * max(dot(Normal, lightDir), 0.0) + 0.001; 
    vec3 specular = nominator / denominator;

    float shadow = PointLightShadow(worldPos, Normal, i);

    // add to outgoing radiance Lo
    float NdotL = max(dot(Normal, lightDir), 0.0);
    Lo +=  (1 - shadow) *(KD * Albedo / PI + specular) * radiance * NdotL;
  }

  for(int i = 0; i < min(dirLightInfo.count, MAX_DIRECTION_LIGHT_COUNT); i++) {
    vec3 lightColor = dirLightInfo.lights[i].color;
    vec3 lightDir = -normalize(dirLightInfo.lights[i].direction);

    vec3 radiance = lightColor;
    vec3 H = normalize(viewDir + lightDir);

    // cook-torrance brdf
    float NDF = DistributionGGX(Normal, H, Roughness);
    float G = GeometrySmith(Normal, viewDir, lightDir, Roughness);
    vec3 F = fresnelSchlick(max(dot(H, Normal), 0.0), F0);

    vec3 KS = F;
    vec3 KD = vec3(1.0) - KS;
    KD *= 1.0 - Metallic;

    vec3 nominator = NDF * G * F;
    float denominator = 4.0 * max(dot(Normal, viewDir), 0.0) * max(dot(Normal, lightDir), 0.0) + 0.001; 
    vec3 specular = nominator / denominator;

    mat4 viewMatrix = dirLightInfo.lights[i].view;
    mat4 projectionMatrix = dirLightInfo.lights[i].projection;
    float bias = max(0.05 * (1.0 - dot(normalize(Normal), lightDir)), 0.005);
    vec4 cameraSpacePos = projectionMatrix * viewMatrix * vec4(worldPos, 1.0);
    float shadow = DirectionLightShadow(cameraSpacePos, i, bias);

    // add to outgoing radiance Lo
    float NdotL = max(dot(Normal, lightDir), 0.0);
    Lo += (1 - shadow) * (KD * Albedo / PI + specular) * radiance * NdotL;
  }

  vec3 ambient = vec3(0.03) * Albedo * AO;

  vec3 color = ambient + Lo;
  color = color / (color + vec3(1.0));
  color = pow(color, vec3(1.0/2.2));  

  FragColor = vec4(color, 1.0);
}
