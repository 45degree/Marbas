#pragma once

#include "AssetManager.hpp"
#include "Model.hpp"
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
  Load(const AssetPath& path);
};

}  // namespace Marbas
