#version 450

layout(location = 0) in vec2 inTex;
layout(location = 0) out vec4 outColor;

layout(binding = 0, set = 0) uniform sampler3D voxel;
layout(binding = 1, set = 0) uniform sampler2D gWorldPos;
layout(binding = 2, set = 0) uniform sampler2D gNormal;
layout(binding = 3, set = 0) uniform sampler2D gDiffuse;

layout(std140, binding = 0, set = 1) uniform VoxelInfo {
  vec4 voxelResolutionSize; // the length of xyz and the size of voxel
  vec3 voxelCenter;
};

layout(binding = 1, set = 1) uniform CameraInfo {
  vec3 cameraPos;
};

float coneWeights[6] = float[](0.25, 0.15, 0.15, 0.15, 0.15, 0.15);//各锥体权重

vec3 coneDirections[6] = vec3[] ( 
  vec3(0, 0, 1),
  vec3(0, 0.866025,0.5),
  vec3(0.823639, 0.267617, 0.5),
  vec3(0.509037, -0.700629, 0.5),
  vec3(-0.509037, -0.700629, 0.5),
  vec3(-0.823639, 0.267617, 0.5)
);

vec4
SampleVoxels(vec3 worldPos, float lod) {
	vec3 clipPos = (worldPos - voxelCenter) / (voxelResolutionSize.xyz * voxelResolutionSize.w);
  // clipPos.y = -clipPos.y;
	vec3 ndc = clipPos + 0.5;
	return textureLod(voxel, ndc, lod);
}

vec4
ConeTracing(vec3 pos, vec3 direction, vec3 normal, float tanHalfAngle, float maxDist, float alphaThresh){
  float voxelWorldSize = voxelResolutionSize.w;
	vec3 start = pos + normal * voxelWorldSize;

	float d = voxelWorldSize;

  vec3 color = vec3(0);
	float alpha = 0.0;
	float occlusion = 0.0;

  while(d < maxDist && alpha< alphaThresh){
     float diameter = max(voxelWorldSize, 2.0 * tanHalfAngle * d);
     float lodLevel = log2(diameter / voxelWorldSize);

     vec4 voxelColor = SampleVoxels(start + direction * d, lodLevel);
     occlusion += ((1.0 - alpha) * voxelColor.a) / (1.0 + 0.03 * diameter);
     color += (1.0 - alpha) * voxelColor.rgb;
     alpha += (1.0 - alpha) * voxelColor.a;
     d += diameter;
  }
  return vec4(color, occlusion);
}

void main() {
  vec3 worldPos = texture(gWorldPos, inTex).xyz;
  vec3 normal = texture(gNormal, inTex).xyz;
  vec4 diffuse = texture(gDiffuse, inTex);

  vec3 wo = normalize(cameraPos - worldPos);
  vec3 N = normalize(normal);
  vec3 reflectDir = normalize(reflect(-wo,N));

  /**
   * use gram schmidt orthogonalization to generate a random TBN
   */

  // guide cannot be collinear with normal
	vec3 guide = vec3(0.0f, 1.0f, 0.0f);
	if (abs(dot(N, guide)) > 0.9f)
	  guide = vec3(0.0f, 0.0f, 1.0f);

	vec3 T = normalize(guide - dot(N, guide) * N);
	vec3 B = cross(T, N);
  mat3 TBN = mat3(T,B,N);

  // //叠加六个锥体
  vec4 diffuseReflection = vec4(0, 0, 0, 0);

  // diffuseReflection = coneWeights[0] * ConeTracing(worldPos, normalize(TBN * coneDirections[0]) , N, 0.577, 1000, 0.95);
  for(int i = 0; i < 6; i++) {
    diffuseReflection += coneWeights[i] * ConeTracing(worldPos, normalize(TBN * coneDirections[i]) , N, 0.577, 1000, 0.95);
  }
  outColor.xyz = diffuse.xyz * diffuseReflection.xyz;

  // convert to gamma space
  const float gamma = 2.2;
  outColor.rgb = pow(outColor.rgb, vec3(1.0 / gamma));
}
