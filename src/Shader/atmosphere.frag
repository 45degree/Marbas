#version 450

#extension GL_GOOGLE_include_directive : enable

#include "common/Atmosphere.glsl"
#include "common/Common.glsl"

layout(location = 0) in vec2 inTex;
layout(location = 0) out vec4 outColor;

layout(std140, binding = 0) uniform Matrices {
  mat4 view;
  mat4 projection;
};

layout(std140, binding = 1) uniform AtmosphereInfo {
  vec3 lightDir; // the start point is the sun, and it has beed normalized
  vec3 sunLuminace;

  // Atmosphere Parap
  float atmosphereHeight;
  float rayleighScalarHeight;
  float mieScalarHeight;
  float mieAnisotropy;
  float planetRadius;
  float ozoneCenterHeight;
  float ozoneWidth;
};

layout(binding = 0) uniform sampler2D transmittanceLUT;
layout(binding = 1) uniform sampler2D multiscatterLUT;

void main() {
  vec3 cameraPos = vec3(inverse(view)[3]);
  vec3 viewDir = CubemapUV2DToDir(inTex);
  vec3 eyePos = vec3(0, cameraPos.y + planetRadius, 0) + 100; // add the bias so that can see the sky when y < 0;

  const int N_SAMPLE = 64;
  AtmosphereParam atmosphereParam;
  atmosphereParam.atmosphereHeight = atmosphereHeight;
  atmosphereParam.rayleighScalarHeight = rayleighScalarHeight;
  atmosphereParam.mieScalarHeight = mieScalarHeight;
  atmosphereParam.mieAnisotropy = mieAnisotropy;
  atmosphereParam.planetRadius = planetRadius;
  atmosphereParam.ozoneCenterHeight = ozoneCenterHeight;
  atmosphereParam.ozoneWidth = ozoneWidth;


  vec3 intersectAtmo = RayIntersectSphere(vec3(0, 0, 0), atmosphereHeight + planetRadius, eyePos, viewDir);
  if(intersectAtmo[0] < 0) {
    outColor = vec4(1, 0, 0, 1);
    return;
  }
  float disCamToAtmo = intersectAtmo[2];

  // if intersect with the earth, return 0
  vec3 intersectEarch = RayIntersectSphere(vec3(0, 0, 0), planetRadius, eyePos, viewDir);
  if(intersectEarch[0] >= 0) {
    outColor = vec4(0, 0, 0, 1);
    return;
  }

  float ds = disCamToAtmo / float(N_SAMPLE);

  vec3 p1 = eyePos + viewDir * ds * 0.5;
  vec3 color = vec3(0, 0, 0);
  vec3 TSum = vec3(0, 0, 0);
  for(int i = 0; i < N_SAMPLE - 1; i++) {
    float h = clamp(length(p1) - planetRadius, 0, atmosphereHeight);
    vec3 scatter = RayleighCoefficient(rayleighScalarHeight, h) + MieCoefficient(mieScalarHeight, h);
    vec3 absorption = MieAbsorption(mieScalarHeight, h) + OzoneAbsorption(h, atmosphereParam);
    TSum += (scatter + absorption) * ds;

    // calculate t1
    float mu = dot(normalize(-lightDir), normalize(p1));
    vec2 uv = TransmittanceParamConvertToUV(atmosphereParam, mu, length(p1));
    vec3 t1 = vec3(texture(transmittanceLUT, uv));

    vec3 s = Scattering(atmosphereParam, p1, lightDir, -viewDir);
    vec3 t2 = exp(-TSum);

    vec3 inScattering = t1 * s * t2 * ds * sunLuminace;
    color += inScattering;

    // multi scatter
    vec3 multiScattering = GetMultiScattering(atmosphereParam, p1, lightDir, multiscatterLUT);
    color += multiScattering * t2 * ds * sunLuminace;

    p1 += viewDir * ds;
  }

  color = color / (color + vec3(1.0));
  color = pow(color, vec3(1.0/2.2));

  outColor = vec4(color, 1.0);
}
