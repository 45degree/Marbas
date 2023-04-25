#ifndef ATMOSPHERE_GLSL
#define ATMOSPHERE_GLSL

#include "ConstValue.glsl"

struct AtmosphereParam {
  float atmosphereHeight;
  float rayleighScalarHeight;
  float mieScalarHeight;
  float mieAnisotropy;
  float planetRadius;
  float ozoneCenterHeight;
  float ozoneWidth;
};

vec3
RayleighCoefficient(float scalarHeight, float h) {
  const vec3 sigma = vec3(5.802, 13.558, 33.1) * 1e-6;
  float rhoH = exp(-h / scalarHeight);
  return rhoH * sigma;
}

float
RayleiPhase(float cosTheta) {
  return 3.0 / (16 * PI) * (1 + cosTheta * cosTheta);
}

vec3
MieCoefficient(float scalarHeight, float h) {
  const vec3 sigma = vec3(3.996, 3.996, 3.996) * 1e-6;
  float rhoH = exp(-h / scalarHeight);
  return rhoH * sigma;
}

float
MiePhase(float anisotropy, float cosTheta) {
  float a = 3 / (8 * PI);
  float b = (1 - anisotropy * anisotropy) / (2 + anisotropy * anisotropy);
  float c = 1 + cosTheta * cosTheta;
  float d = pow(1 + anisotropy * anisotropy - 2 * anisotropy * cosTheta , 1.5);
  return a * b * c / d;
}

vec3
MieAbsorption(float scalarHeight, float h) {
  const vec3 sigma = vec3(4.40, 4.40, 4.40) * 1e-6;
  float rhoH = exp(-h / scalarHeight);
  return rhoH * sigma;
}

vec3
OzoneAbsorption(float h, const in AtmosphereParam param) {
  const vec3 sigma = vec3(0.650, 1.881, 0.085) * 1e-6;
  float rhoH = max(0, 1 - abs(h - param.ozoneCenterHeight) / param.ozoneWidth);
  return rhoH * sigma;
}

vec3
Scattering(const in AtmosphereParam param, vec3 scatterPoint, vec3 lightInDir, vec3 lightOutDir) {
  float cosTheta = dot(lightInDir, lightOutDir);
  float h = clamp(length(scatterPoint) - param.planetRadius, 0, param.atmosphereHeight);

  vec3 rayleigh = RayleiPhase(cosTheta) * RayleighCoefficient(param.rayleighScalarHeight, h);
  vec3 mei = MiePhase(param.mieAnisotropy, cosTheta) * MieCoefficient(param.mieScalarHeight, h);

  return rayleigh + mei;
}

vec3
Transmittance(const in AtmosphereParam param, const in vec3 p1, const in vec3 p2) {
  const int N_SAMPLE = 64;

  if(p2 == p1) return vec3(0, 0, 0);

  vec3 dir = normalize(p2 - p1);
  float distance = length(p2 - p1);
  float ds = distance / float(N_SAMPLE);
  vec3 transimittanceSum = vec3(0, 0, 0);

  vec3 p = p1 + dir * ds * 0.5;
  for(int i = 0; i < N_SAMPLE; i++) {
    float h = clamp(length(p) - param.planetRadius, 0, param.atmosphereHeight);
    vec3 scatter = RayleighCoefficient(param.rayleighScalarHeight, h) + MieCoefficient(param.mieScalarHeight, h);
    vec3 absorption = MieAbsorption(param.mieScalarHeight, h) + OzoneAbsorption(h, param);

    transimittanceSum += (scatter + absorption) * ds;
    p += dir * ds;
  }

  return exp(-transimittanceSum);
}

/**
 * get mu and r from uv
 */
vec2
TransmittanceParamConvertFromUV(const in AtmosphereParam param, vec2 uv) {
  float topRadius = param.planetRadius + param.atmosphereHeight;
  float bottomRadius = param.planetRadius;

  float H = sqrt(topRadius * topRadius - bottomRadius * bottomRadius);
  float rho = H * uv.y;
  float r = sqrt(rho * rho + bottomRadius * bottomRadius);

  float dmin = topRadius - r;
  float dmax = H + rho;
  float d = (dmax - dmin) * uv.x + dmin;
  float mu = d == 0.0f ? 1.0f : (H * H - d * d) / (2 * r * d);
  mu = clamp(mu, -1.0f, 1.0f);

  return vec2(mu, r);
}


/**
 * @brief convert mu and rho to uv
 * @param mu the cos of Zenith Angle
 * @param r the distance from the center of the earth
 */
vec2
TransmittanceParamConvertToUV(const in AtmosphereParam param, float mu, float r) {
  float topRadius = param.planetRadius + param.atmosphereHeight;
  float bottomRadius = param.planetRadius;

  float H = sqrt(topRadius * topRadius - bottomRadius * bottomRadius);
  float rho = sqrt(r * r - bottomRadius * bottomRadius);

  // calculate d
  float _tmp = r * r * (mu * mu - 1) + topRadius * topRadius;
  float d = max(0, -r * mu + sqrt(_tmp));
  float dmin = topRadius - r;
  float dmax = H + rho;

  return vec2((d-dmin) / (dmax - dmin), rho / H);
}

vec2
MultiScatterParamConvertToUV(const in AtmosphereParam param, float mu, float r) {
  float u = mu * 0.5 + 0.5;
  float v = (r - param.planetRadius) / param.atmosphereHeight;

  return vec2(u, v);
}

vec2
MultiScatterParamConvertFromUV(const in AtmosphereParam param, vec2 uv) {
  float mu = uv.x * 2.0 - 1;
  float r = uv.y * param.atmosphereHeight + param.planetRadius;

  return vec2(mu, r);
}

vec3
GetMultiScattering(const in AtmosphereParam param, const vec3 point, const vec3 lightDir, const sampler2D lut) {
  float h = clamp(length(point) - param.planetRadius, 0, param.atmosphereHeight);
  vec3 scatter = RayleighCoefficient(param.rayleighScalarHeight, h) + MieCoefficient(param.mieScalarHeight, h);

  float r = length(point);
  float mu = dot(normalize(point), normalize(-lightDir));
  vec2 uv = MultiScatterParamConvertToUV(param, mu, r);
  vec3 G_ALL = texture(lut, uv).xyz;

  return G_ALL * scatter;
}

#endif
