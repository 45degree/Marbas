#pragma once

#include "AssetManager/Mesh.hpp"
#include "Common/MathCommon.hpp"
#include "Core/Scene/GPUDataPipeline/TextureGPUData.hpp"
#include "Core/Scene/Scene.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

class MeshGPUData : public GPUDataPipelineDataBase {
 public:
  Buffer* m_vertexBuffer = nullptr;
  Buffer* m_indexBuffer = nullptr;
  size_t m_indexCount = 0;

  Buffer* m_materialInfoBuffer = nullptr;
  Buffer* m_transformBuffer = nullptr;

  uintptr_t m_descriptorSet;
  uintptr_t m_sampler;
  std::shared_ptr<TextureGPUData> m_diffuseTexture = nullptr;
  std::shared_ptr<TextureGPUData> m_aoTexture = nullptr;
  std::shared_ptr<TextureGPUData> m_metallicTexture = nullptr;
  std::shared_ptr<TextureGPUData> m_normalTexture = nullptr;
  std::shared_ptr<TextureGPUData> m_roughnessTexture = nullptr;

 private:
  struct MaterialInfo {
    glm::ivec4 texInfo = glm::vec4(0);
    glm::vec4 diffuseColor = glm::vec4(0);
    glm::float32 metallicValue;
    glm::float32 roughnessValue;
  } m_materialInfo;
  static Image* m_emptyImage;
  static ImageView* m_emptyImageView;

 public:
  MeshGPUData();
  ~MeshGPUData();

  static const DescriptorSetArgument&
  GetDescriptorSetArgument();

  Task<>
  Load(Mesh& mesh);

  Task<>
  Update(Mesh& mesh);

 private:
  void
  UpdateMaterial(Mesh& mesh);

  void
  UpdateMaterialInfo(const Mesh& mesh);

  void
  CreateAndBindDescriptorSet();
};

using MeshGPUDataManager = Singleton<GPUDataPipelineBase<entt::entity, MeshGPUData>>;

}  // namespace Marbas
