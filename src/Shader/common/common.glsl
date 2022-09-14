layout(std140, binding = 0) uniform commonUniform {
  float time;
  mat4 view;
  mat4 projection;
  vec2 VIEWPORT_SIZE; // Size of viewport (in pixels).
}

// in vec2 VIEWPORT_SIZE
//
// 视区大小(以像素为单位).
//
// inout mat4 WORLD_MATRIX
//
// 模型空间到世界空间变换.
//
// in mat4 INV_CAMERA_MATRIX
//
// 世界空间向视图空间转变。
//
// inout mat4 PROJECTION_MATRIX
//
// 视图空间向裁剪空间变换.
//
// in mat4 CAMERA_MATRIX
//
// 视图空间向世界空间变换.
//
// inout mat4 MODELVIEW_MATRIX
//
// 模型空间向视图空间变换(如果可用).
//
// inout mat4 INV_PROJECTION_MATRIX
//
// 裁剪空间向视图空间变换。
//
// inout vec3 VERTEX
//
// 局部坐标中的顶点.
//
// out vec4 POSITION
//
// 如果写入, 则覆盖最终顶点位置.
//
// inout vec3 NORMAL
//
// 局部坐标法线.
//
// inout vec3 TANGENT
//
// 局部坐标切线.
//
// inout vec3 BINORMAL
//
// 局部坐标次法线.
//
// out float ROUGHNESS
//
// 顶点照明的粗糙度.
//
// inout vec2 UV
//
// UV主通道.
//
// inout vec2 UV2
//
// UV2辅助通道.
//
// in bool OUTPUT_IS_SRGB
//
// 当计算发生在sRGB色彩空间时为 true (GLES2为 true ,GLES3为 false ).
//
// inout vec4 COLOR
//
// 顶点颜色.
//
// inout float POINT_SIZE
//
// 点渲染的点大小.
//
// in int INSTANCE_ID
//
// 实例化的实例ID.
//
// in vec4 INSTANCE_CUSTOM
//
// 实例自定义数据(主要用于粒子).
