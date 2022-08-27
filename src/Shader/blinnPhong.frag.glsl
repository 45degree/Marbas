#version 450

#define MAX_LIGHT_COUNT 20

struct Lights {
  vec3 pos;
  vec3 color;
};

layout(location = 0) in vec2 TexCoords;
layout(location = 0) out vec4 FragColor;

layout(std140, binding = 1) uniform LightsInfo {
  Lights lights[MAX_LIGHT_COUNT];
  uint lightsCount;
  vec3 viewPos;
};

layout(binding = 0) uniform sampler2D gColor;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gPosition;

void main() {
  // Retrieve data from gbuffer
  vec3 FragPos = texture(gPosition, TexCoords).rgb;
  vec3 Normal = texture(gNormal, TexCoords).rgb;
  vec3 Diffuse = texture(gColor, TexCoords).rgb;
  float Specular = texture(gColor, TexCoords).a;

  // Then calculate lighting as usual
  vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
  vec3 viewDir  = normalize(viewPos - FragPos);
  for(int i = 0; i < min(lightsCount, MAX_LIGHT_COUNT); ++i) {
    vec3 lightDir = normalize(lights[i].pos - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].color;
    lighting += diffuse;
  }
  FragColor = vec4(lighting, 1.0);
}
