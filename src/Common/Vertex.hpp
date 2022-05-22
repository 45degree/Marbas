#pragma once

namespace Marbas {

#pragma pack(1)
extern "C" struct Vertex {
  float posX = 0.f, posY = 0.f, posZ = 0.f;
  float normalX = 0.f, normalY = 0.f, normalZ = 0.f;
  float textureU = 0.f, textureV = 0.f;
};
#pragma pack()


}  // namespace Marbas
