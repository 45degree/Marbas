#pragma once

#include <cstdint>

#include "Common/Common.hpp"
#include "Common/MathCommon.hpp"
#include "Common/Mesh.hpp"
#include "Common/Vertex.hpp"
#include "Resource/MaterialResource.hpp"
#include "Tool/Uid.hpp"

namespace Marbas {

struct MeshComponent_Impl;
struct Model;
struct MeshComponent {
  std::shared_ptr<Mesh> m_mesh = nullptr;
  std::weak_ptr<Model> m_model;

  struct UniformBufferBlockData {
    alignas(16) glm::mat4 model = glm::mat4(1.0);
    alignas(16) glm::mat4 view = glm::mat4(1.0);
    alignas(16) glm::mat4 projective = glm::mat4(1.0);
  } m_uniformBufferData;

  std::shared_ptr<MeshComponent_Impl> m_impldata = nullptr;
};

}  // namespace Marbas
