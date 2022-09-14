#pragma once

#include <cstdint>

#include "Common/Common.hpp"
#include "Common/MathCommon.hpp"
#include "Common/Mesh.hpp"
#include "Common/Vertex.hpp"
#include "Resource/MaterialResource.hpp"
#include "Tool/Uid.hpp"

namespace Marbas {

struct Model;
struct MeshComponent_Impl {
  std::shared_ptr<VertexBuffer> vertexBuffer;
  std::shared_ptr<IndexBuffer> indexBuffer;
  std::shared_ptr<DescriptorSet> descriptorSet;
  std::shared_ptr<MaterialResource> materialResource;
};

struct MeshComponent {
  std::shared_ptr<Mesh> m_mesh = nullptr;
  std::weak_ptr<Model> m_model;

  struct UniformBufferBlockData {
    alignas(16) glm::mat4 model = glm::mat4(1.0);
    alignas(4) bool hasNormalTex = false;
    alignas(4) bool hasAOTex = false;
    alignas(4) bool hasRoughnessTex = false;
    alignas(4) bool hasMetallicTex = false;
  } m_uniformBufferData;

  std::shared_ptr<MeshComponent_Impl> m_impldata = nullptr;

  void
  LoadMeshImpl(RHIFactory* rhiFactory);
};

}  // namespace Marbas
