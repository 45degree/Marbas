#ifndef MARBAS_RESOURCE_MATERIAL_RESOURCE_HPP
#define MARBAS_RESOURCE_MATERIAL_RESOURCE_HPP

#include <glog/logging.h>

#include <set>

#include "Resource/ResourceBase.hpp"
#include "Resource/ShaderResource.hpp"
#include "Resource/TextureResource.hpp"

namespace Marbas {

class ResourceManager;
class MaterialResource final : public ResourceBase {
 public:
  explicit MaterialResource(std::unique_ptr<Material>&& material)
      : ResourceBase(), m_material(std::move(material)) {}

 public:
  void SetShader(const Uid& shaderResourceId) { m_shaderResource = shaderResourceId; }

  void AddAmbientTexture(const Uid& ambientTextureId) {
    m_ambientTextureUids.insert(ambientTextureId);
  }

  void AddAmbientTexture(const Vector<Uid>& ambientTextureIds) {
    m_ambientTextureUids.insert(ambientTextureIds.begin(), ambientTextureIds.end());
  }

  void AddDiffuseTexture(const Uid& diffuseTextureId) {
    m_diffuseTextureUids.insert(diffuseTextureId);
  }

  void AddDiffuseTextures(const Vector<Uid>& diffuseTextureIds) {
    m_diffuseTextureUids.insert(diffuseTextureIds.begin(), diffuseTextureIds.end());
  }

  Material* GetMaterial() const noexcept {
    if (!m_isLoad) return nullptr;

    return m_material.get();
  }

  [[nodiscard]] Material* LoadMaterial(ResourceManager* resourceManager) const;

 private:
  std::unordered_set<Uid> m_diffuseTextureUids;
  std::unordered_set<Uid> m_ambientTextureUids;
  Uid m_shaderResource;
  mutable std::unique_ptr<Material> m_material;
};

}  // namespace Marbas

#endif
