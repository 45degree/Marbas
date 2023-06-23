#pragma once

#include "AssetManager/Mesh.hpp"
#include "Common/MathCommon.hpp"
#include "Core/Scene/GPUDataPipeline/TextureGPUData.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

struct MeshRenderComponent {
  Buffer* m_vertexBuffer = nullptr;
  Buffer* m_indexBuffer = nullptr;
  size_t m_indexCount = 0;
  uintptr_t m_descriptorSet;

 public:
  MeshRenderComponent(RHIFactory* rhiFactory, uintptr_t sampler, ImageView* emptyImageView);
  ~MeshRenderComponent();

 public:
  void
  Init(Mesh& mesh);

  void
  Update(Mesh& mesh);

  static const DescriptorSetArgument&
  GetDescriptorSetArgument();

 private:
  void
  CreateMeshDescriptorSet(const Mesh& mesh);

  void
  CreateMeshVertexIndexData(const Mesh& mesh);

  void
  CreateMaterialBuffer(Mesh& mesh);

  void
  UpdateMaterialInfo(const Mesh& mesh);

  void
  UpdateMaterial(Mesh& mesh);

 private:
  struct MaterialInfo {
    glm::ivec4 texInfo = glm::vec4(0);
    glm::vec4 diffuseColor = glm::vec4(0);
    glm::float32 metallicValue;
    glm::float32 roughnessValue;
  } m_materialInfo;
  Buffer* m_materialInfoBuffer = nullptr;
  RHIFactory* m_rhiFactory = nullptr;

  uintptr_t m_externSampler;
  ImageView* m_externEmptyImageView;

  std::shared_ptr<TextureGPUData> m_diffuseTexture = nullptr;
  std::shared_ptr<TextureGPUData> m_aoTexture = nullptr;
  std::shared_ptr<TextureGPUData> m_metallicTexture = nullptr;
  std::shared_ptr<TextureGPUData> m_normalTexture = nullptr;
  std::shared_ptr<TextureGPUData> m_roughnessTexture = nullptr;
};

}  // namespace Marbas
