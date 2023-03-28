#version 450

#extension GL_GOOGLE_include_directive : enable

#include "common/Atmosphere.glsl"
#include "common/Common.glsl"

layout(location = 0) in vec2 inTex;
layout(location = 0) out vec4 multiScatterValue;

layout(std140, binding = 0) uniform AtmosphereInfo {
  float atmosphereHeight;
  float rayleighScalarHeight;
  float mieScalarHeight;
  float mieAnisotropy;
  float planetRadius;
  float ozoneCenterHeight;
  float ozoneWidth;
};
layout(binding = 0) uniform sampler2D transmittanceLUT;

const int N_DIRECTION = 64;
const int N_SAMPLE = 32;
vec3 RandomSphereSamples[64] = {
  vec3(-0.7838,-0.620933,0.00996137),
  vec3(0.106751,0.965982,0.235549),
  vec3(-0.215177,-0.687115,-0.693954),
  vec3(0.318002,0.0640084,-0.945927),
  vec3(0.357396,0.555673,0.750664),
  vec3(0.866397,-0.19756,0.458613),
  vec3(0.130216,0.232736,-0.963783),
  vec3(-0.00174431,0.376657,0.926351),
  vec3(0.663478,0.704806,-0.251089),
  vec3(0.0327851,0.110534,-0.993331),
  vec3(0.0561973,0.0234288,0.998145),
  vec3(0.0905264,-0.169771,0.981317),
  vec3(0.26694,0.95222,-0.148393),
  vec3(-0.812874,-0.559051,-0.163393),
  vec3(-0.323378,-0.25855,-0.910263),
  vec3(-0.1333,0.591356,-0.795317),
  vec3(0.480876,0.408711,0.775702),
  vec3(-0.332263,-0.533895,-0.777533),
  vec3(-0.0392473,-0.704457,-0.708661),
  vec3(0.427015,0.239811,0.871865),
  vec3(-0.416624,-0.563856,0.713085),
  vec3(0.12793,0.334479,-0.933679),
  vec3(-0.0343373,-0.160593,-0.986423),
  vec3(0.580614,0.0692947,0.811225),
  vec3(-0.459187,0.43944,0.772036),
  vec3(0.215474,-0.539436,-0.81399),
  vec3(-0.378969,-0.31988,-0.868366),
  vec3(-0.279978,-0.0109692,0.959944),
  vec3(0.692547,0.690058,0.210234),
  vec3(0.53227,-0.123044,-0.837585),
  vec3(-0.772313,-0.283334,-0.568555),
  vec3(-0.0311218,0.995988,-0.0838977),
  vec3(-0.366931,-0.276531,-0.888196),
  vec3(0.488778,0.367878,-0.791051),
  vec3(-0.885561,-0.453445,0.100842),
  vec3(0.71656,0.443635,0.538265),
  vec3(0.645383,-0.152576,-0.748466),
  vec3(-0.171259,0.91907,0.354939),
  vec3(-0.0031122,0.9457,0.325026),
  vec3(0.731503,0.623089,-0.276881),
  vec3(-0.91466,0.186904,0.358419),
  vec3(0.15595,0.828193,-0.538309),
  vec3(0.175396,0.584732,0.792038),
  vec3(-0.0838381,-0.943461,0.320707),
  vec3(0.305876,0.727604,0.614029),
  vec3(0.754642,-0.197903,-0.62558),
  vec3(0.217255,-0.0177771,-0.975953),
  vec3(0.140412,-0.844826,0.516287),
  vec3(-0.549042,0.574859,-0.606705),
  vec3(0.570057,0.17459,0.802841),
  vec3(-0.0330304,0.775077,0.631003),
  vec3(-0.938091,0.138937,0.317304),
  vec3(0.483197,-0.726405,-0.48873),
  vec3(0.485263,0.52926,0.695991),
  vec3(0.224189,0.742282,-0.631472),
  vec3(-0.322429,0.662214,-0.676396),
  vec3(0.625577,-0.12711,0.769738),
  vec3(-0.714032,-0.584461,-0.385439),
  vec3(-0.0652053,-0.892579,-0.446151),
  vec3(0.408421,-0.912487,0.0236566),
  vec3(0.0900381,0.319983,0.943135),
  vec3(-0.708553,0.483646,0.513847),
  vec3(0.803855,-0.0902273,0.587942),
  vec3(-0.0555802,-0.374602,-0.925519),
};

vec3
IntegralMultiScattering(const in AtmosphereParam param, vec3 point, vec3 lightDir) {
  const float uniformPhase = 1.0 / (4.0 * PI);
  const float sphereSolidAngle = 4.0 * PI / float(N_DIRECTION);

  const float atmoRadius = param.planetRadius + param.atmosphereHeight;

  vec3 G_2 = vec3(0, 0, 0);
  vec3 f_ms = vec3(0, 0, 0);
  for(int i = 0; i < N_DIRECTION; i++) {
    vec3 dir = RandomSphereSamples[i];
    vec3 intersectAtmo = RayIntersectSphere(vec3(0, 0, 0), atmoRadius, point, dir);
    if(intersectAtmo[0] < 0) {
      continue;
    }

    float distRay = intersectAtmo[2];
    vec3 intersectPlane = RayIntersectSphere(vec3(0, 0, 0), param.planetRadius, point, dir);
    if(intersectPlane[0] > 0) {
      distRay = intersectPlane[1];
    }

    float ds = distRay / N_SAMPLE;
    vec3 p1 = point + dir * ds * 0.5;
    vec3 opticalDepth = vec3(0, 0, 0);
    for(int j = 0; j < N_SAMPLE; j++) {
      float h = clamp(length(p1) - planetRadius, 0, atmosphereHeight);
      vec3 scatter = RayleighCoefficient(rayleighScalarHeight, h) + MieCoefficient(mieScalarHeight, h);
      vec3 absorption = MieAbsorption(mieScalarHeight, h) + OzoneAbsorption(h, param);
      opticalDepth += (scatter + absorption) * ds;

      // calculate t1
      float mu = dot(normalize(-lightDir), normalize(p1));
      vec2 uv = TransmittanceParamConvertToUV(param, mu, length(p1));
      vec3 t1 = vec3(texture(transmittanceLUT, uv));

      vec3 s = Scattering(param, p1, lightDir, -dir);
      vec3 t2 = exp(-opticalDepth);

      // 用 1.0 代替太阳光颜色, 该变量在后续的计算中乘上去
      G_2 += t1 * s * t2 * uniformPhase * ds * 1.0;  
      f_ms += t2 * scatter * uniformPhase * ds;

      p1 += dir * ds;
    }
  }

  G_2 *= sphereSolidAngle;
  f_ms *= sphereSolidAngle;
  return G_2 * (1.0 / (1.0 - f_ms));
}

void main() {
  AtmosphereParam atmosphereParam;
  atmosphereParam.atmosphereHeight = atmosphereHeight;
  atmosphereParam.rayleighScalarHeight = rayleighScalarHeight;
  atmosphereParam.mieScalarHeight = mieScalarHeight;
  atmosphereParam.mieAnisotropy = mieAnisotropy;
  atmosphereParam.planetRadius = planetRadius;
  atmosphereParam.ozoneCenterHeight = ozoneCenterHeight;
  atmosphereParam.ozoneWidth = ozoneWidth;

  vec2 param = MultiScatterParamConvertFromUV(atmosphereParam, inTex);
  float cos_theta = param.x;
  float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
  vec3 lightDir = vec3(sin_theta, cos_theta, 0);
  vec3 p = vec3(0, param.y, 0);

  multiScatterValue = vec4(IntegralMultiScattering(atmosphereParam, p, lightDir), 1.0);
}
