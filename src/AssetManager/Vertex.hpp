#pragma once

namespace Marbas {

#pragma pack(1)
struct Vertex {
  float posX = 0.f, posY = 0.f, posZ = 0.f;
  float normalX = 0.f, normalY = 0.f, normalZ = 0.f;
  float textureU = 0.f, textureV = 0.f;
  float tangentX = 1.0f, tangentY = 0.0f, tangentZ = 0.0f;
  float bitangentX = 0.0f, bitangentY = 1.0f, bitangentZ = 0.0f;

  template <typename Archive>
  void
  serialize(Archive& ar) {
    ar(posX, posY, posZ,                   //
       normalX, normalY, normalZ,          //
       textureU, textureV,                 //
       tangentX, tangentY, tangentZ,       //
       bitangentX, bitangentY, bitangentZ  //
    );
  }
};
#pragma pack()

}  // namespace Marbas
