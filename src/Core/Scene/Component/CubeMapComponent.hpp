#pragma once

#include <memory>

#include "Common/Common.hpp"
#include "Common/MathCommon.hpp"
#include "Common/Vertex.hpp"
#include "RHI/RHI.hpp"
#include "Resource/ResourceManager.hpp"
#include "Tool/Uid.hpp"

namespace Marbas {

struct CubeMapComponent_Impl {
  std::shared_ptr<VertexBuffer> vertexBuffer;
  std::shared_ptr<IndexBuffer> indexBuffer;
  std::shared_ptr<DescriptorSet> descriptorSet;
  std::shared_ptr<TextureCubeMapResource> textureCubeMapResource;
};

struct CubeMapComponent {
  static constexpr std::array<Vertex, 8> vertices = {
      Vertex{.posX = -1, .posY = -1, .posZ = 1},  Vertex{.posX = 1, .posY = -1, .posZ = 1},
      Vertex{.posX = 1, .posY = 1, .posZ = 1},    Vertex{.posX = -1, .posY = 1, .posZ = 1},
      Vertex{.posX = -1, .posY = -1, .posZ = -1}, Vertex{.posX = 1, .posY = -1, .posZ = -1},
      Vertex{.posX = 1, .posY = 1, .posZ = -1},   Vertex{.posX = -1, .posY = 1, .posZ = -1},
  };

  static constexpr std::array<uint32_t, 36> indices = {3, 0, 1, 3, 1, 2, 2, 1, 6, 6, 1, 5,
                                                       6, 3, 2, 7, 3, 6, 1, 0, 4, 1, 4, 5,
                                                       7, 0, 3, 0, 7, 4, 4, 7, 5, 7, 6, 5};

  std::optional<Uid> cubeMapResourceId = std::nullopt;
  std::shared_ptr<CubeMapComponent_Impl> m_implData = nullptr;
};

}  // namespace Marbas
