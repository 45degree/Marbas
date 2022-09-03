#version 450

layout(location = 0) in vec2 aTex;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D gColor;
layout(binding = 1) uniform samplerCube lightDepthTexture;
layout(binding = 2) uniform sampler2D gPosition;
layout(binding = 3) uniform sampler2D gNormal;

layout(std140, binding = 0) uniform CameraMatrix {
  mat4 VIEW;
  mat4 PROJETION;
  vec3 RIGHT;
  vec3 UP;
  vec3 POS;
};

layout(std140, binding = 1) uniform LightInfo {
  vec3 lightPos;
  float farPlane;
  mat4 matrices[6];
  mat4 projectMatrix;
};

float ShadowCalculation(vec3 fragPos, float bias) {
    // Get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    // Use the light to fragment vector to sample from the depth map
    float closestDepth = texture(lightDepthTexture, fragToLight).r;
    // It is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= farPlane;
    // Now get current linear depth as the length between the fragment and light position
    // Now test for shadows
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    return shadow;

    // float shadow = 0.0;
    // float samples = 4.0;
    // float offset = 0.1;
    // for(float x = -offset; x < offset; x += offset / (samples * 0.5)) {
    //     for(float y = -offset; y < offset; y += offset / (samples * 0.5)) {
    //         for(float z = -offset; z < offset; z += offset / (samples * 0.5)) {
    //             float closestDepth = texture(lightDepthTexture, fragToLight + vec3(x, y, z)).r; 
    //             closestDepth *= farPlane;
    //             if(currentDepth - bias > closestDepth)
    //                 shadow += 1.0;
    //         }
    //     }
    // }
    // shadow /= (samples * samples * samples);
    // return shadow;
}

void main() {
  vec3 position = texture(gPosition, aTex).rgb;

  vec3 normal = texture(gNormal, aTex).rgb;
  vec3 lightDir = normalize(lightPos - position);
  float bias = max(0.05 * (1.0 - dot(normalize(normal), lightDir)), 0.005);

  float shadow = ShadowCalculation(position, bias);
  outColor = (1 - shadow) * texture(gColor, aTex);
}
