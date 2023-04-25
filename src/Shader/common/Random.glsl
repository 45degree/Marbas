#ifndef MARBAS_COMMON_RANDOM_GLSL
#define MARBAS_COMMON_RANDOM_GLSL

float
Random1DTo1D(float value,float a,float b){
  float random = fract(sin(value+b)*a);
  return random;
}

float
Random2DTo1D(vec2 value, float a, vec2 b) {
  //avaoid artifacts
  vec2 smallValue = sin(value);
  //get scalar value from 2d vector	
  float  random = dot(smallValue,b);
  random = fract(sin(random) * a);
  return random;
}

vec2
Random2DTo2D(vec2 value){
  return vec2(
    Random2DTo1D(value, 14375.5964, vec2(15.637, 76.243)),
    Random2DTo1D(value, 14684.6034, vec2(45.366, 23.168))
  );
}

float
Random3DTo1D(vec3 value, float a, vec3 b) {
  vec3 smallValue = sin(value);
  float random = dot(smallValue, b);
  random = fract(sin(random) * a);
  return random;
}

vec2
Random3DTo2D(vec3 value){
  return vec2(
    Random3DTo1D(value, 14375.5964, vec3(15.637, 76.243, 37.168)),
    Random3DTo1D(value, 14684.6034, vec3(45.366, 23.168, 65.918))
  );
}

vec3
Random3DTo3D(vec3 value){
  return vec3(
    Random3DTo1D(value, 14375.5964, vec3(15.637, 76.243, 37.168)),
    Random3DTo1D(value, 14684.6034, vec3(45.366, 23.168, 65.918)),
    Random3DTo1D(value, 17635.1739, vec3(62.654, 88.467, 25.111))
  );
}

float
Random4DTo1D(vec4 value, float a, vec4 b) {
  vec4 smallValue = sin(value);
  float  random = dot(smallValue,b);
  random = fract(sin(random) * a);
  return random;
}

vec2 Random4DTo2D(vec4 value) {
  return vec2(
    Random4DTo1D(value, 14375.5964, vec4(15.637, 76.243, 37.168, 83.511)),
    Random4DTo1D(value, 14684.6034, vec4(45.366, 23.168, 65.918, 57.514))
	);
}

vec3 Random4DTo3D(vec4 value) {
  return vec3(
    Random4DTo1D(value, 14375.5964, vec4(15.637, 76.243, 37.168, 83.511)),
    Random4DTo1D(value, 14684.6034, vec4(45.366, 23.168, 65.918, 57.514)),
    Random4DTo1D(value, 14985.1739, vec4(62.654, 88.467, 25.111, 61.875))
  );
}

vec4 Random4DTo4D(vec4 value) {
  return vec4(
    Random4DTo1D(value,14375.5964, vec4(15.637, 76.243, 37.168, 83.511)),
    Random4DTo1D(value,14684.6034, vec4(45.366, 23.168, 65.918, 57.514)),
    Random4DTo1D(value,14985.1739, vec4(62.654, 88.467, 25.111, 61.875)),
    Random4DTo1D(value,17635.1739, vec4(44.383, 38.174, 67.688, 22.351))	
  );
}

#endif
