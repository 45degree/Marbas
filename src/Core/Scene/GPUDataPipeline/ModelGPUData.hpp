#pragma once

#include "AssetManager/Mesh.hpp"
#include "AssetManager/ModelAsset.hpp"
#include "TextureGPUData.hpp"

namespace Marbas {

class MeshGPUData {
  friend class ModelGPUData;

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
  MeshGPUData(const Mesh& mesh, RHIFactory* rhiFactory);
  ~MeshGPUData();

  static const DescriptorSetArgument&
  GetDescriptorSetArgument();

 private:
  void
  Update(const Mesh& mesh);

  void
  UpdateMaterial(const Mesh& mesh);

  void
  UpdateMaterialInfo(const Mesh& mesh);

  void
  CreateAndBindDescriptorSet();

 private:
  RHIFactory* m_rhiFactory = nullptr;
};

class ModelGPUData final : public GPUDataPipelineDataBase {
  using Asset = ModelAsset;

 public:
  Task<>
  Load(const Asset& asset);

  Task<>
  Update(const Asset& asset);

 public:
  MeshGPUData*
  GetMeshGPU(size_t index) {
    return m_meshGPU[index].get();
  }

  size_t
  MeshCount() const {
    return m_meshGPU.size();
  }

 private:
  std::vector<std::unique_ptr<MeshGPUData>> m_meshGPU;
};

using ModelGPUDataManager = Singleton<GPUDataPipelineFromAssetBase<ModelGPUData>>;

}  // namespace Marbas
