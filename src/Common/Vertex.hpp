#pragma once

namespace Marbas {

#pragma pack(1)
extern "C" struct Vertex {
  float posX = 0.f, posY = 0.f, posZ = 0.f;
  float normalX = 0.f, normalY = 0.f, normalZ = 0.f;
  float textureU = 0.f, textureV = 0.f;
  float tangentX = 1.0f, tangentY = 0.0f, tangentZ = 0.0f;
  float bitangentX = 0.0f, bitangentY = 1.0f, bitangentZ = 0.0f;
};
#pragma pack()

}  // namespace Marbas
