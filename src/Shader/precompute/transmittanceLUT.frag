#version 450

#extension GL_GOOGLE_include_directive : enable

#include "common/Atmosphere.glsl"
#include "common/Common.glsl"

layout(location = 0) in vec2 inTex;
layout(location = 0) out vec4 outValue;

layout(std140, binding = 0) uniform AtmosphereInfo {
  float atmosphereHeight;
  float rayleighScalarHeight;
  float mieScalarHeight;
  float mieAnisotropy;
  float planetRadius;
  float ozoneCenterHeight;
  float ozoneWidth;
};

void main() {
  AtmosphereParam atmosphereParam;
  atmosphereParam.atmosphereHeight = atmosphereHeight;
  atmosphereParam.rayleighScalarHeight = rayleighScalarHeight;
  atmosphereParam.mieScalarHeight = mieScalarHeight;
  atmosphereParam.mieAnisotropy = mieAnisotropy;
  atmosphereParam.planetRadius = planetRadius;
  atmosphereParam.ozoneCenterHeight = ozoneCenterHeight;
  atmosphereParam.ozoneWidth = ozoneWidth;

  vec2 param = TransmittanceParamConvertFromUV(atmosphereParam, inTex);
  float cos_theta = param.x;
  float h = param.y;

  vec3 dir = vec3(sqrt(1 - cos_theta * cos_theta), cos_theta, 0);
  float atmosphereRadius = atmosphereParam.planetRadius + atmosphereParam.atmosphereHeight;
  vec3 intersectP2 = RayIntersectSphere(vec3(0, 0, 0), atmosphereRadius, vec3(0, h, 0), dir);
  if(intersectP2[0] <= 0) {
    outValue = vec4(0, 0, 0, 1);
    return;
  }
  float distP2 = intersectP2[2];

  vec3 p2 =vec3(0, h, 0) + dir * distP2;
  vec3 t = Transmittance(atmosphereParam, vec3(0, h, 0), p2);
  outValue = vec4(t, 1.0);
}
