#include "Core/Scene/Component/MeshComponent.hpp"

#include "Common/Common.hpp"
#include "Core/Common.hpp"
#include "RHI/Interface/RHIFactory.hpp"
#include "RHI/Interface/VertexBuffer.hpp"

namespace Marbas {

void
MeshComponent::LoadMeshImpl(RHIFactory* rhiFactory) {
  if (m_mesh == nullptr) return;

  const auto& vertices = m_mesh->m_vertices;
  auto verticesLen = sizeof(Vertex) * vertices.size();
  auto vertexBuffer = rhiFactory->CreateVertexBuffer(vertices.data(), verticesLen);
  vertexBuffer->SetLayout(GetMeshVertexInfoLayout());
  m_impldata->vertexBuffer = std::move(vertexBuffer);

  const auto& indices = m_mesh->m_indices;
  auto indexBuffer = rhiFactory->CreateIndexBuffer(indices);
  m_impldata->indexBuffer = std::move(indexBuffer);
}

}  // namespace Marbas
