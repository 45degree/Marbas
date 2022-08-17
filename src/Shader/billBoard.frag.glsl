#version 440

layout(location = 0) in vec2 aTex;
layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D billBoardTexture;

void main() {
  vec4 color = texture(billBoardTexture, aTex);
  FragColor = color;
}
