#pragma once

#include "AssetManager.hpp"
#include "AssetManager/GPUAssetUpLoader.hpp"
#include "AssetManager/TextureAsset.hpp"
#include "Model.hpp"
#include "RHIFactory.hpp"
#include "cereal/types/vector.hpp"

namespace Marbas {

class ModelAsset final : public AssetBase {
  friend class ModelGPUAsset;

 private:
  std::vector<Mesh> m_model;
  std::string m_modelName = "";

 public:
  template <typename Archive>
  void
  serialize(Archive& ar) {
    ar(cereal::base_class<AssetBase>(this), m_modelName, m_model);
  }

  void
  SetModelName(const String& modelName) {
    m_modelName = modelName;
  }

  Mesh&
  GetMesh(size_t index) {
    return m_model[index];
  }

  const Mesh&
  GetMesh(size_t index) const {
    return m_model[index];
  }

  size_t
  GetMeshCount() const {
    return m_model.size();
  }

  static std::shared_ptr<ModelAsset>
  Load(const Path& path);
};

class ModelGPUAsset final : public GPUAssetBase<ModelGPUAsset> {
  friend class GPUAssetBase<ModelGPUAsset>;

 public:
  class MeshGPUAsset {
    friend class ModelGPUAsset;

   public:
    Buffer* m_vertexBuffer = nullptr;
    Buffer* m_indexBuffer = nullptr;
    size_t m_indexCount = 0;

    struct MaterialInfo {
      alignas(4) glm::uint hasDiffuseTex = 0;
      alignas(4) glm::uint hasNormalTex = 0;
      alignas(4) glm::uint hasAoTex = 0;
      alignas(4) glm::uint hasRoughnessTex = 0;
      alignas(4) glm::uint hasMetallicTex = 0;
      // TODO
      alignas(16) glm::vec4 diffuseColor = glm::vec4(0);
      glm::float32 metallicValue;
      glm::float32 roughnessValue;
    } m_materialInfo;
    Buffer* m_materialInfoBuffer = nullptr;
    Buffer* m_transformBuffer = nullptr;

    std::shared_ptr<TextureGPUAsset> m_diffuseTexture = nullptr;
    std::shared_ptr<TextureGPUAsset> m_aoTexture = nullptr;
    std::shared_ptr<TextureGPUAsset> m_metallicTexture = nullptr;
    std::shared_ptr<TextureGPUAsset> m_normalTexture = nullptr;
    std::shared_ptr<TextureGPUAsset> m_roughnessTexture = nullptr;

    MeshGPUAsset(const Mesh& mesh, RHIFactory* rhiFactory);
    ~MeshGPUAsset();

   private:
    void
    Update(const Mesh& mesh);

   private:
    RHIFactory* m_rhiFactory = nullptr;
  };
  Vector<std::shared_ptr<MeshGPUAsset>> m_meshGPUAsset;

  ModelGPUAsset(const std::shared_ptr<ModelAsset>& modelAsset, RHIFactory* rhiFactory);

  static std::shared_ptr<ModelGPUAsset>
  LoadToGPU(const std::shared_ptr<ModelAsset>& modelAsset, RHIFactory* rhiFactory) {
    return std::make_shared<ModelGPUAsset>(modelAsset, rhiFactory);
  }

 private:
  void
  Update(const std::shared_ptr<ModelAsset>& modelAsset);
};

}  // namespace Marbas
