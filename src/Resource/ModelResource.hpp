#pragma once

#include "Common/Model.hpp"
#include "Resource/ResourceBase.hpp"

namespace Marbas {

class ModelResource final : public ResourceBase {
 public:
  explicit ModelResource(const Path& modelPath) : ResourceBase(), m_modelPath(modelPath) {}

 public:
  /**
   * @berif read model from trandition file such as .obj
   */
  void
  LoadResource(RHIFactory* rhiFactory, std::shared_ptr<ResourceManager>& resourceManager) override;

  std::shared_ptr<Model>
  GetModel() {
    return m_model;
  }

 private:
  const Path m_modelPath;
  std::shared_ptr<Model> m_model;
};

}  // namespace Marbas
